/*
 * alchemy_hook.h — Alchemy Hook System
 *
 * ═══════════════════════════════════════════════════════════════════════
 * 수학적 기반: Event-Driven Architecture (Gamma et al., 1994)
 * ═══════════════════════════════════════════════════════════════════════
 *
 * Hook 시스템은 Observer 패턴의 변형:
 *   Subject (Hook Manager) → Observers (Hook 함수들)
 *
 * Gamma의 원칙: "여러 객체가 특정 이벤트를 관찰할 필요가 있을 때"
 *   → alchemy에서는 Tool 실행 전후, 응답 완료 시에 사용.
 *
 * Claude Code의 8종 Hook에서 핵심 3종만 추출:
 *   PreToolUse  → 실행 전 검증/필터
 *   PostToolUse → 실행 후 처리 (통계, 로깅)
 *   Stop        → 응답 완료 시 처리
 *
 * ═══════════════════════════════════════════════════════════════════════
 * 아름다움의 원칙
 * ═══════════════════════════════════════════════════════════════════════
 *
 * 1. 단일 파일 — h/c pair. 60줄. 함수 포인터 체인.
 * 2. 정적 배열 — 동적 할당 없음. O(1) 디스패치.
 * 3. 체인 가능 — 여러 hook이 같은 이벤트에 등록 가능.
 * 4. 실패 격리 — 하나의 hook 실패가 체인을 중단하지 않음.
 * 5. 제로 의존성 — libc만 사용.
 *
 * ═══════════════════════════════════════════════════════════════════════
 */

#ifndef ALCHEMY_HOOK_H
#define ALCHEMY_HOOK_H

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================
 * Hook 이벤트 타입
 * ================================================================ */

typedef enum {
    HOOK_PRE_TOOL   = 0,  /* 도구 실행 전 (검증용, return 0=허용, -1=차단) */
    HOOK_POST_TOOL  = 1,  /* 도구 실행 후 (결과 처리용) */
    HOOK_STOP       = 2,  /* Agent 응답 완료 시 */
    HOOK_MAX_EVENTS = 3,
} hook_event_t;

/* ================================================================
 * Hook 함수 타입
 *
 * PreTool:   tool_name, args, result(읽기전용) → 0=허용, -1=차단
 * PostTool:  tool_name, args, result(수정가능) → void
 * Stop:      void → void
 * ================================================================ */

#define HOOK_NAME_LEN 32

typedef int  (*hook_pre_fn_t)(const char *tool, const char *args);
typedef void (*hook_post_fn_t)(const char *tool, const char *args,
                               char *result, int result_len);
typedef void (*hook_stop_fn_t)(void);

/* ================================================================
 * API
 * ================================================================ */

/* Hook 등록
 * event:  HOOK_PRE_TOOL / HOOK_POST_TOOL / HOOK_STOP
 * name:   hook 식별자 (디버깅용)
 * pre_fn: PreTool hook (event=HOOK_PRE_TOOL일 때만 사용)
 * post_fn: PostTool hook (event=HOOK_POST_TOOL일 때만 사용)
 * stop_fn: Stop hook (event=HOOK_STOP일 때만 사용)
 * returns: 0 = 성공, -1 = 슬롯 부족
 */
int hook_register(hook_event_t event, const char *name,
                  hook_pre_fn_t pre_fn,
                  hook_post_fn_t post_fn,
                  hook_stop_fn_t stop_fn);

/* PreTool Hook 실행 (등록된 모든 hook 호출, 하나라도 차단하면 중단)
 * returns: 0 = 전부 허용, -1 = 하나라도 차단
 */
int hook_fire_pre(const char *tool, const char *args);

/* PostTool Hook 실행 (등록된 모든 hook 호출) */
void hook_fire_post(const char *tool, const char *args,
                    char *result, int result_len);

/* Stop Hook 실행 */
void hook_fire_stop(void);

/* 등록된 hook 목록 조회 */
int hook_list(char *out, int max_out);

#ifdef __cplusplus
}
#endif

#endif /* ALCHEMY_HOOK_H */
