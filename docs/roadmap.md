# Alchemy 로드맵

> 참고: Claude Code / Codex / OpenCode / Hermes Agent 기능 분석 결과
> 최종 목표: **5개 플랫폼 중 어디에도 뒤지지 않는 C 기반 AI Agent 프레임워크**

---

## 현재 대비 플랫폼 격차

| 기능 영역 | Claude Code | Codex | OpenCode | Hermes | alchemy (현재) |
|:----------|:-----------:|:-----:|:--------:|:------:|:----------------:|
| Subagent/Multi-agent | ✅ agents/ | ❌ | ❌ | ✅ kanban | ❌ |
| Settings/Permissions | ✅ 3계층 | ❌ | ❌ | ✅ toolsets | ❌ |
| Hooks (Pre/Post) | ✅ 8종 | ❌ | ❌ | ❌ | ❌ |
| MCP 통합 | ✅ 내장 | ❌ | ⚠️ | ✅ native-mcp | ❌ |
| 자동 학습 메모리 | ✅ 자동 | ❌ | ❌ | ✅ session_search | ❌ |
| 슬래시 명령어 | ✅ 20+ | ❌ | ❌ | ❌ | ❌ |
| 세션 관리 | ✅ resume/continue | ❌ | ✅ session list | ✅ cron | ❌ |
| 비용 추적 | ✅ cost/token | ❌ | ✅ stats | ❌ | ❌ |
| 플러그인 | ✅ plugin system | ❌ | ❌ | ✅ skills | ❌ |
| 스트리밍 | ✅ stream-json | ❌ | ❌ | ❌ | ❌ |
| 컨텍스트 시각화 | ✅ /context | ❌ | ❌ | ❌ | ❌ |
| 멀티 프로바이더 | ✅ switch model | ❌ | ✅ provider-agnostic | ✅ multiple | ⚠️ 1개 |
| 워크트리/병렬 | ✅ --worktree | ✅ git worktree | ✅ parallel | ❌ | ❌ |
| 에러 복구 | ✅ fallback model | ❌ | ❌ | ⚠️ | ❌ |
| HTML 출력 | ❌ | ❌ | ❌ | ❌ | ✅ NEW |

---

## Phase 1: Core 완성 (즉시, 1-2주)

> 목표: 안정적인 싱글 에이전트 기반 완성

### 1.1 Settings/Permissions 시스템
```
파일: .alchemy/settings.json (ALCHEMY.md 참조)
```

```json
{
  "permissions": {
    "allow": ["terminal(make *)", "read_file"],
    "ask": ["write_file(*.c)", "terminal(git push *)"],
    "deny": ["terminal(rm -rf *)", "terminal(shutdown *)"]
  }
}
```

**구현:** C 구조체 + JSON 파서 (alchemy_http.c의 JSON 파싱 재사용)

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Settings | ✅ .claude/settings.json | ❌ | ❌ | ✅ config.yaml | ⏳ Phase 1 |

### 1.2 Hooks 시스템 (Pre/Post Tool)
```
알케미-c hooks:
  - PreToolUse:   도구 실행 전 검증/필터
  - PostToolUse:  도구 실행 후 처리 (로깅, 통계)
  - Stop:         응답 완료 시 처리
```

**구현:** 함수 포인터 체인 (alchemy_tool.c에 통합)
```c
typedef int (*hook_fn_t)(const char *tool_name, const char *args, char *result);
int hook_register(int event_type, hook_fn_t fn);
```

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Hooks | ✅ 8종 | ❌ | ❌ | ❌ | ⏳ Phase 1 |

### 1.3 에러 복구 (Retry + Fallback)

```
LLM 호출 실패 시:
  1. 429 (Rate Limit) → 5초 대기 후 재시도 (최대 3회)
  2. 503 / timeout → fallback 모델로 전환
  3. JSON parse fail → 응답 버림, "재시도" 메시지
  4. Tool 실행 실패 → graceful message (crash 금지)
```

**구현:** `alchemy_llm.c`에 retry_with_backoff() 추가

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Retry | ✅ fallback | ❌ | ❌ | ⚠️ | ⏳ Phase 1 |

