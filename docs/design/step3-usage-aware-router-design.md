# Step 3 설계: Usage-Aware Multi-Model Router

## 1. API 사용량 추적 메커니즘

### OpenAI
**응답 헤더** (모든 API 응답에 포함):
```
x-ratelimit-remaining-requests: N     # 남은 요청 수
x-ratelimit-remaining-tokens: N       # 남은 토큰 수
x-ratelimit-reset-requests: 1h32m15s  # 리셋까지 남은 시간
x-ratelimit-reset-tokens: 1h32m15s    # 리셋까지 남은 시간
```

### Anthropic
**토큰 버킷 알고리즘** (문서 명시) — 지속적 보충, 고정 리셋 없음
- RPM (requests/min), ITPM (input tokens/min), OTPM (output tokens/min)
- 응답 헤더에 남은 용량 포함
- 429 응답 시 `retry-after` 헤더

### DeepSeek
- 종량제: 제한 없음 (충전余额만 있으면 계속 사용 가능)

## 2. 라우팅 우선순위

```
① GPT 5.4 (월 구독)
   조건: x-ratelimit-remaining-tokens > 100 (최소 임계값)
   
② Sonnet 4.6 (월 구독)
   조건: GPT 한도 소진 AND x-ratelimit-remaining-tokens > 100
   
③ DeepSeek V4 (종량제)
   조건: GPT + Sonnet 둘 다 한도 소진
   
※ GPT 회복 감지: 다음 GPT API 응답 헤더에서 remaining > 0 확인
```

## 3. 컨텍스트 동기화

모델 전환 시 사용자 무감각 전환을 위해:

```
[공통 컨텍스트 포맷]
시스템: 너는 일관된 스타일로 응답하는 AI 어시스턴트야.
        (공통 시스템 프롬프트 - 모델별로 동일)

히스토리: [이전 메시지들 - JSON 배열, 모든 모델이 동일하게 수신]

현재 메시지: [사용자 입력]
```

**토크나이저 차이 대응:**
- 컨텍스트는 **텍스트(UTF-8) 그대로** 전달 (토큰 카운트 불필요)
- 각 모델 내부에서 자체 토크나이저로 처리
- 우리는 텍스트만 전달하면 됨 (C 코드에서 별도 작업 불필요)

## 4. Usage Tracker 구현

```c
typedef struct {
    const char *name;           // "openai", "anthropic", "deepseek"
    int remaining_requests;     // 남은 요청 수 (-1 = unknown)
    int remaining_tokens;       // 남은 토큰 수 (-1 = unknown)
    time_t last_check;          // 마지막 확인 시간
    int priority;               // 1=GPT, 2=Sonnet, 3=DeepSeek
} model_quota_t;

// 각 API 응답에서 헤더 파싱하여 remaining 업데이트
void quota_update_from_headers(model_quota_t *m, const char *response_headers);

// 사용 가능한 최우선 모델 반환
const model_quota_t *router_select_model(model_quota_t models[], int count);
```

## 5. 회복 감지 (Polling)

```
GPT 한도 소진 → DeepSeek 사용 중
         ↓
일정 간격(예: 5분)으로 GPT API에 
가벼운 요청(토큰 1개짜리) 전송
         ↓
x-ratelimit-remaining-tokens > 100?
         ↓
YES → GPT로 즉시 전환 (사용자 무감각)
```

## 6. 결정 테이블

| 상태 | 행동 |
|:----|:------|
| GPT remaining > 100 | GPT로 라우팅 |
| GPT remaining < 100, Sonnet > 100 | Sonnet으로 전환 |
| GPT+Sonnet 둘 다 소진 | DeepSeek으로 fallback |
| 다음 GPT 응답에서 remaining 회복 감지 | GPT로 자동 복귀 |
| DeepSeek 사용 중 GPT 회복 | 다음 요청부터 GPT로 (히스토리 유지) |
