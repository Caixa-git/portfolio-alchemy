/*
 * alchemy_tool.h
 * Alchemy Tool Execution System
 *
 * LLM이 요청한 tool_calls를 실행하는 시스템:
 *   - Tool 레지스트리 (등록/찾기/실행)
 *   - LLM 응답에서 tool_calls JSON 파싱
 *   - tools JSON spec 생성 (LLM 요청 body에 포함)
 *
 * 내장 도구 (built-in):
 *   - web_search: 웹 검색
 *   - read_file:  파일 읽기
 *   - write_file: 파일 쓰기
 *   - terminal:   셸 명령어 실행
 */

#ifndef ALCHEMY_TOOL_H
#define ALCHEMY_TOOL_H

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================
 * 상수
 * ================================================================ */
#define TOOL_NAME_LEN      32
#define TOOL_DESC_LEN      128
#define TOOL_RESULT_LEN    16384   /* 16KB: write_file content, large output */
#define TOOL_ARGS_LEN      4096    /* 4KB: tool arguments (large file paths) */
#define TOOL_CALL_ID_LEN   48
#define MAX_TOOL_CALLS     8       /* 한 턴에 실행 가능한 최대 tool call 수 */
#define MAX_TOOL_ROUNDS    15      /* 한 메시지당 최대 tool→LLM 반복 수 */

/* ================================================================
 * Tool 정의
 * ================================================================ */
/* tool 함수 시그니처:
 *   args:   JSON 문자열 (LLM이 전달한 arguments)
 *   out:    결과 출력 버퍼
 *   out_size: 버퍼 크기
 *   returns: 0 = 성공, -1 = 실패
 */
typedef int (*tool_fn_t)(const char *args, char *out, int out_size);

typedef struct {
    char name[TOOL_NAME_LEN];
    char description[TOOL_DESC_LEN];
    tool_fn_t fn;
} alchemy_tool_t;

/* ================================================================
 * Tool 레지스트리
 * ================================================================ */

/* 시스템 초기화 (내장 도구 등록) */
void tool_init(void);

/* 사용자 정의 도구 등록 */
int tool_register(const alchemy_tool_t *tool);

/* 도구 이름으로 찾기 */
const alchemy_tool_t *tool_find(const char *name);

/* 등록된 도구 수 */
int tool_count(void);

/* index로 도구 가져오기 (0 ~ count-1) */
const alchemy_tool_t *tool_get(int idx);

/* ================================================================
 * Tool 실행
 * ================================================================ */

/* 단일 도구 실행 */
int tool_execute(const char *name, const char *args, char *out, int out_size);

/* ================================================================
 * LLM 요청 body용 tools JSON spec 생성
 * ================================================================
 * 생성 예:
 *   "tools":[{"type":"function","function":{"name":"web_search",
 *     "description":"...","parameters":{"type":"object",...}}}]
 */
int tool_build_spec(char *out, int out_size);

/* ================================================================
 * LLM 응답에서 tool_calls 파싱
 * ================================================================
 * body:  LLM 응답 JSON body ("choices" 배열 포함)
 * names:  발견된 tool call들의 이름 (배열)
 * args:   각 tool call의 arguments JSON (배열)
 * ids:    각 tool call의 id (배열)
 * max:    최대 파싱할 tool call 수
 * returns: 발견된 tool call 수 (0 = 없음, -1 = 오류)
 *
 * names[i] / args[i] / ids[i] 버퍼 크기:
 *   names: TOOL_NAME_LEN
 *   args:  TOOL_ARGS_LEN
 *   ids:   TOOL_CALL_ID_LEN
 */
int tool_parse_calls(const char *body,
                     char names[][TOOL_NAME_LEN],
                     char args[][TOOL_ARGS_LEN],
                     char ids[][TOOL_CALL_ID_LEN],
                     int max);

#ifdef __cplusplus
}
#endif

/* LLM 응답 body에서 message 객체 추출 (tool_calls + reasoning_content 포함) */
int tool_extract_calls_json(const char *body, char *out, int max_out);

#endif /* ALCHEMY_TOOL_H */
