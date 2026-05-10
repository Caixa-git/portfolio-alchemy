---
date: 2026-05-08
type: decision
tags: [architecture, pattern]
status: active
---

# 아키텍처: Reactor + Microkernel 하이브리드

## 맥락
AI 에이전트는 여러 I/O 채널(Discord WebSocket, LLM HTTP, stdin)을 동시에 처리해야 함.
단일 스레드에서 효율적으로 처리할 수 있는 패턴이 필요.

## 결정
- **Reactor 패턴**: epoll 기반 이벤트 루프로 모든 I/O 단일 스레드 처리
- **Microkernel**: 코어는 메시지 라우팅만. Discord/LM/도구는 독립 모듈
- 참고: uIP(Adam Dunkels)의 단일 버퍼 상태 머신 철학 차용

## 결과
멀티스레드 동기화 문제 제로, 컨텍스트 스위칭 오버헤드 없음.
모듈 추가가 간단해짐 (함수 포인터 테이블 등록만).
