---
date: 2026-05-08
type: benchmark
tags: [step1, ssl, https, openssl]
status: active
---

# Step 1 벤치마크: OpenSSL HTTPS GET

## 측정 환경
- 호스트: WSL2 (Ubuntu 24.04)
- 컴파일러: gcc 13.3.0 (기본 CFLAGS, -g 포함)
- OpenSSL: 3.0.13
- 대상: example.com:443 → GET /

## 실행 결과
```
[CONNECT] example.com:443 (TLS) ...
[CONNECT] TLS 핸드셰이크 성공!
  암호화 프로토콜: TLSv1.3
  암호화 스위트: TLS_AES_256_GCM_SHA384

[SEND] HTTPS GET transmitted
[RECV] Reading response ...

=== RESPONSE (837 bytes) ===
HTTP/1.1 200 OK
Server: cloudflare
...
[DONE] TLS connection closed.
```

## 바이너리 크기 (Step 0 vs Step 1)

| 섹션 | Step 0 (socket) | Step 1 (OpenSSL) | 증가량 |
|:-----|:--------------:|:----------------:|:------:|
| text (코드) | 3,992 | 5,261 | +1,269 |
| data | 728 | 800 | +72 |
| bss | 16 | 16 | +0 |
| **dec (전체)** | **4,736** | **6,077** | **+1,341 (28.3%)** |
| 파일 크기 (debug) | 21,152 | 21,216 | +64 |
| **strip 후 파일** | - | **14,472** | - |

## 분석
- OpenSSL 추가로 인한 실제 코드 증가: **1,269 bytes (text)**
- TLS 1.3 + AES-256-GCM 암호화 통신 정상 확인
- Step 0과 동일한 HTTP 응답 수신 → 동작 일치 ✅
- strip 시 14,472 bytes → 향후 LTO + gc-sections 적용 시 더 감소 예상

## 실행 로그
![실행 출력](../screenshots/step1-https-output.txt)
