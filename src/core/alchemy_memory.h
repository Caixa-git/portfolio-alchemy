/*
 * alchemy_memory.h
 * Alchemy Memory — 파일 기반 Context Persist
 *
 * 바이너리 포맷으로 context를 파일에 저장/로드
 * 프로세스 재시작 후에도 대화 이력 유지
 */

#ifndef alchemy_MEMORY_H
#define alchemy_MEMORY_H

#include "alchemy_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 기본 메모리 파일 경로 */
#define MEMORY_DEFAULT_PATH "./alchemy.mem"

/* Context를 파일에 저장
 * ctx: 저장할 컨텍스트
 * path: 파일 경로 (NULL이면 MEMORY_DEFAULT_PATH)
 * returns: 0 = 성공, -1 = 실패
 */
int memory_save(const alchemy_context_t *ctx, const char *path);

/* 파일에서 Context 로드
 * ctx: 로드 대상 (초기화 후 채워짐)
 * path: 파일 경로 (NULL이면 MEMORY_DEFAULT_PATH)
 * returns: 0 = 성공, -1 = 파일 없음/손상
 */
int memory_load(alchemy_context_t *ctx, const char *path);

/* 메모리 파일 존재 여부 확인 */
int memory_exists(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* alchemy_MEMORY_H */
