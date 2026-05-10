---
date: 2026-05-08
type: decision
tags: [step2, llm, post, json]
status: active
---

# Step 2: LLM API 호출 (OpenAI-compatible HTTPS POST)

## 맥락
LLM API와 통신하려면 HTTPS POST + JSON body + JSON 응답 파싱 필요.
외부 JSON 라이브러리 없이 snprintf와 strstr로 최소 처리.

## 결정
- **HTTP POST**: step1의 OpenSSL BIO 기반, method만 GET→POST
- **JSON body**: snprintf로 직접 생성 (zero alloc)
- **JSON 응답**: strstr로 `"content":"` 검색 후 추출
- **API 키**: 환경변수 `LLM_API_KEY` (코드에 하드코딩 금지)
- **엔드포인트**: 환경변수 `LLM_ENDPOINT` (기본: api.openai.com/v1)
- **모델**: 환경변수 `LLM_MODEL` (기본: gpt-4o-mini)
- **3개 LLM 공통**: 모두 OpenAI-compatible API이므로 단일 코드로 처리

## 결과
- Step 1 대비 **text +3,674 bytes (30.7% 증가)**
- strip 후 **22,704 bytes**
- JSON 라이브러리 없는 순수 C, zero malloc
- DeepSeek/OpenAI/Anthropic 모두 동일 바이너리로 사용 가능
