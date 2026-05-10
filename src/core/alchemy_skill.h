/*
 * alchemy_skill.h
 * Alchemy Skill System — 경량 파일 기반 스킬 저장/검색
 *
 * skills/ 디렉터리의 .skill 파일을 스캔하여 메모리에 로드.
 * 각 스킬: 이름(파일명), 설명(첫줄 #), 본문(나머지).
 * 의존성: libc (POSIX dirent.h + stdio.h)
 */

#ifndef ALCHEMY_SKILL_H
#define ALCHEMY_SKILL_H

#include "alchemy_skill_evolve.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SKILL_NAME_LEN     64
#define SKILL_DESC_LEN     256
#define SKILL_CONTENT_LEN  4096   /* 최대 스킬 파일 크기 제한 (파싱용) */
#define SKILL_MAX_SKILLS   16

typedef struct {
    char name[SKILL_NAME_LEN];
    char description[SKILL_DESC_LEN];
    char category[32];      /* CI/CD, Coding, Research, Design, General */
    char *content;      /* 힙 할당 — 정적 배열 대신 필요한 만큼만 */
    int  content_len;   /* content의 실제 길이 (NULL 종료 제외) */
    skill_meta_t meta;  /* 경험치, 레벨, 점수 정보 */
} alchemy_skill_t;

/* skills/ 디렉터리에서 모든 .skill 파일 로드
 * path: skills 디렉터리 경로 (NULL = "./skills/")
 * returns: 로드된 스킬 수, -1 = 오류
 */
int skill_load_all(const char *path);

/* 이름으로 스킬 찾기 (정확 매칭) */
const alchemy_skill_t *skill_find(const char *name);

/* 설명에서 키워드 검색 (strstr)
 * returns: 첫 번째 매칭 스킬, NULL = 없음
 */
const alchemy_skill_t *skill_search(const char *keyword);

/* 등록된 스킬 수 */
int skill_count(void);

/* 인덱스로 스킬 접근 */
const alchemy_skill_t *skill_get(int idx);

/* 이름 목록을 문자열로 반환 (LLM 프롬프트용) */
int skill_list_names(char *out, int max_out);

/* 등록된 모든 스킬 메모리 해제 */
void skill_free_all(void);

/* 스킬 내용을 프롬프트에 주입 (LLM context용)
 * skill: 스킬 포인터
 * out: 출력 버퍼
 */
int skill_inject(const alchemy_skill_t *skill, char *out, int max_out);

#ifdef __cplusplus
}
#endif

#endif /* ALCHEMY_SKILL_H */
