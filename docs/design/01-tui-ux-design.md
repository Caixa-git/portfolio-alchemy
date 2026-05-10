# TUI/UX Design — alchemy-c Breathing & Status System

> Phase A: alchemy_breath 강화 + 상태 표시줄 + 타이머
> 설계일: 2026-05-09

---

## 1. Design Philosophy

"Breathing should feel alive, not busy."

TUI는 코딩 도구의 보조 수단이지 주인공이 아니다.  
Claude Code의 shimmer dot + elapsed timer 패턴을 채택하되,  
alchemy의 `{◆}` 시그니처를 유지한다.

### Math + Beauty

| 원칙 | 적용 |
|------|------|
| **Sine-wave 호흡** | 현재 `f(t)=b_min+(b_max-b_min)*(sin(ωt)+1)/2` 유지 |
| **Shimmer** | Claude Code 스타일 cosine-wave 색상 스윕 — `◆`가 숨쉬면서 좌→우로 밝은 띠가 이동 |
| **Elapsed Timer** | Codex StatusIndicatorWidget 차용 — `(42s esc to interrupt)` |
| **Reduced Motion** | `NO_COLOR` / `ALCHEMY_NO_ANIM` 환경변수 지원 |

### Benchmark 채택/거절

| 출처 | 채택 | 거절 |
|------|------|------|
| **Claude Code** | shimmer dot + elapsed timer + side-by-side diff | 36-frame ASCII 스프라이트 (너무 무거움) |
| **Codex CLI** | pausable timer, status indicator widget 구조 | syntect 의존성 (C에 없음) |
| **OpenCode** | tool call inline 표시 | Bubble Tea (Go 전용) |
| **Crush** | — | scrambled 문자 애니메이션 (코딩 도구에 과함) |

---

## 2. Architecture

### 기존 breath 시스템 (현재)

```
┌─ {◆} Finding the optimal skill.. ─────────────────┐
│  sine-wave brightness (1500ms) + dots cycling      │
│  breath_lock() → 풀밝기 고정 → breath_stop()      │
└────────────────────────────────────────────────────┘
```

### 개선 breath 시스템 (Phase A)

```
┌─ {◆} Finding the optimal skill..  (42s) ──────────┐
│  sine-wave brightness + shimmer sweep + elapsed     │
│  [tool_call 표시: web_search("github")]             │
│  breath_lock() → 풀밝기 + 타이머 정지               │
└────────────────────────────────────────────────────┘
```

### 새 API 레이어

```c
// ── alchemy_breath.h (기존 유지 + 확장) ──

/* Shimmer 모드 */
#define BREATH_SHIMMER_NONE   0  // 기존 sine-wave only
#define BREATH_SHIMMER_DOT    1  // Claude Code 스타일 shimmer (◆ 자체가 밝기 변화)
#define BREATH_SHIMMER_SWEEP  2  // codex 스타일 shimmer (좌→우 밝은 띠)

/* 상태 표시줄 컨텍스트 */
typedef struct {
    const char *tool_name;      // 현재 실행 중인 도구 (NULL이면 표시 안 함)
    const char *tool_args;      // 도구 인자 (truncated)
    int         elapsed_ms;     // 경과 시간 (ms)
    int         paused;         // 타이머 일시 정지 (사용자 입력 대기 중)
} breath_ctx_t;

/* 새 API */
void breath_set_context(const breath_ctx_t *ctx);  // 도구 실행 정보 업데이트
void breath_set_elapsed(int ms);                     // 경과 시간 설정
void breath_set_shimmer(int mode);                   // shimmer 모드 변경
void breath_pause_timer(void);                       // 타이머 일시 정지 (사용자 입력 대기)
void breath_resume_timer(void);                      // 타이머 재개
```

### 출력 포맷

```c
// 한 줄 포맷:
// \r{◆} <status><dots>  (<elapsed>)└ <tool_name>(<args>)
//
// 예:
// \r{◆} Finding the optimal skill..  (42s)└ web_search("github")
// \r{◆} Generating response...  (1m 02s)
// \r{◆} Done! (locked)  (3s)

/* 밝기 계산 (기존 유지) */
// ◆: sine-wave 15% ↔ 100% (1500ms/cycle, 20fps)
// {와 }: 75% 고정
// 타이머: 항상 100% (변하지 않음)
// └ prefix 도구 정보: 항상 100%
```

---

## 3. 표준 출력 규칙

alchemy의 모든 표준 출력은 다음 규칙을 따른다:

### 출력 레이어

| 레이어 | ANSI 스타일 | 예 |
|--------|------------|-----|
| **◆ 호흡** | TrueColor sine-wave | `\033[38;2;R;G;Bm◆` |
| **상태 텍스트** | 밝기 #FFCC3D (255,204,61) 고정 | `Finding the optimal skill` |
| **경과 시간** | Bold+DIM (242) | `(42s)` |
| **도구 정보** | Italic+DIM (245) | `└ web_search("github")` |
| **에러** | Bold+Red (196) | `✖ Error: ...` |
| **성공** | Bold+Green (82) | `✔ Done` |
| **시스템** | Bold+Cyan (81) | `→ Response received.` |
| **사용자** | Bold+Yellow (227) | `> 사용자 입력` |

### multi-line 메시지와의 관계

```
{◆} Generating tests..  (1m 02s)└ write_file("test.c")
→ Tests generated: 3 passed, 0 failed    ← breath_lock() 이후 새 줄
✔ All tests passed.                      ← 시스템 메시지
> what's next?                            ← 사용자 입력
```

### non-TTY fallback

```c
// isatty(STDOUT_FILENO) == 0 → ANSI 코드 제거
// 출력: [{◆}] Finding the optimal skill..  (42s)
// 포맷: [emoji] status dots (elapsed) context
```

---

## 4. 파일 구조

```
src/core/
├── alchemy_breath.c    ← 기존 139줄 → ~200줄 (확장)
└── alchemy_breath.h    ← 기존 73줄 → ~100줄 (API 확장)
```

변경 최소화: 기존 API에 `breath_set_context()`, `breath_set_elapsed()`만 추가.  
기존 호출 코드 변경 불필요.

---

## 5. Stop Conditions

1. `isatty(STDOUT_FILENO) == 0` → ANSI fallback
2. `ALCHEMY_NO_ANIM` 환경변수 → shimmer 비활성화
3. `NO_COLOR` 환경변수 → 모든 ANSI 제거
4. timeout > 1시간 → hh:mm:ss 표기로 전환 (자동)
5. timer overflow 방지: max 99:59:59 (359,999초)

---

## 6. 참조 구현

- Claude Code: `ratatui` shimmer + `crossterm` timer (Rust)
- Codex CLI: `StatusIndicatorWidget` + `shimmer.rs` (Rust)
- alchemy_breath.c: 현재 `g_spinner` 전역 상태 + `select()` 기반 tick