### 1.4 세션 관리

```c
/* 세션 저장/복원 */
int session_save(const char *id);       // context + memory 저장
int session_load(const char *id);       // 이전 세션 복원
int session_list(char *out, int max);   // 세션 목록
```

**구현:** `alchemy_memory.c`에 세션 ID 기반 스냅샷 저장.

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Sessions | ✅ /resume | ❌ | ✅ session list | ✅ cron | ⏳ Phase 1 |

---

## Phase 2: Agent 성능 (2-4주)

> 목표: 개발 생산성을 극대화하는 도구/기능

### 2.1 멀티 프로바이더 + 자동 라우터

현재는 단일 엔드포인트만 지원. OpenCode처럼 provider-agnostic하게:

```c
/* Provider 추상화 */
typedef struct {
    char name[32];
    char endpoint[256];
    char api_key_env[64];     // 환경변수명
    int  priority;             // 0=1순위, 1=2순위...
    int  weight;               // 호출 비중 (라운드로빈용)
} provider_t;
```

**자동 fallback 체인:** DeepSeek → Claude → GPT 순서로 장애 시 전환.

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Multi-provider | ✅ switch | ❌ | ✅ agnostic | ✅ multiple | ⏳ Phase 2 |

### 2.2 컨텍스트 시각화 (Context Dashboard)

Claude Code의 `/context` 명령어 대응:

```c
/* 컨텍스트 사용률 시각화 */
void context_visualize(char *out, int max);
/* 출력 예:
[████████░░░░░░░░░░░░] 41% used (8.2KB / 20KB)
  system:   2.1KB  ████████░░░░
  messages: 4.3KB  ██████████████████░░
  tools:    1.8KB  ███████░░░░░
  tokens:   2,341
*/
```

**출력 포맷:** HTML progress bar + ascii art fallback (터미널용)

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Context viz | ✅ /context | ❌ | ❌ | ❌ | ⏳ Phase 2 |

### 2.3 스트리밍 LLM 응답

현재: 전체 응답 대기 후 처리.
목표: SSE(Server-Sent Events) 스트리밍 → 점진적 UI 업데이트.

```c
/* Streaming 콜백 */
typedef void (*stream_cb_t)(const char *chunk, int is_done);
int llm_post_stream(const char *endpoint, const char *api_key,
                    const char *body, int body_len,
                    stream_cb_t callback);
```

**활용:** Discord에서 메시지 타이합 인디케이터 + 점진적 출력.

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Streaming | ✅ stream-json | ❌ | ⚠️ | ❌ | ⏳ Phase 2 |

### 2.4 자동 메모리 학습

Hermes의 `session_search()` + Claude Code의 auto-memory:

```c
/* 대화에서 자동으로 사실 추출 */
/* LLM 응답에서 "사용자는 X를 선호함" 같은 패턴 감지 → 자동 저장 */
int memory_auto_extract(const char *llm_response);
```

**저장:** `alchemy_memory.c`의 KV 저장소에 자동 추가.

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Auto memory | ✅ 자동 | ❌ | ❌ | ✅ session_search | ⏳ Phase 2 |

---

## Phase 3: 고급 기능 (4-8주)

> 목표: Claude Code 수준의 확장성과 생태계

### 3.1 Subagent / Multi-agent 시스템

Claude Code의 `.claude/agents/*.md` 대응:

```c
/* Subagent 정의 */
typedef struct {
    char name[32];
    char description[256];
    char *system_prompt;       /* 전용 persona */
    tool_fn_t *tools;          /* 허용된 도구 목록 */
    int         tool_count;
} subagent_t;

/* Subagent에 작업 위임 */
int subagent_delegate(subagent_t *agent, const char *task,
                      char *result, int max_result);
```

**실행:** fork() + pipe()로 격리된 컨텍스트에서 실행.

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Subagents | ✅ agents/ | ❌ | ❌ | ✅ kanban | ⏳ Phase 3 |

### 3.2 MCP 서버 통합

MCP (Model Context Protocol) 서버 연결:

