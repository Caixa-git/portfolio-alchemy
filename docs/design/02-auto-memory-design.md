# Auto Memory Learning — alchemy-c Persistent Memory System

> Phase B: 세션 간 자동 학습 + Ebbinghaus 망각 곡선 + FTS5 검색
> 설계일: 2026-05-09

---

## 1. Design Philosophy

"Memory should feel like remembering, not searching."

Claude Code의 `.claude/memory/MEMORY.md` 자동 저장 + Hermes의 `session_search` FTS5 검색을 결합.  
여기에 MemoryBank의 Ebbinghaus 망각 곡선 + HeLa-Mem의 연관 그래프를 더한다.

### Math + Beauty

| 원칙 | 적용 |
|------|------|
| **Ebbinghaus 망각 곡선** | `R = e^(-t/S)` — S는 기억 강도. 자주 언급되는 사실은 S가 큼 (천천히 망각) |
| **Claude Code MEMORY.md** | `$ALCHEMY_DIR/memory/` 아래에 토픽별 .mem 파일 |
| **Hermes FTS5** | SQLite FTS5로 전문 검색 (C에서 직접 구현: strstr + mmap) |
| **HeLa-Mem 연관** | "A → B" 형태의 연관 그래프. A를 물어보면 B도 함께 제공 |
| **최소 권한** | 25KB 제한 (Claude Code 동일). 초과 시 LRU 가지치기 |

### Benchmark 채택/거절

| 출처 | 채택 | 거절 |
|------|------|------|
| **Claude Code** | MEMORY.md 토픽 파일 + 200줄/25KB 제한 + toggleable | JSON 포맷 (바이너리가 더 효율적) |
| **Hermes Agent** | FTS5 검색 + frozen snapshot + context fencing | Python 의존 (C용 직접 구현) |
| **MemoryBank** | Ebbinghaus 망각 곡선 `R=e^(-t/S)` | 전체 시스템이 LLM에 의존 (offline 불가) |
| **MemGPT/Letta** | Fast/slow memory tiers | OS 수준 가상 메모리 (C에 과함) |
| **Zep Graphiti** | — | Neo4j/RDB 의존 (너무 무거움) |
| **HeLa-Mem** | Hebbian 연관 그래프 | 가중치 업데이트 복잡도 |

---

## 2. 메모리 계층

### 3-Tier Architecture

```
Level 0: Session Memory (alchemy_memory.c)  ← 기존
  ring buffer, 16개 메시지, 바이너리 v2 포맷
  → 세션 컨텍스트 유지용 (프로세스 재시작 후 복원)

Level 1: Episodic Memory (신규)  ← Auto Memory Learning 핵심
  토픽별 .mem 파일 (텍스트), FTS5 인덱스 (mmap)
  → "사용자가 선호하는 것", "이전에 한 결정" 저장

Level 2: Semantic Memory (신규)
  사실/규칙/선호도 — Ebbinghaus 망각 곡선 적용
  → "이 프로젝트는 C99 사용", "사용자는 JSON보다 ini 선호"
```

### 저장 포맷 (Level 1: Episodic)

```c
/* $ALCHEMY_DIR/memory/<topic_hash>.mem
 *
 * 포맷: UTF-8 text, RFC 822-like 헤더 + 바디
 * ---
 * Topic: build-system
 * Created: 2026-05-09T10:30:00Z
 * Accessed: 2026-05-09T11:00:00Z
 * Strength: 1.0          ← Ebbinghaus S 값 (0.0~2.0)
 * Tags: makefile, cmake, build
 * Related: compiler-flags, dependencies
 * ---
 * User prefers single Makefile over CMake.
 * Uses gcc with -Wall -Wextra -g flags.
 * Build output goes to build/ directory.
 */
```

### 저장 포맷 (Level 2: Semantic)

```c
/* $ALCHEMY_DIR/semantic/<fact_hash>.sem
 *
 * 포맷: 단일 사실 (key=value)
 * ---
 * Key: project.language
 * Value: C99
 * Strength: 2.0       ← 강한 확신 (직접 말함)
 * Source: session_20260509_1030
 * ---
 * Key: build.tool
 * Value: make
 * Strength: 0.7       ← 약한 확신 (관찰만)
 * Source: session_20260509_1030
 * ---
 * Key: style.prefer
 * Value: minimal ANSI, no ncurses
 * Strength: 1.5
 * Source: session_20260508_1500
 */
```

