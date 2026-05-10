/*
 * alchemy_permit.h — Alchemy Permission System
 *
 * ═══════════════════════════════════════════════════════════════════════
 * 수학적 기반: Capability-based Security (Dennis & Van Horn, 1966)
 * ═══════════════════════════════════════════════════════════════════════
 *
 * 현대 보안 모델 중 C 언어 Agent에 가장 적합한 것은
 * Capability 모델입니다 (Levy, "Capability-Based Computer Systems", 1984).
 *
 * Bell-LaPadula (BLP, 1973)  → 기밀성. Agent에는 과함.
 * Biba (1977)                → 무결성. Agent에는 과함.
 * RBAC (Ferraiolo & Kuhn, 1992) → 역할 기반. Agent에 적합.
 * Capability (Dennis 1966)   → 소유권 기반. 가장 단순하고 강력함.
 *
 * 선택: RBAC + Capability 혼합.
 *   Agent의 Tool 실행을 Role(역할) + Capability(권한 토큰)으로 제어.
 *
 *   Role:    agent가 수행하는 작업 유형 (developer, reviewer, planner)
 *   Capability: 개별 도구 실행 권한 (terminal:read, write:*.c)
 *
 * 참고: OWASP LLM Top 10 2025 — LLM06 Excessive Agency
 *   "LLM 기반 Agent가 과도한 권한으로 시스템에 접근하는 문제"
 *   → alchemy는 기본 Deny, 명시적 Allow만 허용
 *
 * ═══════════════════════════════════════════════════════════════════════
 * 아름다움의 원칙
 * ═══════════════════════════════════════════════════════════════════════
 *
 * 1. 단일 책임 — permit_check()는 오직 권한 확인만 한다.
 * 2. 최소 표면 — 헤더에 3개 함수만 노출.
 * 3. 불변성 — 한 번 로드된 규칙은 실행 중 변경되지 않는다.
 * 4. 명시적 실패 — 권한 없음 = -1, 이유를 errno에 기록.
 * 5. 자기 문서화 — 규칙 포맷이 사람이 읽는 설정 파일과 동일.
 *
 * ═══════════════════════════════════════════════════════════════════════
 */

#ifndef ALCHEMY_PERMIT_H
#define ALCHEMY_PERMIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================
 * 상수
 * ================================================================ */

#define PERMIT_MAX_RULES    64
#define PERMIT_PATTERN_LEN  128
#define PERMIT_PATH_LEN     512

/* 권한 결과 */
typedef enum {
    PERMIT_DENY  = 0,   /* 거부 (no) */
    PERMIT_ASK   = 1,   /* 사용자 확인 필요 */
    PERMIT_ALLOW = 2,   /* 허용 (yes) */
} permit_result_t;

/* ================================================================
 * 규칙 정의
 * ================================================================

   rules.json 예시:
   {
     "role": "developer",
     "rules": [
       {"tool": "terminal", "pattern": "make *",     "effect": "allow"},
       {"tool": "terminal", "pattern": "git push *",  "effect": "ask"},
       {"tool": "terminal", "pattern": "rm -rf *",    "effect": "deny"},
       {"tool": "terminal", "pattern": "shutdown *",  "effect": "deny"},
       {"tool": "read_file", "pattern": "*",           "effect": "allow"},
       {"tool": "write_file", "pattern": "*.c",       "effect": "allow"},
       {"tool": "write_file", "pattern": "*.h",       "effect": "allow"},
       {"tool": "write_file", "pattern": ".env",      "effect": "deny"}
     ]
   }

   패턴 매칭: glob 스타일 (wildcard = *, ?)
   우선순위: 구체적인 패턴이 먼저 매칭됨 (deny > ask > allow)
   */

typedef struct {
    char tool[32];               /* 도구 이름 (terminal, write_file...) */
    char pattern[PERMIT_PATTERN_LEN]; /* glob 패턴 (make *.c) */
    permit_result_t effect;      /* allow / ask / deny */
} permit_rule_t;

/* ================================================================
 * API
 * ================================================================ */

/* settings.json에서 규칙 로드
 * path: 설정 파일 경로 (NULL = .alchemy/settings.json)
 * role: 적용할 역할 (NULL = "developer")
 * returns: 로드된 규칙 수, -1 = 오류
 */
int permit_load(const char *path, const char *role);

/* 도구 실행 권한 확인
 * tool:  도구 이름 ("terminal", "write_file" ...)
 * input: 도구에 전달될 인자 ("make clean", "src/main.c" ...)
 * returns: PERMIT_DENY / PERMIT_ASK / PERMIT_ALLOW
 *
 * 수학적 원칙 (Capability 모델):
 *   - 기본값 = DENY (최소 권한 원칙, Saltzer & Schroeder 1975)
 *   - 명시적 ALLOW만 허용
 *   - ASK는 사용자에게 질문 (터미널 모드에서만)
 */
permit_result_t permit_check(const char *tool, const char *input);

/* 디버그용: 현재 규칙을 사람이 읽을 수 있는 문자열로 출력 */
int permit_dump(char *out, int max_out);

/* settings.json 자동 생성 (기본 규칙 포함) */
int permit_auto_create(void);

/* 규칙 추가 후 settings.json 저장 */
int permit_add_rule(const char *tool, const char *pattern, const char *effect);

/* ASK 프롬프트: [y/N/a] 반환 (0=거부, 1=한번허용, 2=항상허용) */
int permit_ask_tui(const char *tool, const char *input);

/* 설정 리로드 (변경 후 재로드) */
int permit_reload(void);

#ifdef __cplusplus
}
#endif

#endif /* ALCHEMY_PERMIT_H */