```c
typedef struct {
    char name[32];
    char command[256];    /* 실행 명령어 (stdio transport) */
    char *tools_json;     /* MCP 서버가 제공하는 도구 목록 */
} mcp_server_t;

int mcp_connect(mcp_server_t *server);
int mcp_call_tool(mcp_server_t *server, const char *tool,
                  const char *args, char *out, int max_out);
```

**활용:** 외부 MCP 서버(filesystem, github, DB)를 alchemy 도구로 등록.

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| MCP | ✅ 내장 | ❌ | ⚠️ | ✅ native-mcp | ⏳ Phase 3 |

### 3.3 플러그인 시스템

런타임에 `.so` 동적 라이브러리 로드:

```c
typedef struct plugin {
    char name[64];
    void *handle;           /* dlopen handle */
    int (*init)(void);      /* 플러그인 초기화 */
    int (*register_tools)(void);  /* 도구 등록 */
    int (*register_hooks)(void);  /* 훅 등록 */
} plugin_t;

int plugin_load(const char *path);
int plugin_unload(const char *name);
```

**의존성:** `dlopen()` / `dlsym()` (POSIX) — Windows용 `LoadLibrary()` fallback.

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Plugins | ✅ .claude/plugins | ❌ | ❌ | ✅ skills (built-in) | ⏳ Phase 3 |

### 3.4 워크트리 / 병렬 실행

독립적인 git worktree에서 병렬 작업:

```c
int worktree_create(const char *branch, char *path, int max_path);
int worktree_exec(const char *path, const char *cmd, char *out, int max_out);
int worktree_remove(const char *path);
```

**패턴:** Claude Code의 `--worktree` + Codex의 git worktree 방식 차용.

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Worktree | ✅ --worktree | ✅ git worktree | ✅ parallel dir | ❌ | ⏳ Phase 3 |

### 3.5 슬래시 명령어 시스템

```c
typedef struct {
    char name[32];           /* "review", "compact", "status" */
    char description[128];
    int (*handler)(const char *args, char *out, int max_out);
} slash_command_t;

int slash_register(const slash_command_t *cmd);
int slash_execute(const char *input, char *out, int max_out);
```

**내장 명령어 후보:**

| 명령어 | 기능 | 출처 |
|:-------|:-----|:-----|
| `/status` | Agent 상태 + 컨텍스트 사용률 | Claude Code |
| `/compact` | 컨텍스트 압축 (오래된 메시지 정리) | Claude Code |
| `/review` | 현재 변경사항 코드 리뷰 | Claude Code |
| `/memory` | 메모리 편집 | Claude Code |
| `/model` | 모델 전환 | Claude Code |
| `/stats` | 사용 통계 + 비용 | OpenCode |
| `/dashboard` | HTML 대시보드 생성 (브라우저 열기) | Alchemy |
| `/session` | 세션 목록/전환 | Claude Code |

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Commands | ✅ 20+ slash | ❌ | ❌ | ❌ | ⏳ Phase 3 |

---

## Phase 4: 생태계 (8-12주)

> 목표: 독립적인 C 언어 Agent 생태계 구축

### 4.1 비용 추적 (Token Usage)

```c
typedef struct {
    int  total_input_tokens;
    int  total_output_tokens;
    int  total_tool_calls;
    int  session_count;
    double estimated_cost_usd;  /* 모델별 단가 적용 */
} cost_stats_t;

void cost_record(int input_tokens, int output_tokens, const char *model);
void cost_report(char *out, int max);
```

| 비교 | Claude Code | Codex | OpenCode | Hermes | alchemy |
|:-----|:-----------:|:-----:|:--------:|:------:|:---------:|
| Cost | ✅ /cost | ❌ | ✅ stats | ❌ | ⏳ Phase 4 |

### 4.2 CLI 개선 (TUI)

현재: 단순 stdin/stdout.
목표: Claude Code 스타일 TUI.

```c
/* 최소한의 TUI: 상태 표시줄 + 메시지 영역 */
void tui_init(void);
void tui_status(const char *fmt, ...);
void tui_message(const char *role, const char *content);
void tui_progress(int pct);
```

