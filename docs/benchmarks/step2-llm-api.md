---
date: 2026-05-08
type: benchmark
tags: [step2, llm, api, post]
status: active
---

# Step 2 벤치마크: LLM API 호출 (HTTPS POST)

## 실행 결과 (API 키 없음)
```
time:21:36:46  level:ERROR  app:step2  mod:core  msg:LLM_API_KEY not set
```
→ API 키 없이 graceful 종료, LTSV 로그 정상 기록 확인

## 바이너리 크기 (Step 1 vs Step 2)

| 섹션 | Step 1 (SSL) | Step 2 (LLM) | 증가량 |
|:-----|:----------:|:-----------:|:------:|
| text (코드) | 7,109 | 10,783 | **+3,674** |
| data | 4,974 | 5,014 | +40 |
| bss | 16 | 16 | +0 |
| **dec (전체)** | **12,099** | **15,813** | **+3,714 (30.7%)** |
| strip 후 파일 | - | **22,704** | - |

## 분석
- HTTPS POST + JSON 처리 + URL 파싱 추가로 text 3.7KB 증가
- 기존 SSL 연결 코드(step1) 재사용하여 중복 최소화
- strip 후 22.7KB — 여전히 초경량

## Step별 전체 비교 (누적)

| Step | 설명 | text | dec | strip |
|:----:|------|:---:|:---:|:----:|
| 0 | TCP HTTP GET | 3,992 | 4,736 | - |
| 1 | OpenSSL HTTPS | 7,109 | 12,099 | 14,472 |
| 2 | LLM API POST | 10,783 | 15,813 | 22,704 |
