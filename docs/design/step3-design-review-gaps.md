# Step 3 설계 검토: 빈틈 분석

## 발견된 7개 구멍

### 🕳️ 1. Probe 자체가 rate limit을 소모함

**문제:** 5분마다 GPT에 probe("hi")를 보내면, probe 자체가 rate limit을 잡아먹음.
- probe를 위해 하루 288번 API 호출
- probe로 소진된 token만큼 실제 사용 가능 token이 줄어듦

**해결: `x-ratelimit-reset` 헤더를 활용하면 probe 불필요**
- OpenAI는 모든 응답에 `x-ratelimit-reset-tokens: 1h32m15s` 형식으로 **정확한 리셋 시간**을 줌
- remaining=0이면, reset 시간까지 **probe 없이 기다렸다가** 자동 복귀 가능
- **Anthropic만 probe 필요** (token bucket은 정확한 리셋 시간 없음)

→ **수정:** GPT probe 불필요. Anthropic만 30분 간격 probe.

---

### 🕳️ 2. Anthropic token bucket의 회복 속도를 모름

**문제:** "지속적 보충"이라고만 돼있고, 분당 얼마나 회복되는지 문서에 명시 안 됨.
- probe 간격을 얼마로 해야 할지 결정 불가

**해결: 경험적 학습**
- 실제 429를 받고, retry-after 헤더 확인
- retry-after 시간이 지난 후 probe → remaining 확인
- 이 패턴을 몇 번 반복하면 "분당 회복량"을 추정 가능

→ **수정:** Probe 로그 기반으로 회복 속도 추정

---

### 🕳️ 3. 429 발생 시 대기 전략 부재

**문제:** 현재 설계에 429 처리 로직이 없음
- 429 받으면 무조건 다음 모델로?
- 아니면 retry-after 기다렸다가 재시도?

**해결:**
```
429 from GPT → 
  1. retry-after 헤더 읽기
  2. 현재 모델을 "rate_limited" 상태로 설정
  3. 다음 우선순위 모델로 즉시 전환
  4. retry-after 시간 경과 후 → probe로 회복 확인
```

---

### 🕳️ 4. 모든 모델이 다 소진되면?

**문제:** GPT + Sonnet + DeepSeeven 다 못 쓰는 상황
- DeepSeek 잔액 부족?
- 일시적 네트워크 장애?
- API 자체 다운?

**해결:**
```
① DeepSeek 잔액 확인 API (DeepSeek API에 잔액 조회 엔드포인트 있는지 확인)
② 모든 모델 실패 시 → 에러 메시지 큐잉
③ 재시도 큐 (5분 후 자동 재시도)
```

---

### 🕳️ 5. 같은 rate limit window 내에서 probe가 실제 요청을 막을 수 있음

**문제:** remaining이 얼마 안 남은 상태에서 probe가 마지막 1회를 소진하면, 실제 사용자 메시지를 처리 못 함.

**해결:**
- Probe는 remaining > MIN_THRESHOLD(50) 일 때만 전송
- remaining < 50 이면 probe 금지
- 사용자 요청이 probe보다 항상 우선

---

### 🕳️ 6. 모델 간 응답 스타일 차이

**문제:** 같은 시스템 프롬프트를 줘도 GPT와 Sonnet과 DeepSeek의 응답 스타일이 다름
- GPT: 장황한 편
- Sonnet: 분석적
- DeepSeek: 간결

**해결:**
```
시스템 프롬프트에 모델별 스타일 보정 구문 추가:
  GPT: "간결하게 답변해"
  Sonnet: "한 문장으로 요약하지 말고 자연스럽게"
  DeepSeek: "이전 응답과 같은 스타일로"
```
→ 완전한 해결은 어렵지만, 최소한의 보정은 가능

---

### 🕳️ 7. DeepSeek 잔액 부족 시나리오

**문제:** DeepSeek도 종량제라 잔액이 0이면 사용 불가

**해결:**
- DeepSeek API에 잔액 조회 엔드포인트 확인 필요
- or 402/403 응답을 잔액 부족으로 감지
- 모든 모델 소진 시 "잔액 부족" 사용자 알림

---

## 수정된 설계

### Probe 전략 (최적화 후)

| 모델 | Probe 방식 | 간격 | 이유 |
|:----|:----------|:----|:-----|
| **GPT** | ❌ Probe 불필요 | — | `x-ratelimit-reset` 헤더로 정확한 리셋 시간 확인 가능 |
| **Sonnet** | ✅ 경량 probe | 30분 | Token bucket, 정확한 리셋 시간 없음 |
| **DeepSeek** | ❌ Probe 불필요 | — | 종량제, rate limit 없음 |

### 429 처리

```
응답 코드 429:
  → rate_limited = true
  → rate_limit_time = now
  → retry_after = header 값 (or 기본 60초)
  → 다음 우선순위 모델로 전환
  → retry_after * 2초 후 probe (exponential backoff)
```

### 응답 스타일 보정

```
[공통 시스템 프롬프트]
당신은 일관된 스타일로 답변합니다.
{model_hint}
반말을 사용하고, 따뜻하고 자연스럽게 답변합니다.

model_hint:
  GPT:  "답변은 3문장 이내로 간결하게 유지하세요."
  Sonnet: "지나치게 분석적인 표현은 피하고 자연스럽게 답변하세요."
  DeepSeek: "이전 응답 스타일과 일관되게 답변하세요."
```
