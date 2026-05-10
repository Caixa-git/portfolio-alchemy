/*
 * alchemy_context.h
 * Alchemy Context Sync — 모델 전환 시 메시지 이력 보존
 *
 * ring buffer 형태로 최근 N개 메시지 저장
 * → LLM 호출 시 messages JSON 배열로 변환
 * → 모델 전환과 무관하게 항상 같은 이력 전달
 *
 * 확장: tool_call_id 지원 (tool 호출 결과 저장)
 */

#ifndef ALCHEMY_CONTEXT_H
#define ALCHEMY_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#define CONTEXT_MAX_MSGS    16   /* 최대 보존 메시지 수 */
#define CONTEXT_ROLE_LEN    16
#define CONTEXT_CONTENT_LEN 1024
#define CONTEXT_TOOL_ID_LEN 48

/* 단일 메시지 엔트리 */
typedef struct {
    char role[CONTEXT_ROLE_LEN];           /* "user", "assistant", "tool" */
    char content[CONTEXT_CONTENT_LEN];     /* content (tool calls JSON or text) */
    char tool_call_id[CONTEXT_TOOL_ID_LEN]; /* tool_call_id (tool role 전용) */
    unsigned char flags;                   /* bit 0: has_tool_calls (assistant가 tool 요청) */
} context_entry_t;

/* 메시지 이력 (ring buffer) */
typedef struct {
    context_entry_t entries[CONTEXT_MAX_MSGS];
    int count;    /* 현재 저장된 메시지 수 */
    int head;     /* ring buffer head (가장 오래된 메시지) */
} alchemy_context_t;

/* 초기화 */
void context_init(alchemy_context_t *ctx);

/* 메시지 추가
 * role: "user" 또는 "assistant"
 * content: 메시지 내용 (최대 CONTEXT_CONTENT_LEN-1)
 * CONTEXT_MAX_MSGS 초과 시 가장 오래된 메시지 제거
 */
void context_add(alchemy_context_t *ctx, const char *role, const char *content);

/* Assistant tool_calls 메시지 추가
 * content: 빈 문자열 (content = null)
 * tool_calls_json: tool_calls 배열 JSON 문자열
 */
void context_add_tool_call(alchemy_context_t *ctx, const char *tool_calls_json);

/* Tool 실행 결과 추가
 * tool_call_id: 원본 tool call의 id
 * content: 실행 결과 텍스트
 */
void context_add_tool_result(alchemy_context_t *ctx,
                             const char *tool_call_id, const char *content);

/* 저장된 메시지 수 반환 */
int context_count(const alchemy_context_t *ctx);

/* i번째 메시지 접근 (0 = 가장 오래된, count-1 = 최신) */
const context_entry_t *context_get(const alchemy_context_t *ctx, int i);

/* messages JSON 배열 생성
 * role=tool인 경우 tool_call_id 포함
 * flags & 1인 assistant는 tool_calls 포함
 */
int context_build_messages(const alchemy_context_t *ctx, char *out, int max_out);

/* LLM 요청용 전체 body 생성 (model + system + messages + tools + max_tokens)
 * system_prompt: 시스템 프롬프트 (NULL = 없음)
 * tools_spec: tool_build_spec() 결과 (NULL = tools 제외)
 */
int context_build_body_full(const alchemy_context_t *ctx, const char *system_prompt,
                            const char *model, const char *tools_spec,
                            int max_tokens, char *out, int max_out);

/* 기존: tools 없는 단순 body (호환용, 이제는 build_body_full 사용 권장) */
int context_build_body(const alchemy_context_t *ctx, const char *model,
                       char *out, int max_out);

/* 이력 초기화 */
void context_clear(alchemy_context_t *ctx);

/* ================================================================
 * Context Visualization (Claude Code의 /context 대응)
 * ================================================================ */

/* 컨텍스트 사용 통계 */
typedef struct {
    int total_chars;        /* 전체 문자 수 */
    int total_kb;           /* 전체 KB */
    int max_kb;             /* 최대 KB (window limit) */
    int usage_pct;          /* 사용률 0-100 */
    int msg_count;          /* 전체 메시지 수 */
    int user_count;         /* user 메시지 수 */
    int asst_count;         /* assistant 메시지 수 */
    int tool_count;         /* tool 메시지 수 */
    int system_bytes;       /* system prompt 크기 (외부에서 설정) */
    int tools_bytes;        /* tools spec 크기 (외부에서 설정) */
} context_stats_t;

/* 컨텍스트 사용 통계 계산
 * stats: 결과를 저장할 구조체
 * system_bytes: 시스템 프롬프트 크기 (0 = unknown)
 * tools_bytes: tools spec 크기 (0 = unknown)
 */
void context_stats(const alchemy_context_t *ctx, int system_bytes, int tools_bytes,
                   context_stats_t *stats);

/* 컨텍스트 시각화 (ASCII + progress bar)
 * Claude Code의 /context 명령어에 대응.
 * stats: context_stats()로 계산한 통계
 * out: 출력 버퍼
 * max_out: 버퍼 크기
 * returns: 쓴 바이트 수
 */
int context_visualize(const context_stats_t *stats, const alchemy_context_t *ctx,
                      char *out, int max_out);

/* 컨텍스트 시각화 (HTML fragment)
 * HTML 대시보드에 포함할 progress bar + breakdown
 */
int context_visualize_html(const context_stats_t *stats, const alchemy_context_t *ctx,
                           char *out, int max_out);
void context_keep_last(alchemy_context_t *ctx, int keep_count);

#ifdef __cplusplus
}
#endif

#endif /* ALCHEMY_CONTEXT_H */
