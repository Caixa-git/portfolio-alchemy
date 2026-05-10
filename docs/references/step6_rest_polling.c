/*
 * step6_core.c
 * Alchemy Core Loop (REST Polling)
 *
 * REST API 폴링 → Router → LLM → Tool → 응답
 *
 * 환경변수:
 *   DISCORD_TOKEN       (필수)
 *   DISCORD_POLL_CHANNEL (필수)
 *   DEEPSEEK_API_KEY    (선택, fallback)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include "alchemy_log.h"
#include "alchemy_router.h"
#include "alchemy_llm.h"
#include "alchemy_probe.h"
#include "alchemy_context.h"
#include "alchemy_window.h"
#include "alchemy_memory.h"
#include "alchemy_tool.h"
#include "alchemy_skill.h"

#define MAX_RESP_LEN 1900

/* OpenSSL HTTPS GET/POST */
static BIO *https_connect(const char *token, const char *method,
                          const char *path, const char *body, int body_len) {
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) return NULL;
    BIO *bio = BIO_new_ssl_connect(ctx);
    if (!bio) { SSL_CTX_free(ctx); return NULL; }
    SSL *ssl;
    BIO_get_ssl(bio, &ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    BIO_set_conn_hostname(bio, "discord.com:443");
    SSL_set_tlsext_host_name(ssl, "discord.com");
    if (BIO_do_connect(bio) <= 0) { BIO_free_all(bio); SSL_CTX_free(ctx); return NULL; }

    char req[4096];
    int n;
    if (body && body_len > 0) {
        n = snprintf(req, sizeof(req),
            "%s /api/v10%s HTTP/1.1\r\nHost: discord.com\r\nAuthorization: Bot %s\r\n"
            "Content-Type: application/json\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s",
            method, path, token, body_len, body);
    } else {
        n = snprintf(req, sizeof(req),
            "%s /api/v10%s HTTP/1.1\r\nHost: discord.com\r\nAuthorization: Bot %s\r\nConnection: close\r\n\r\n",
            method, path, token);
    }
    if (BIO_write(bio, req, n) <= 0) { BIO_free_all(bio); SSL_CTX_free(ctx); return NULL; }
    return bio;
}

/* HTTP 응답 읽기 + 상태코드 반환 */
static int https_read(BIO *bio, char *out, int max_out, int *status) {
    int total = 0, n;
    while ((n = BIO_read(bio, out + total, max_out - 1 - total)) > 0) total += n;
    out[total] = '\0';
    BIO_free_all(bio);
    int sc = 0;
    sscanf(out, "HTTP/1.1 %d", &sc);
    if (status) *status = sc;
    return sc;
}

/* JSON 문자열 값 추출 ("key":"value" 형태) */
static void json_str(const char *json, const char *key, char *out, int max) {
    out[0] = 0;
    char pat[64]; snprintf(pat, sizeof(pat), "\"%s\":", key);
    const char *p = strstr(json, pat);
    if (!p) return;
    p += strlen(pat);
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '"') return;
    p++;
    int i = 0;
    while (*p && *p != '"' && i < max-1) {
        if (*p == '\\' && *(p+1) == '"') { out[i++] = '"'; p+=2; }
        else if (*p == '\\' && *(p+1) == 'n') { out[i++] = '\n'; p+=2; }
        else if (*p == '\\' && *(p+1) == '\\') { out[i++] = '\\'; p+=2; }
        else { out[i++] = *p; p++; }
    }
    out[i] = 0;
}

/* 메시지 전송 */
static int send_msg(const char *token, const char *cid, const char *content) {
    char body[2048]; int bl = 0;
    bl += snprintf(body+bl, sizeof(body)-bl, "{\"content\":\"");
    for (int i = 0; content[i] && bl < (int)sizeof(body)-8; i++) {
        char c = content[i];
        if (c == '"' || c == '\\') { body[bl++] = '\\'; body[bl++] = c; }
        else if (c == '\n') { body[bl++] = '\\'; body[bl++] = 'n'; }
        else body[bl++] = c;
    }
    bl += snprintf(body+bl, sizeof(body)-bl, "\"}");

    char path[128]; snprintf(path, sizeof(path), "/channels/%s/messages", cid);
    BIO *bio = https_connect(token, "POST", path, body, bl);
    if (!bio) return -1;
    char resp[4096]; int sc;
    https_read(bio, resp, sizeof(resp), &sc);
    return (sc == 200) ? 0 : -1;
}

