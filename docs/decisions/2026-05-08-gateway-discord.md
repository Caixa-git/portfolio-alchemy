---
date: 2026-05-08
type: decision
tags: [gateway, discord]
status: active
---

# 게이트웨이: Discord 전용

## 맥락
Hermes Agent는 Telegram, Slack, WhatsApp 등 다양한 게이트웨이 지원.
현재 사용 패턴은 Discord DM 한 곳에서 모든 작업을 처리.

## 결정
게이트웨이는 **Discord** 하나만 구현.
WebSocket 기반 Discord Gateway API로 메시지 송수신.

## 결과
게이트웨이 코드 단순화 (소켓 1개로 모든 통신 처리).
향후 다른 플랫폼 추가는 별도 모듈로 확장 가능.
