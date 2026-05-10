---
date: 2026-05-08
type: decision
tags: [llm, api]
status: active
---

# LLM 공급자 3개로 제한

## 맥락
기존 Hermes Agent는 Ollama, vLLM, GGUF 등 수많은 LLM 백엔드를 지원.
실제 사용 패턴상 DeepSeek / OpenAI / Anthropic 3개만 사용.

## 결정
- **DeepSeek** (deepseek-chat)
- **OpenAI** (GPT-4o, GPT-4o-mini)
- **Anthropic** (Claude 3.5 Sonnet, Claude 4)

3개로 제한. 모두 OpenAI-compatible API 형식이므로 단일 HTTP 클라이언트로 처리.

## 결과
LLM 라우터 코드 단순화, 바이너리 감소.
새 LLM 추가는 endpoint 구조체만 추가하면 되므로 확장성 유지.