**의존성:** ncurses (opt-in) / ANSI escape (fallback)

### 4.3 테스트 인프라

```c
/* Mock LLM 서버 — API 없이 테스트 가능 */
int test_llm_start_mock(const char *fixture_response);
void test_llm_stop_mock(void);

/* Mock 파일 시스템 */
int test_fs_mount(const char *virtual_path, const char *content);
void test_fs_unmount(const char *virtual_path);
```

### 4.4 패키지/배포

```c
/* alchemy 설치 스크립트 */
alchemy install        # 바이너리 + 기본 설정
alchemy init           # 프로젝트 초기화 (ALCHEMY.md 생성)
alchemy doctor         # 시스템 진단 (의존성 체크)
```

---

## 요약 로드맵

```
Phase 1 (1-2주) │ Core 완성
├── Settings/Permissions    ✅ library/ds/ + alchemy_permit
├── Hooks (Pre/Post Tool)  ✅ alchemy_hook (함수 포인터 체인)
├── Error Recovery          ✅ library/algo/alchemy_retry
└── Session Management      ✅ library/ds/alchemy_session

Phase 2 (2-4주) │ Agent 성능
├── Multi-Provider Router   ✅ library/llm/alchemy_llm_router
├── Context Visualization   ✅ alchemy_dashboard + context_visualize
├── LLM Streaming           ✅ library/llm/alchemy_llm_router (SSE)
└── Auto Memory Learning    ⏳ (Phase 2.5)

Phase 3 (4-8주) │ 고급 기능
├── Subagent System         ✅ library/algo/alchemy_subagent
├── MCP Integration         ⏳ (library/llm/ 확장)
├── Plugin System           ⏳ (library/algo/ → dlopen)
├── Worktree/Parallel       ⏳
└── Slash Commands          ✅ library/algo/alchemy_slash

Phase 4 (8-12주) │ 생태계
├── Cost Tracking           ✅ library/algo/alchemy_cost
├── TUI (Terminal UI)       ✅ library/algo/alchemy_tui
├── Test Infrastructure     ✅ library/algo/alchemy_test
└── Package/Distribution    ⏳ (binary packaging)
```

## 우선순위 매트릭스

| 기능 | 영향도 | 구현 난이도 | Phase | 이유 |
|:-----|:------:|:----------:|:-----:|:------|
| Settings/Permissions | 🔥 높음 | 🟢 쉬움 (1) | P1 | 보안 기본기 |
| Hooks 시스템 | 🔥 높음 | 🟢 쉬움 (1) | P1 | 확장성 기본기 |
| 에러 복구 | 🔥 높음 | 🟢 쉬움 (1) | P1 | 안정성 |
| 세션 관리 | ⚡ 중간 | 🟡 보통 (2) | P1 | 연속성 |
| 멀티 프로바이더 | ⚡ 중간 | 🟡 보통 (2) | P2 | 비용 최적화 |
| 컨텍스트 시각화 | 🔥 높음 | 🟢 쉬움 (1) | P2 | 사용자 경험 |
| 스트리밍 | ⚡ 중간 | 🔴 어려움 (3) | P2 | UX 개선 |
| 자동 메모리 | ⚡ 중간 | 🟡 보통 (2) | P2 | 학습 능력 |
| Subagent | 🔥 높음 | 🔴 어려움 (3) | P3 | 병렬화 |
| MCP 통합 | ⚡ 중간 | 🟡 보통 (2) | P3 | 생태계 |
| 슬래시 명령어 | ⚡ 중간 | 🟢 쉬움 (1) | P3 | 사용성 |
| 플러그인 | 낮음 | 🔴 어려움 (3) | P3 | 확장성 |
| 비용 추적 | 낮음 | 🟢 쉬움 (1) | P4 | 관리 |
| TUI | ⚡ 중간 | 🔴 어려움 (3) | P4 | 경험 |
| 테스트 인프라 | ⚡ 중간 | 🟡 보통 (2) | P4 | 품질 |
```
