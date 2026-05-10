#ifndef ALCHEMY_AGENT_H
#define ALCHEMY_AGENT_H

#include "alchemy_context.h"
#include "alchemy_tool.h"
#include "alchemy_model.h"
#include "alchemy_llm_router.h"

#define AGENT_SYSTEM_PROMPT_MAX 16384
#define AGENT_REPLY_MAX 4096

typedef struct {
    alchemy_context_t ctx;
    char system_prompt[AGENT_SYSTEM_PROMPT_MAX];
    router_config_t *router;           /* NULL = use legacy llm_post() */
    char last_error[256];               /* last error message */
    int  total_llm_calls;
    int  total_tool_calls;
    int  total_fallback_count;
    stream_cb_t stream_cb;             /* streaming callback (NULL = blocking) */
    void *stream_userdata;             /* userdata for stream callback */
} alchemy_agent_t;

/* Initialize agent: context, tool, skill */
void agent_init(alchemy_agent_t *agent);

/* Set router for multi-provider + retry + fallback */
void agent_set_router(alchemy_agent_t *agent, router_config_t *router);

/* Load system prompt from file (fallback if not found) */
void agent_load_prompt(alchemy_agent_t *agent, const char *path);

/*
 * Process one user message through LLM + Tool loop.
 * Uses router if available, otherwise legacy llm_post().
 *
 * Flow:
 *   1. context_add("user", user_msg) → memory_save
 *   2. LLM 호출 (router_select → router_send or llm_post)
 *   3. On failure: router_cooldown → retry next provider (up to 3)
 *   4. tool_calls 있으면 → 실행 → 결과 context 추가 → 재호출
 *   5. tool_calls 없으면 → content 추출 → context_add → memory_save
 *
 * Returns 0 on success, -1 on error.
 */
int agent_process_message(alchemy_agent_t *agent,
                          const char *api_key, const char *endpoint, const char *model,
                          const char *user_msg,
                          char *reply, int max_reply);

/* Get agent stats as JSON string */
int agent_stats_json(const alchemy_agent_t *agent, char *out, int max_out);

#endif /* ALCHEMY_AGENT_H */
