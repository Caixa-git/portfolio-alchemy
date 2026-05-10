# 벤치마크 기록

> 모든 모듈은 구현 → 측정 → 기록 사이클을 따릅니다.

## Step별 비교

| Step | 설명 | text (bytes) | data (bytes) | dec (bytes) | strip 후 |
|:----:|------|:-----------:|:-----------:|:----------:|:--------:|
| 0 | TCP HTTP GET | 3,992 | 728 | 4,736 | - |
| 1 | OpenSSL HTTPS | 7,109 | 4,974 | 12,099 | 14,472 |
| 4 | Discord Gateway | 13,935 | 5,046 | 18,997 | 26,800 |
| 2 | LLM API POST | 10,783 | 5,014 | 15,813 | 22,704 |

## 상세

| 문서 | 내용 |
|------|------|
| [step0-baseline.md](step0-baseline.md) | Step 0: 순수 TCP socket HTTP GET (baseline) |
| [step1-openssl-https.md](step1-openssl-https.md) | Step 1: OpenSSL HTTPS GET |
| [step4-discord-gateway.md](step4-discord-gateway.md) | Step 4: Discord Gateway |
| [step2-llm-api.md](step2-llm-api.md) | Step 2: LLM API POST |

---

*마지막 업데이트: 2026-05-08*
