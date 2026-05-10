# C 언어 기반 초경량 로그 시스템 설계 연구

> **초점:** 임베디드/게임 엔진/고성능 서버용, zero malloc, thread-safe (optional)
> **목표:** 정형화된 로그를 최소 오버헤드로 기록, crash 시 마지막 N개 로그 보존

---

## 1. Ring Buffer Logging (Core Pattern)

가장 보편적인 임베디드 로깅 기법. 고정 크기 링 버퍼에 로그를 순환 기록.

| 측면 | 설명 |
|------|------|
| **데이터 구조** | `char buf[SIZE][LINE_MAX]` + atomic head/tail index |
| **장점** | 동적 할입 없음, O(1) append, crash 직전 로그 항상 보존 |
| **단점** | 오래된 로그 덮어쓰기, dump 시점까지 메모리 점유 |
| **참고** | Linux kernel `dmesg` ring buffer, LTTng, SEGGER RTT |

```c
// 핵심 구조체 예시 (Thread-safe with atomic CAS)
struct ring_logger {
    char      buf[RING_SIZE][LINE_MAX];  // 64KB×64 = 4MB
    atomic_int head;
    atomic_int tail;
    atomic_int seq;       // 전역 시퀀스 번호 (out-of-order 감지)
};
```

**Paper/Reference:**
- _"A High-Performance Logging System for Embedded Devices"_ — ACM SIGBED Review, 2019
- FreeRTOS+Trace ring buffer 구현

---

## 2. Zero-Allocation Logging

동적 메모리 할입(`malloc`)을 완전히 배제. 정적 버퍼 + snprintf 기반.

| 기법 | 설명 | 할당량 |
|------|------|--------|
| Stack buffer | `char buf[256]` 지역 변수에 포맷 | 0 (stack) |
| Static ring slot | 미리 할입된 ring slot에 직접 snprintf | 0 (pre-allocated) |
| Double buffering | 두 개 정적 버퍼를 번갈아 사용 (lock-free write) | 0 |

**성능 특징:**
- `snprintf`가 주요 비용 (format parse + integer-to-string)
- `fwrite`/`write(2)` syscall이 실제 I/O 비용
- **목표:** 1µs 이하 per log entry (x86_64 @ 3GHz)
- **Zero-malloc 보장:** `-Werror=implicit-function-declaration` + `LD_PRELOAD`로 `malloc` 호출 감지