---

## 3. 자동 추출 (Auto-Extraction)

### Trigger 조건 (언제 저장하는가)

| 조건 | 예 | 행동 |
|------|----|------|
| **명시적 선호** | "나는 X를 선호해" | 즉시 Semantic Memory 저장 (Strength=2.0) |
| **반복 패턴** | 3회 연속 같은 인자로 make 실행 | Episodic Memory 저장 (Strength=0.5) |
| **수정 결정** | "이거 다시 고쳐", "이건 이렇게" | Episodic Memory 저장 (Strength=0.8) |
| **에러/경고** | "rm -rf는 위험해" | Semantic Memory 저장 (Strength=1.0) |
| **프로젝트 규칙** | "이 프로젝트는 C99야" | Semantic + Episodic 저장 (Strength=1.5) |

### 추출 알고리즘

```c
// LLM 응답에서 메모리 후보 추출
// 간단한 패턴 매칭 (정규식 없이 strstr 기반):
//
// 1. "나는 X를 선호해" / "I prefer X"
//    → semantic_store("preference.X", "X", strength=2.0)
//
// 2. "이 프로젝트는 X" / "this project uses X"
//    → semantic_store("project.property", "X", strength=1.5)
//
// 3. 3회 이상 반복된 tool 호출 패턴
//    → episodic_store("tool.pattern", description, strength=0.5)
//
// 4. 사용자가 명시적으로 "기억해줘" / "remember"
//    → 즉시 저장 (strength=2.0, source="explicit")

int memory_auto_extract(const char *llm_response);
```

### 망각 곡선 (Ebbinghaus)

```c
// R = e^(-t/S)
//
// R: 기억 유지 확률 (0.0 ~ 1.0)
// t: 마지막 접근 이후 경과 시간 (일)
// S: 기억 강도 (0.1 ~ 2.0)
//
// S 값 예:
//   2.0 — 사용자가 직접 말함 (명시적) → 1일 후 R = e^(-1/2) = 0.607
//   1.0 — 관찰 (1회)                 → 1일 후 R = e^(-1/1) = 0.368
//   0.5 — 추측 (약한 패턴)           → 1일 후 R = e^(-1/0.5) = 0.135
//
// S 증가 조건:
//   - 동일 사실이 다시 언급됨: S += 0.3 (강화)
//   - 사용자가 "맞아" 확인: S += 0.5
//   - 관련 질문에 사용됨: S += 0.1 (사용 효과)
//
// S 감소 조건:
//   - 하루 지남: S *= 0.99 (서서히 감소)
//   - 반대 사실 언급: S *= 0.5

double memory_recall_probability(double strength, double days_since_access);
void  memory_reinforce(const char *key);  // S += delta
void  memory_decay_all(void);             // 매일 1회 호출
```

### 가지치기 (Pruning)

```c
// 25KB 제한 (Claude Code 동일)
//
// 가지치기 우선순위:
// 1. Strength < 0.3 → 즉시 삭제 (망각)
// 2. R < 0.1 → 삭제 후보
// 3. 25KB 초과 시 R 가장 낮은 것부터 삭제
//
// 컴팩션: 7일마다 strength < 0.5 + accessed > 30일 = 삭제
```

---

## 4. 검색 (Recall)

### 검색 우선순위

```c
// 메모리 검색 호출 시:
//
// 1. Exact match (strcmp): key 정확 일치 → 즉시 반환
// 2. Prefix match (strncmp): "build." → 모든 build.* 반환
// 3. FTS5 (strstr + mmap): 바디에서 키워드 검색
// 4. 연관 그래프: "A" 검색 시 "A → B" 관계의 B도 함께 반환
// 5. 세션 검색: 현재 세션의 ring buffer 검색 (Level 0)
//
// 결과: Strength 내림차순 정렬, 상위 5개만 시스템 프롬프트에 주입

int memory_recall(const char *query, char *out, int max_out);
// out 포맷:
// [Memory: build-system (S=1.5)]
// User prefers single Makefile over CMake.
//
// [Memory: compiler-flags (S=1.0)]
// Uses gcc with -Wall -Wextra -g flags.
```

