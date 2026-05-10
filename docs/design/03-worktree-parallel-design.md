# Worktree & Parallel Execution — alchemy-c Multi-Agent System

> Phase C: git worktree 관리 + subagent 확장 + 작업 큐
> 설계일: 2026-05-09

---

## 1. Design Philosophy

"Three isolation levels for three scales of work."

| 격리 수준 | 방법 | 사용처 | 참조 |
|-----------|------|--------|------|
| **Level 1: In-process** | Thread-safe dispatch (Codex parallel.rs 패턴) | 단일 세션 내 동시 도구 호출 | Codex CLI RwLock |
| **Level 2: Process** | fork() + pipe() subagent | 격리된 작업 실행, LLM 호출 | Hermes delegate_task |
| **Level 3: Filesystem** | git worktree | 파일 변경이 있는 병렬 작업 | Claude Code --worktree |

### Math + Beauty

| 원칙 | 적용 |
|------|------|
| **fork+pipe 프로세스 격리** | POSIX 표준. 완전한 메모리 격리. 독립적 crash domain |
| **git worktree 파일 격리** | Git 객체 공유 + 워킹 디렉토리 분리. Claude Code가 검증 |
| **CAS 작업 큐** | Compare-And-Swap로 작업 상태 관리. Hermes Kanban의 SQLite WAL 패턴 차용 |
| **Circuit Breaker** | N회 연속 실패 시 자동 차단. Hermes failure_limit 차용 |
| **최소 권한** | subagent는 허용된 tool만 실행. Codex parallel.rs의 read/write lock 개념 |

### Benchmark 채택/거절

| 출처 | 채택 | 거절 |
|------|------|------|
| **Claude Code --worktree** | git worktree 생성/관리/자동정리, `.worktreeinclude` 패턴 | auto-merge (위험, 충돌 가능) |
| **Codex parallel.rs** | Read/Write lock 기반 동시성 제어 | tokio 의존성 (C에 없음) |
| **Hermes Kanban** | CAS 작업 큐 + circuit breaker + heartbeat | Python ThreadPoolExecutor (GIL 제한) |
| **Hermes delegate_task** | toolset 제한 + 타임아웃 + 격리 | Context inheritance (C에선 pipe로 충분) |
| **OpenCode agent** | read-only subagent | stateless model (연속 대화 불가) |
| **AgentSPEX (논문)** | typed step 정의 + checkpoint | XML DSL (과함) |

---

## 2. Architecture

### 세 가지 실행 모드

```
┌─────────────────────────────────────────────────────┐
│  alchemy_parallel.h  — 통합 API                      │
│                                                      │
│  fork+pipe subagent  ← Level 2 (process isolation)  │
│  git worktree        ← Level 3 (filesystem)         │
│  작업 큐              ← scheduling                   │
└─────────────────────────────────────────────────────┘
```

### Level 1: In-process Dispatch (선택 적용)

```c
// ── 알케미 도구 수준의 병렬 실행 ──
// Codex CLI parallel.rs 패턴: read lock / write lock
//
// read lock: web_search, read_file, calculator 등 I/O 바운드
// write lock: write_file, patch, terminal(git push) 등 상태 변경
//
// C99 select() 기반 non-blocking I/O로 구현
// 실제 LLM tool_call의 parallel=true/false 속성에 따라 결정

typedef enum {
    PARALLEL_SAFE,   // read lock — 동시 실행 가능
    PARALLEL_EXCLUSIVE  // write lock — 단독 실행
} parallel_level_t;
```

### Level 2: Process (fork+pipe subagent) — 기존 확장

```c
// ── alchemy_subagent.h 기존 API 유지 + 확장 ──

// 기존 (변경 없음):
//   subagent_init, subagent_add_tool
//   subagent_spawn, subagent_send, subagent_recv
//   subagent_kill, subagent_alive, subagent_exec

// 새 API:
typedef struct {
    subagent_t   base;
    int          exit_code;    // waitpid() 결과
    double       cpu_time;     // clock_gettime(CLOCK_PROCESS_CPUTIME_ID)
    int          heartbeat;    // 마지막 heartbeat (sec)
    int          timed_out;    // 1 = timeout
} subagent_instance_t;

// 확장 spawn:
int subagent_spawn_ext(subagent_instance_t *inst,
                       subagent_def_t *def,
                       int timeout_sec);

// 작업 이력 관리:
int subagent_result_save(const subagent_instance_t *inst,
                         const char *task_id);
int subagent_result_load(subagent_instance_t *inst,
                         const char *task_id);
```

### Level 3: Git Worktree (신규)