/* 메시지 수신 (after 기반) */
static int fetch_msg(const char *token, const char *cid,
                     const char *after_id,
                     char *content, int cmax,
                     char *username, int umax,
                     char *msgid, int idmax) {
    content[0] = username[0] = msgid[0] = 0;
    char path[256];
    if (after_id && after_id[0]) {
        snprintf(path, sizeof(path), "/channels/%s/messages?limit=5&after=%s", cid, after_id);
    } else {
        snprintf(path, sizeof(path), "/channels/%s/messages?limit=1", cid);
    }
    BIO *bio = https_connect(token, "GET", path, NULL, 0);
    if (!bio) return -1;
    char resp[8192]; int sc;
    https_read(bio, resp, sizeof(resp), &sc);
    if (sc != 200) return -1;

    char *body = strstr(resp, "\r\n\r\n");
    if (!body) return -1;
    body += 4;
    if (body[0] == '[' && body[1] == ']') return 0;

    /* 배열: 메시지 순회. 첫 번째(가장 최신) ID는 항상 저장 */
    int first = 1;
    const char *p = body;
    while (1) {
        const char *ob = strchr(p, '{');
        if (!ob) break;

        /* 닫는 중괄호 찾기 */
        int br = 0;
        const char *cb = NULL;
        for (const char *q = ob; *q; q++) {
            if (*q == '{') br++;
            else if (*q == '}') { br--; if (br == 0) { cb = q; break; } }
        }
        if (!cb) break;

        /* 이 message block만 추출 */
        int mlen = cb - ob + 1;
        char msgb[1024];
        if (mlen > (int)sizeof(msgb)-1) { p = cb + 1; continue; }
        memcpy(msgb, ob, mlen); msgb[mlen] = 0;

        /* 첫 번째(가장 최신) 메시지 ID 저장 */
        if (first) { json_str(msgb, "id", msgid, idmax); first = 0; }

        /* bot 체크 or 자기 자신 메시지 무시 */
        char tmp_un[64];
        json_str(msgb, "username", tmp_un, sizeof(tmp_un));
        int is_self = 0;
        if (strstr(msgb, "\"bot\": true") || strstr(msgb, "\"bot\":true")) is_self = 1;
        if (strstr(tmp_un, "채종협")) is_self = 1;

        if (!is_self) {
            json_str(msgb, "content", content, cmax);
            strncpy(username, tmp_un, umax-1);
            if (content[0]) return 1;
        }
        p = cb + 1;
    }
    return (msgid[0]) ? 0 : 0;
}

