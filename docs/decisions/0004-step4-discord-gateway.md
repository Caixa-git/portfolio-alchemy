# ADR-0004: Step 4 — Discord Gateway

**상태:** 구현 완료
**날짜:** 2026-05-08
**영향:** Step 5 (Core Loop)의 입력 계층

## 결정

C 기반 WebSocket 클라이언트 + Discord Gateway API v10 구현.

## 구현 상세

### WebSocket (RFC 6455)
- `ws_connect()`: HTTP Upgrade + TLS (OpenSSL BIO)
- `ws_read_frame()`: 프레임 헤더 파싱 (FIN/opcode/mask/payload_len)
- `ws_send_text()`: 텍스트 프레임 전송 (masking 포함)
- Ping/Pong/Close 프레임 처리

### Discord Gateway v10
- OP 10 Hello → heartbeat_interval 추출
- OP 2 Identify → 봇 토큰 + intents 전송
- OP 0 Dispatch → READY / MESSAGE_CREATE 이벤트 처리
- OP 1 Heartbeat → 70% interval 주기
- OP 11 Heartbeat ACK 수신

### Intents
- GUILD_MESSAGES(512) + DIRECT_MESSAGES(4096) + MESSAGE_CONTENT(32768) = 37376

## 벤치마크
- 바이너리: text 20,762 / strip 44K
- WebSocket text frame payload 최대 2242 bytes (READY 이벤트)
- Heartbeat Interval: ~41.25초 (Discord 기본)