### Context Injection

```c
// 시스템 프롬프트에 메모리 주입:
//
// ── Persistent Memory (auto-learned) ──
// [build-system] User prefers single Makefile over CMake.
// [compiler-flags] Uses gcc with -Wall -Wextra -g.
// [tool.pattern] User runs "make" 3 times before checking output.
//
// 최대 1024바이트 (25KB 중 일부)
// recall 결과가 1024B 초과 시 Strength 높은 순으로 자름
```

---

## 5. 파일 구조

```
$ALCHEMY_DIR/
└── memory/
    ├── episodic/           ← Level 1: .mem 파일들
    │   ├── <hash1>.mem
    │   └── <hash2>.mem
    ├── semantic/           ← Level 2: .sem 파일들
    │   ├── <hash1>.sem
    │   └── <hash2>.sem
    └── index.bin           ← FTS5 인덱스 (mmap, 32KB 고정)
```

### C 구현 파일

```
library/algo/
├── alchemy_memory_auto.h   ← 신규
└── alchemy_memory_auto.c   ← 신규 (~300줄 예상)

src/core/
├── alchemy_memory.c        ← 기존 (변경 없음, 세션 컨텍스트용)
└── alchemy_memory.h        ← 기존
```

---

## 6. API 설계

```c
// alchemy_memory_auto.h

/* ── Episodic Memory (Level 1) ── */
int  mem_auto_init(const char *base_dir);        // $ALCHEMY_DIR/memory/ 생성
void mem_auto_store(const char *topic,
                    const char *content,
                    double strength,
                    const char *tags);            // 자동 저장
int  mem_auto_recall(const char *query,
                     char *out, int max_out);    // 검색 (최대 5개, S 정렬)
void mem_auto_reinforce(const char *topic);       // 강화 (S += delta)

/* ── Semantic Memory (Level 2) ── */
void mem_semantic_store(const char *key,
                        const char *value,
                        double strength,
                        const char *source);      // 사실 저장
const char* mem_semantic_get(const char *key);    // 정확 일치 검색
int  mem_semantic_search(const char *prefix,
                         char *out, int max_out); // 접두사 검색

/* ── Ebbinghaus 망각 곡선 ── */
void mem_auto_decay_all(void);                     // 매일 1회 호출 (S 감소)
int  mem_auto_prune(void);                         // 25KB 초과 시 정리
void mem_compact(void);                            // 7일 간격 컴팩션

/* ── 자동 추출 (LLM 응답에서) ── */
int  mem_auto_extract(const char *llm_response);   // 패턴 매칭 → 자동 저장

/* ── 시스템 프롬프트 주입 ── */
int  mem_auto_inject(char *sys_prompt, int max);   // context에 메모리 추가
```

---

## 7. 사용 흐름

```
session_start()
  → mem_auto_init("$ALCHEMY_DIR/memory/")
  → mem_auto_recall(current_query, buf, 1024)
  → 시스템 프롬프트에 메모리 주입

LLM 호출 중...
  → 응답 도착
  → mem_auto_extract(llm_response)  ← 자동 추출
  → mem_auto_reinforce(matched_topic)  ← 강화

session_end()
  → mem_auto_decay_all()
  → mem_auto_prune()

매일 첫 실행 (7일마다)
  → mem_compact()  ← 오래된 약한 메모리 정리
```

---

## 8. Stop Conditions

1. $ALCHEMY_DIR/memory/ 디스크 사용량 > 1MB → Episodic만 저장 (Semantic 중단)
2. 디렉토리 생성 실패 → silent fallback (메모리 없이 실행)
3. mmap 실패 (32KB 할당 불가) → strstr fallback (인덱스 없이 순차 검색)
4. 단일 .mem 파일 > 4KB → 4KB에서 자름 (마지막 문장 보존 시도)
5. 총 .mem 파일 수 > 128개 → 가장 오래된 32개 삭제

---

## 9. 참조 구현

- Claude Code: `.claude/memory/MEMORY.md` + 토픽별 파일
- Hermes: `memory_tool.py` + `session_search_tool.py` + FTS5
- MemoryBank: Zhong et al. 2023 — Ebbinghaus forgetting curve
- HeLa-Mem: Hebbian associative graph memory