```c
// ── alchemy_worktree.h (신규) ──
//
// Claude Code --worktree 패턴:
//   1. git worktree add .alchemy/worktrees/<name> <branch>
//   2. cd worktree → 작업 실행
//   3. git commit (자동)
//   4. exit → 변경 있으면 keep, 없으면 remove

#define WORKTREE_NAME_MAX   64
#define WORKTREE_DIR_MAX    256
#define WORKTREE_BRANCH_MAX 128

typedef struct {
    char name[WORKTREE_NAME_MAX];     // "feature-auth"
    char path[WORKTREE_DIR_MAX];      // .alchemy/worktrees/feature-auth/
    char branch[WORKTREE_BRANCH_MAX]; // worktree-feature-auth
    int  has_changes;                 // 1 = uncommitted changes
    int  created_at;                  // unix ts
} worktree_t;

/* 작업트리 생성 (git worktree add) */
int  worktree_create(worktree_t *wt, const char *name,
                     const char *base_branch);
/* 작업트리 상태 확인 */
int  worktree_status(worktree_t *wt);
/* 작업트리에서 명령 실행 */
int  worktree_exec(const worktree_t *wt, const char *cmd,
                   char *out, int max_out);
/* 변경사항 커밋 */
int  worktree_commit(const worktree_t *wt, const char *message);
/* PR 생성 (gh CLI) */
int  worktree_pr(const worktree_t *wt, const char *title);
/* 작업트리 제거 */
int  worktree_remove(const worktree_t *wt);
/* 모든 작업트리 목록 */
int  worktree_list(char *out, int max_out);
/* 오래된 작업트리 정리 (cleanupPeriodDays) */
int  worktree_cleanup(int max_age_days);
```

### 작업 큐 (Job Queue)

```c
// ── alchemy_task_queue.h (신규) ──
//
// Hermes Kanban의 SQLite + CAS 패턴을 C99로 단순화.
// 파일 기반 작업 큐 (JSON lines format).

#define TASK_ID_MAX   64
#define TASK_STATE_MAX 16  /* "pending" / "running" / "done" / "failed" */

typedef struct {
    char   id[TASK_ID_MAX];
    char   state[TASK_STATE_MAX];
    char   subagent_name[SUBAGENT_NAME_MAX];
    char   task_body[4096];       // 작업 설명
    int    created_at;
    int    started_at;
    int    completed_at;
    int    exit_code;
    int    failure_count;         // circuit breaker
    int    max_retries;           // default 3
} task_entry_t;

/* 작업 큐 초기화 */
int  task_queue_init(const char *queue_path);
/* 작업 추가 */
int  task_enqueue(const char *name, const char *body);
/* 작업 상태 조회 */
int  task_status(const char *id, char *state, int max_state);
/* 다음 작업 가져오기 (CAS) */
int  task_claim(char *id_out, int max_id);  // pending → running (CAS)
/* 작업 완료 */
int  task_complete(const char *id, int exit_code);  // running → done
/* 작업 실패 */
int  task_fail(const char *id, int exit_code);  // running → failed (retry or circuit break)
/* 회로 차단 확인 */
int  task_is_blocked(const char *name);  // failure_count >= max_retries
/* 모든 작업 목록 */
int  task_list(char *out, int max_out);

/* ── heartbeat ── */
int  task_heartbeat(const char *id);  // running → touched
int  task_reclaim_stale(int timeout_sec);  // No heartbeat > timeout → pending

/* ── Circuit Breaker ── */
// failure_count >= max_retries → "blocked" 상태로 전환
// block 해제: task_reset_blocked() 또는 관리자 수동 해제
int  task_is_blocked(const char *agent_name);
void task_reset_blocked(void);
```

---

## 3. 작업 실행 흐름

```
사용자: "alchemy --worktree feature-auth"

1. task_enqueue("feature-auth", "Add authentication module")
   → queue/feature-auth.task 생성

2. main loop: task_claim()
   → "feature-auth" → "running" 상태로 CAS 전환

3. 격리 수준 결정:
   if --worktree 플래그:
     worktree_create(&wt, "feature-auth", "main")
     worktree_exec(&wt, "cd worktree && alchemy run 'Add auth'")
     worktree_commit(&wt, "feat: add authentication")
     worktree_pr(&wt, "Add authentication module")
     task_complete("feature-auth", 0)
   else:
     subagent_exec(subagent_def, "Add authentication",
                   result_buf, sizeof(result_buf))
     task_complete("feature-auth", 0)

4. heartbeats:
   task_heartbeat("feature-auth")  // 매 30초

5. 오류 처리:
   task_fail("feature-auth", 1) → retry or circuit break

6. 정리:
   worktree_cleanup(7)  // 7일 이상 된 작업트리 정리
   task_reclaim_stale(300)  // 5분 이상 heartbeat 없는 작업 회수
```