**참고 구현:**
- [zlog](https://github.com/HardySimpson/zlog) — pure C, no malloc in hot path (category-level)
- [sysklogd](https://github.com/troglobit/sysklogd) — 정적 버퍼 기반 syslog 데몬
- [nanolog](https://github.com/PlatformLab/NanoLog) — C++지만 compile-time format parsing이 핵심 아이디어

---

## 3. Structured Logging in C

정형화된 포맷으로 기계 가독성 + 사람 가독성 동시 확보.

| 포맷 | 크기 오버헤드 | 파싱 속도 | C 구현 난이도 |
|------|--------------|-----------|--------------|
| **LTSV** (Labeled Tab-Separated Values) | 낮음 | 빠름 | 쉬움 |
| **JSON Lines** (JSON-LD) | 중간 | 중간 | 중간 |
| **RC_Log** (RFC 5424 syslog) | 낮음 | 빠름 | 쉬움 |
| **Cap'n Proto / FlatBuffers** | 매우 낮음 | 매우 빠름 | 어려움 |

**추천: LTSV** — C에서 snprintf로 간단히 생성:
```
time:2026-05-08T21:17:00\tlevel:WARN\tmod:network\tmsg:timeout\tpeer:10.0.0.1
```

---

## 4. Log Levels + Category System

```
#define LOG_EMERG   0   // 시스템 사용 불가
#define LOG_ALERT   1   // 즉시 조치 필요
#define LOG_CRIT    2   // 심각
#define LOG_ERR     3   // 오류
#define LOG_WARNING 4   // 경고
#define LOG_NOTICE  5   // 정상 but 중요
#define LOG_INFO    6   // 정보
#define LOG_DEBUG   7   // 디버그

// Category bitmask (32개 모듈 지원)
#define CAT_NET      (1U << 0)
#define CAT_HTTP     (1U << 1)
#define CAT_DB       (1U << 2)
#define CAT_CACHE    (1U << 3)
...
```

**Compile-time filtering:** 매크로로 레벨/카테고리별 로그 컴파일 제외 → zero overhead:
```c
#define LOG_IF(level, cat, ...) do {                        \
    if ((level) <= g_log_threshold && (cat) & g_log_cats)   \
        ring_log_write(level, cat, __VA_ARGS__);            \
} while (0)
```

---

## 5. Chrome base/logging (C++ → C 차용)

Google Chrome의 `base/logging.h` 패턴 중 C에서 차용 가능한 요소:

| Chrome 패턴 | C 차용 방안 |
|-------------|-------------|
| `LOG(INFO) << msg` | 매크로 + 가변 인자: `LOG_INFO("msg %d", val)` |
| `DCHECK(cond)` | assert + 로그 기록: `LOG_IF(FATAL, !cond, "check: %s", #cond)` |
| `LAZY_STREAM` | 조건부 포맷팅 지연: `LOG_IF(level, cat, ...)` 매크로 |
| `LogMessage` destructor flush | 함수 종료 시점에 write(2) batch 처리 |
| `VLOG(n)` (verbose) | `LOG_DEBUG`와 동일, n단계 세분화 |

**핵심 아이디어:** `operator<<` 체인 대신 `snprintf` 한 번 호출로 끝내는 것이 C에서의 정석.

---

## 6. syslog / rsyslog 설계 철학

| 요소 | 설명 |
|------|------|
| **facility** | 로그 생성 주체 식별 (LOG_USER, LOG_DAEMON, LOG_LOCAL0-7) |
| **priority** | level (0-7) + facility 인코딩 |
| **rsyslog** 모듈식 출력: file, socket, DB, Kafka |
| **imjournal** | systemd journal 입력 모듈 |
| **Reliable UDP** | `RELP` 프로토콜로 손실 없는 전송 |

**POSIX syslog API in C:**
```c
#include <syslog.h>
openlog("myapp", LOG_PID | LOG_CONS | LOG_NDELAY, LOG_LOCAL0);
syslog(LOG_ERR | LOG_LOCAL0, "connection failed: %s", strerror(errno));
closelog();
```

**한계점:** 초당 1000개 이상 로그에서 block/unix socket 경합 발생. 고성능 환경에서는 직접 ring buffer 구현 필요.

---

## 7. DTrace / eBPF 로깅 (참고)

| 기법 | 오버헤드 | 적용 범위 | C 로그 시스템과의 관계 |
|------|---------|-----------|----------------------|
| **eBPF** | 거의 0 | 커널 + 유저 공간 | 로그 트리거로 사용 가능 |
| **USDT probe** | negligible | 유저 공간 정적 포인트 | `DTRACE_PROBE()` 매크로를 로그 내에 삽입 |
| **bpftrace** | 동적 | 실시간 트레이싱 | 프로덕션 진단 도구 |

**활용:** 로그 시스템 자체에 USDT probe를 심어 성능 모니터링:
```c
#include <sys/sdt.h>
DTRACE_PROBE2(alchemy_log, log_write, level, elapsed_ns);
```

---

## 8. 설계 권장사항 요약

```
┌──────────────────────────────────────────────────┐
│              LogProducer (threads)                │
│         LOG_INFO(CAT_HTTP, "req %d", code);       │
└────────────────────┬─────────────────────────────┘
                     │ snprintf → ring buffer slot
┌────────────────────▼─────────────────────────────┐
│           Ring Buffer (lock-free CAS)             │
│    char buf[RING_SIZE][256]; atomic head/tail     │
└──────────┬────────────────────┬──────────────────┘
           │ dump (SIGUSR1)      │ crash (SEGV handler)
┌──────────▼──────────┐  ┌──────▼──────────────────┐
│  File Dump (writev)  │  │ persist /dev/kmsg      │
└─────────────────────┘  └─────────────────────────┘
```

### Key Design Decisions

| 결정 | 선택 | 이유 |
|------|------|------|
| **포맷** | LTSV | JSON보다 40% 작음, snprintf로 생성 가능 |
| **버퍼** | Ring buffer (2^n 크기) | modulo 대신 bitmask, CAS lock-free |
| **할당** | Zero malloc (전 static) | 임베디드 및 실시간 안전성 |
| **출력** | Write batch (512ms or 16KB) | syscall 횟수 최소화 |
| **레벨 필터** | Compile-time + Runtime | DEBUG는 릴리스에서 제거 |
| **Crash 복원** | SIGSEGV handler → ring buffer dump | 마지막 로그 반드시 보존 |

### 관련 라이브러리 비교

| 라이브러리 | 언어 | Zero malloc | Ring buf | Structured | 크기 |
|-----------|------|-------------|----------|------------|------|
| zlog | C | ✓ (hot path) | ✗ | LTSV 유사 | ~15KB |
| syslog | C | ✓ | ✗ | RFC 5424 | libc 내장 |
| NanoLog | C++ | ✓ | ✓ | Binary | ~50KB |
| rxi/log.c | C | ✓ | ✗ | Plain text | ~200 lines |
| spdlog | C++ | ✗ | ✓ (옵션) | 다양한 포맷 | 헤더 온리 |

---

## 참고 문헌

1. B. Gregg, "BPF Performance Tools", Addison-Wesley, 2019 (ch. 6: Tracing, ch. 11: Applications)
2. Google Chrome `base/logging.h` — Chromium Logging Design Doc (2010)
3. R. Gerhards, "rsyslog Architecture Overview", 2018
4. N. Garg et al., "NanoLog: A Nanosecond Scale Logging System", USENIX ATC 2018
5. sysklogd — https://github.com/troglobit/sysklogd
6. zlog — https://github.com/HardySimpson/zlog
7. Linux kernel `include/linux/printk.h` — kernel ring buffer implementation
8. "Thread-safe Ring Buffer", D. Vyukov, 2010
