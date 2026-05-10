---
date: 2026-05-08
type: decision
tags: [step4, discord, websocket]
status: active
---

# Step 4: Discord Gateway

## 맥락
Hermes Agent의 핵심은 Discord 채팅으로 에이전트와 소통하는 것.
WebSocket 기반 Discord Gateway API v10 구현 필요.

## 결정
- WebSocket: easywsclient 구조 참고, C로 직접 구현
- Discord Gateway v10: WebSocket → OP 10 Hello → OP 2 Identify → Heartbeat → Dispatch
- intents: DM(4096) + MESSAGE_CONTENT(32768) = 37376
- 환경변수 DISCORD_TOKEN으로 봇 토큰 관리
- Step 2의 LLM 호출 코드와 결합하여 메시지 수신 시 응답 가능

## 결과
- text: 13,935 bytes (+3,152)
- strip 후: 26,800 bytes
- easywsclient의 WebSocket 프레임 구조 재사용으로 구현 효율화
- 외부 라이브러리 의존성 없음 (OpenSSL만)
