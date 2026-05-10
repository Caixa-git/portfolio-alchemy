# 초경량 TCP/IP 스택 설계 철학

## 참고 자료
- **uIP** (Adam Dunkels, 2001) — "uIP: A Lightweight IP Stack for 8-bit Microcontrollers"
- **lwIP** — "lwIP: A Lightweight TCP/IP Stack"
- **TinyTCP** — 최소한의 TCP 구현

## 핵심 인사이트

### uIP의 설계 철학
- **Event-driven protothread** 방식: 각 연결이 상태 머신으로 동작
- 버퍼는 **단일 전역 패킷 버퍼** 하나만 사용 (zero-copy)
- TCP 스택 전체가 **약 10KB ROM, 1KB RAM** 수준
- BSD socket API 없이 직접 이벤트 콜백 구조

```c
// uIP의 핵심: 단일 버퍼로 송수신 처리
static uint8_t uip_buf[UIP_BUFSIZE];  // 전역 패킷 버퍼

void uip_process(uint8_t flag) {
    // TCP 상태 머신 (약 30개 상태)
    switch(uip_connr->tcpstateflags & UIP_TS_MASK) {
        case UIP_CLOSED:     ...
        case UIP_SYN_RCVD:   ...
        case UIP_ESTABLISHED:...
    }
}
```

### lwIP의 차별점
- uIP보다 큰 메모리 (40KB+ ROM)지만 **멀티스레딩 지원**
- **pbuf** 체인으로 zero-copy 버퍼 관리
- BSD socket API 호환 레이어 제공 (선택사항)

### 우리 프로젝트에 적용할 점
1. **단일 전역 버퍼** — 동적 할당 제거
2. **상태 머신 기반** — 각 연결을 구조체+함수포인터로
3. **이벤트 루프** — select/epoll 없이 순차 처리

## 성능 벤치마크 참고
| 스택 | ROM | RAM | 처리량 |
|------|-----|-----|--------|
| uIP | ~10KB | ~1KB | ~100KB/s |
| lwIP | ~40KB | ~10KB | ~1MB/s+ |
| **우리 목표** | **<5KB** | **<512B** | **충분 (채팅 수준)** |
