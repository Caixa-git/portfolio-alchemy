/*
 * alchemy_llm.h
 * Alchemy LLM API 호출 인터페이스
 *
 * OpenAI-compatible HTTPS POST /chat/completions
 * 최소 의존성: libc + OpenSSL
 */

#ifndef alchemy_LLM_H
#define alchemy_LLM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Raw JSON body를 직접 POST
 * endpoint:  "https://api.openai.com/v1"
 * api_key:   "sk-xxx"
 * body:      전체 JSON body (model + messages 포함)
 * body_len:  body 길이
 * response_out: HTTP 응답 전체 (헤더 + 본문)
 * max_response: 버퍼 크기
 *
 * 반환: 0 = 성공, -1 = 실패
 */
int llm_post(const char *endpoint, const char *api_key,
             const char *body, int body_len,
             char *response_out, int max_response);

/* LLM 채팅 API 호출 (단일 메시지, 편의용)
 * 내부에서 body 생성 후 llm_post() 호출
 */
__attribute__((deprecated)) int llm_chat(const char *endpoint, const char *api_key,
             const char *model, const char *prompt,
             char *response_out, int max_response);

#ifdef __cplusplus
}
#endif

#endif /* alchemy_LLM_H */