---

## 4. 세 가지 실행 모드 비교

| 항목 | Level 1 (In-process) | Level 2 (Process) | Level 3 (Worktree) |
|------|---------------------|-------------------|-------------------|
| **격리** | RwLock | fork+pipe | git worktree |
| **파일 영향** | 있음 | 제한적 (toolset) | 완전 격리 |
| **오버헤드** | 없음 | ~2ms (fork) | ~100ms (git) |
| **Crash 영향** | 부모도 crash | 자식만 죽음 | 독립적 |
| **언제 씀** | 동시 read_file/web_search | 격리된 LLM 작업 | 파일 수정 병렬 작업 |
| **참조** | Codex parallel.rs | Hermes delegate_task | Claude Code --worktree |

---

## 5. 파일 구조

```
library/algo/
├── alchemy_subagent.c    ← 기존 192줄 → ~250줄 (확장)
├── alchemy_subagent.h    ← 기존 96줄 → ~120줄
├── alchemy_worktree.c    ← 신규 ~200줄
├── alchemy_worktree.h    ← 신규 ~80줄
├── alchemy_task_queue.c  ← 신규 ~250줄
└── alchemy_task_queue.h  ← 신규 ~80줄
```

### Makefile 추가

```makefile
$(OBJ_DIR)/alchemy_worktree.o: library/algo/alchemy_worktree.c library/algo/alchemy_worktree.h | $(OBJ_DIR)
\t$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_task_queue.o: library/algo/alchemy_task_queue.c library/algo/alchemy_task_queue.h | $(OBJ_DIR)
\t$(CC) $(CFLAGS) -c -o $@ $<
```

---

## 6. API 사용 예

```c
// ── Worktree 사용 (직접 git 조작) ──
worktree_t wt;
worktree_create(&wt, "feature-auth", "main");
// .alchemy/worktrees/feature-auth/ 에서 작업
worktree_exec(&wt, "alchemy run 'Add user authentication'",
              result_buf, sizeof(result_buf));
worktree_commit(&wt, "feat: add user authentication");
worktree_pr(&wt, "Add user authentication module");
worktree_remove(&wt);  // 또는 worktree_cleanup(7)으로 자동 정리

// ── 작업 큐 사용 ──
task_queue_init(".alchemy/queue/");
task_enqueue("feature-auth", "Add authentication module");

char task_id[64];
task_claim(task_id, sizeof(task_id));
// task_id = "feature-auth"
// → subagent_spawn or worktree_create 실행
// → task_heartbeat(task_id) 매 30초
// → task_complete(task_id, 0) or task_fail(task_id, 1)
```

---

## 7. 안전장치

| 상황 | 대응 |
|------|------|
| **fork 실패** | graceful fallback: 현재 프로세스에서 직렬 실행 |
| **git worktree 충돌** | `worktree-<name>-<timestamp>` 로 자동 rename |
| **작업자 crash** | task_reclaim_stale() → 재시도 (최대 3회) |
| **회로 차단 (3회 실패)** | task_is_blocked() → 사용자에게 보고 |
| **하트비트 누락 (300초)** | task_reclaim_stale(300) → pending으로 복귀 |
| **디스크 부족** | worktree_cleanup(0) → 즉시 모든 작업트리 정리 |
| **타임아웃** | subagent_kill() → SIGTERM → SIGKILL |
| **PID 재사용** | 생성 시간(st_mtime) + waitid() WNOWAIT로 검증 |

---

## 8. Stop Conditions

1. `git --version` 실패 → worktree 불가능 (Level 2만 사용)
2. `.alchemy/worktrees/` 디스크 부족 → 에러 보고
3. 동시 작업 큐 최대 4개 (fork 폭주 방지)
4. 단일 worktree 최대 1GB (미달성 시 무시)
5. `ALCHEMY_NO_PARALLEL` 환경변수 → 모든 작업 직렬 실행
6. 헤드리스 모드 (isatty == 0) → worktree PR 기능 비활성화

---

## 9. 참조 구현

- Claude Code: `--worktree` flag, `.claude/worktrees/`, `.worktreeinclude`
- Codex CLI: `parallel.rs`, `ToolCallRuntime`, `RwLock`
- Hermes Agent: `kanban_db.py`, `kanban_tools.py`, `delegate_task.py`
- OpenCode: `agent-tool.go`, `Agent` interface
- AgentSPEX: arXiv 2605.02396 — typed steps, checkpoint, parallel execution
