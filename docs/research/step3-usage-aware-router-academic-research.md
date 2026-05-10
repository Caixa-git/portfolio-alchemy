# Step 3 학술 연구: 사용량 기반 멀티모델 라우터

> 조사일: 2026-05-08
> 목적: 월 구독(OpenAI/Anthropic) + 종량제(DeepSeek) 모델 간 seamless 전환
> 핵심: 사용자는 모델 전환을 인지하지 못해야 함

---

## [1] EMAFusion: Self-Optimizing Seamless LLM Selection (⭐ 가장 관련성 높음)
- **arXiv:** 2504.10681
- **저자:** Soham Shah, Kumar Shridhar, Surojit Chatterjee, Souvik Sen
- **날짜:** April 2025

### 핵심 기여
"Self-Optimizing System for Seamless LLM Selection and Integration"

**세 가지 라우터 통합:**
1. **Taxonomy-based router** — familiar query types
2. **Learned router** — ambiguous inputs
3. **Cascading** — progressively escalate cheap→expensive based on multi-judge confidence

**성능:**
- Best individual model보다 **2.6%p 향상** (94.3% vs 91.7%)
- 평균 비용 대비 **4X 저렴**
- GPT-4 대비 **1/20 비용**으로 17.1%p 향상

**우리에게 적용:**
Taxonomy router → "이 질문 유형은 OpenAI/Anthropic/DeepSeek 중 어디?"

---

## [2] Unified Approach to Routing and Cascading for LLMs (ETH Zurich)
- **arXiv:** 2410.10347
- **저자:** Jasper Dekoninck, Maximilian Baader, Martin Vechev

### 핵심 기여
Routing + Cascading **통합 프레임워크의 이론적 최적성 증명**

**Cascade Routing (통합 전략):**
```
1. Quality estimator로 각 모델별 예상 품질 추정
2. 저비용 모델부터 시도
3. quality < threshold → 다음 모델로 cascade
4. Best model selection = cascade routing
```

---

## [3] LLMBridge: Cost-Aware LLM Proxy (Tufts University)
- **arXiv:** 2410.11857
- **실사용 검증:** WhatsApp Q&A, 14.7K 요청, 12개월

### 핵심 기여
세 가지 최적화 전략을 LLM Proxy로 통합:
1. **Model selection** — prompt complexity 기반
2. **Context management** — 지능적 컨텍스트 축소
3. **Semantic caching** — local models + vector DB

**비용-품질 인터페이스:**
```
Application → CostPreference(x) → LLMBridge → Router
                                        ↓
                             max(Utility) = Quality - λ × Cost
```

---

## [4] HybridFlow: Resource-Adaptive Subtask Routing (Zhejiang)
- **arXiv:** 2512.22137

### 핵심 기여
**Benefit-Cost Utility Model for subtask-level routing:**
```
Utility(s, m) = α × AccuracyGain(s, m) - β × Cost(s, m) - γ × Latency(s, m)
```

---

## 설계 원칙: Alchemy Usage-Aware Router

### 라우팅 결정 트리

```
[사용자 메시지]
     │
     ▼
Usage-Aware Router:
     │
     ├─ OpenAI 일일 한도 남음? → GPT 5.4
     │
     ├─ Anthropic 일일 한도 남음? → Sonnet 4.6
     │     (둘 다 남으면: Round-Robin 또는 질문 유형 기반)
     │
     └─ 둘 다 소진? → DeepSeek V4 (종량제 fallback)

[모든 경로] → 컨텍스트 유지 → [사용자에게 응답]
```

### 컨텍스트 동기화 원칙
모델 변경 시 사용자가 느끼는 차이 최소화:
1. **시스템 프롬프트 공통 템플릿** — 모델별 차이를 시스템 프롬프트로 보정
2. **히스토리 유지** — 채팅 히스토리를 모든 모델이 동일하게 수신
3. **응답 스타일 통일** — 각 모델에 "너는 일관된 스타일로 응답해" 지시

### Usage Tracking
각 모델의 일일 사용량 API 엔드포인트:
- OpenAI: `GET https://api.openai.com/dashboard/billing/usage`
- Anthropic: `GET https://api.anthropic.com/v1/usage` (또는 rate limit 헤더)

---

## 결론

이 프레임워크는 **EMAFusion의 통합 라우터 + ETH의 cascade optimality + HybridFlow의 utility model**을 종합한 것.

**C로 구현 시:** 
- Taxonomy router: 간단한 if/else 체인 (질문 길이, 키워드 기반)
- Usage tracker: 최근 24시간 호출 카운터
- Seamless 전환: 사용자에게 "모델이 전환되었습니다" 따위의 메시지 출력 금지