/* ================================================================ */
int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    setvbuf(stdout, NULL, _IONBF, 0);
    LOG_INIT("step6", LOG_LEVEL_INFO);

    const char *token = getenv("DISCORD_TOKEN");
    const char *cid = getenv("DISCORD_POLL_CHANNEL");
    if (!token || !cid) { fprintf(stderr, "Need DISCORD_TOKEN + DISCORD_POLL_CHANNEL\n"); return 1; }

    alchemy_router_t router; router_init(&router);
    alchemy_context_t ctx; context_init(&ctx);
    if (memory_exists(NULL)) memory_load(&ctx, NULL);
    tool_init();
    skill_load_all(NULL);

    const char *sp = "You are Alchemy, a helpful AI assistant. Use tools when needed. Be concise.";

    char last_id[32] = {0};
    printf("READY\n"); fflush(stdout);

    while (1) {
        char content[1024]={0}, user[64]={0}, mid[32]={0};
        fetch_msg(token, cid, last_id, content, sizeof(content), user, sizeof(user), mid, sizeof(mid));
        if (mid[0]) strncpy(last_id, mid, sizeof(last_id)-1);

        if (content[0]) {
            printf("\n[%s] %s\n", user, content); fflush(stdout);

            context_add(&ctx, "user", content);
            memory_save(&ctx, NULL);

            /* LLM + Tool 루프 */
            int tr = 0; char lr[8192]; char *fr = NULL;
            model_id_t m; const model_config_t *cfg = NULL; char *ak = NULL;

            for (;;) {
                if (tr >= MAX_TOOL_ROUNDS) break;
                if (tr == 0) { m = router_decide(&router); cfg = &router.configs[m]; }
                tr++;
                LOG_INFO(LOG_MOD_CORE, "LLM loop start, model=%s", cfg->name);
                if (router_need_probe(&router, m)) { probe_send_and_update(&router, m); m = router_decide(&router); cfg = &router.configs[m]; }
                ak = getenv(cfg->api_key);
                LOG_INFO(LOG_MOD_CORE, "API key check: %s=%s", cfg->api_key, ak ? "set" : "NULL");
                if (!ak) { ak = getenv("DEEPSEEK_API_KEY"); if (!ak) { send_msg(token, cid, "No API key."); LOG_INFO(LOG_MOD_CORE, "No API key"); goto done; } m = MODEL_DEEPSEEK; cfg = &router.configs[MODEL_DEEPSEEK]; }
                if (window_need_truncate(&ctx, m, WINDOW_RESPONSE_MARGIN)) window_trim_safe(&ctx, m, WINDOW_RESPONSE_MARGIN);
                char ts[4096]={0}; tool_build_spec(ts, sizeof(ts));
                char lb[8192]; int bl = context_build_body_full(&ctx, sp, cfg->model_name, ts, 4096, lb, sizeof(lb));
                if (bl < 0) { send_msg(token, cid, "Context build failed."); LOG_INFO(LOG_MOD_CORE, "Context build failed"); goto done; }
                LOG_INFO(LOG_MOD_CORE, "Calling llm_post to %s", cfg->endpoint);
                if (llm_post(cfg->endpoint, ak, lb, bl, lr, sizeof(lr)) != 0) { send_msg(token, cid, "LLM failed."); LOG_INFO(LOG_MOD_CORE, "LLM call failed"); goto done; }
                LOG_INFO(LOG_MOD_CORE, "LLM call succeeded");
                /* HTTP 400 확인 */
                int hsc=0; sscanf(lr, "HTTP/1.1 %d", &hsc);
                if (hsc >= 400) {
                    char *eb = strstr(lr, "\r\n\r\n"); if (eb) { eb += 4;
                    char errbuf[512]; int el = 0;
                    for (int i=0; eb[i] && el < 500 && i < 700; i++) { if (eb[i] >= 32 || eb[i]=='\n') errbuf[el++]=eb[i]; }
                    errbuf[el]=0;
                    LOG_INFO(LOG_MOD_CORE, "HTTP %d: %.300s", hsc, errbuf);
                    }
                }
                const char *he = strstr(lr, "\r\n\r\n");
                if (he) {
                    int hl = he - lr; char hb[2048];
                    if (hl > (int)sizeof(hb)-1) hl = sizeof(hb)-1;
                    memcpy(hb, lr, hl); hb[hl]=0;
                    if (m == MODEL_GPT) router_update_from_openai_headers(&router, m, hb);
                    else if (m == MODEL_SONNET) router_update_from_anthropic_headers(&router, m, hb);
                    int sc=0; sscanf(lr, "HTTP/1.1 %d", &sc);
                    if (sc == 429) { router_mark_429(&router, m, 60); goto done; }
                }
                char *bs = strstr(lr, "\r\n\r\n"); if (!bs) goto done; bs += 4;
                LOG_INFO(LOG_MOD_CORE, "Response body (300): %.300s", bs);
                char tn[MAX_TOOL_CALLS][TOOL_NAME_LEN], ta[MAX_TOOL_CALLS][TOOL_ARGS_LEN], ti[MAX_TOOL_CALLS][TOOL_CALL_ID_LEN];
                int nc = tool_parse_calls(bs, tn, ta, ti, MAX_TOOL_CALLS);
                if (nc > 0) {
                    context_add_tool_call(&ctx, "");
                    for (int i=0;i<nc;i++) { char r2[TOOL_RESULT_LEN]; tool_execute(tn[i], ta[i], r2, sizeof(r2)); context_add_tool_result(&ctx, ti[i], r2); }
                    memory_save(&ctx, NULL);
                    continue;
                }
                char rp[MAX_RESP_LEN]; int rl=0;
                const char *cp = strstr(bs, "\"content\":\"");
                if (cp) { cp += 11; while (*cp && *cp != '"' && rl < MAX_RESP_LEN-1) { if (*cp=='\\'&&*(cp+1)=='"') { rp[rl++]='"'; cp+=2; } else if (*cp=='\\'&&*(cp+1)=='n') { rp[rl++]='\n'; cp+=2; } else { rp[rl++]=*cp; cp++; } } }
                rp[rl]=0;
                if (rl > 0) { fr = rp; context_add(&ctx, "assistant", rp); memory_save(&ctx, NULL); }
                break;
            }
            if (fr) send_msg(token, cid, fr);
            else send_msg(token, cid, "Done.");
            done:;
        }
        usleep(2000000);
    }
    return 0;
}
