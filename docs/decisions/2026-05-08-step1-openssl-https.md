---
date: 2026-05-08
type: decision
tags: [step1, ssl, openssl, implementation]
status: active
---

# Step 1: OpenSSL HTTPS 구현

## 맥락
LLM API 호출을 위해 HTTPS가 필요. Step 0의 TCP socket에 TLS 레이어를 추가.

## 결정
- OpenSSL **BIO** 고수준 API 사용 (`BIO_new_ssl_connect`)
- TLSv1.3 강제 (TLS_client_method)
- SNI 설정 (`SSL_set_tlsext_host_name`)
- 단일 버퍼로 응답 수신 (Step 0과 동일한 패턴)

## 결과
- TLS 핸드셰이크 성공 ✅
- 암호화: TLS_AES_256_GCM_SHA384
- HTTP 200 OK 수신
- 코드 증가: **+1,269 bytes** (text)
- strip 후 바이너리: **14,472 bytes**
