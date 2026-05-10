---
date: 2026-05-08
type: benchmark
tags: [step0, tcp, http, baseline]
status: active
---

# Step 0 벤치마크: TCP Socket HTTP GET

## 측정 환경
- 호스트: WSL2 (Ubuntu 24.04)
- CPU: (Windows 호스트)
- 컴파일러: gcc 13.3.0 (기본 CFLAGS, no optimization)
- 대상: example.com:80 → GET /

## 실행 결과
```
[CONNECT] example.com:80 ...
[CONNECT] OK
[SEND] HTTP GET transmitted

=== RESPONSE (837 bytes) ===
HTTP/1.1 200 OK
Content-Type: text/html
Transfer-Encoding: chunked
Server: cloudflare
...
```

## 바이너리 크기

| 섹션 | 크기 (bytes) |
|------|:-----------:|
| text (코드) | 3,992 |
| data (초기화된 데이터) | 728 |
| bss (초기화되지 않은 데이터) | 16 |
| **총계 (dec)** | **4,736** |
| **파일 크기** | **16,704** |

## 응답 시간
- TCP 연결: 즉시
- HTTP 응답 수신: 837 bytes (chunked)
- 연결 종료: 정상

## 스크린샷
![Example.com 페이지](screenshots/step0-example-dot-com.png)
*브라우저로 확인한 example.com 페이지*

## 참고
- 외부 라이브러리 의존성: **0**
- 사용한 시스템 콜: socket, connect, send, recv, close, gethostbyname
- 이 벤치마크가 향후 모든 모듈의 **baseline**이 됨
