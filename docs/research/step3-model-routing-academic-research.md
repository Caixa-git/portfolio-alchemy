# Step 3 학술 연구: 비용 기반 모델 라우팅 및 캐싱 정책

> 조사일: 2026-05-08
> 범위: arXiv (CS.CL, CS.DC, CS.LG)
> 출처: ETH Zurich, Tufts University, Zhejiang University 등

---

## [1] A Unified Approach to Routing and Cascading for LLMs
- **arXiv:** 2410.10347
- **저자:** Jasper Dekoninck, Maximilian Baader, Martin Vechev (ETH Zurich, Switzerland)
- **상태:** v3 (May 2025)

### 수학적 핵심 기여

**Routing (단일 모델 선택):**
```
목적: argmin_{m ∈ M} Cost(m)  s.t.  Quality(m, q) ≥ τ

where:
  M = 사용 가능한 모델 집합
  Cost(m) = 모델 m의 API 비용 (token cost)
  Quality(m, q) = 추정된 응답 품질 (quality estimator)
  τ = 최소 허용 품질 임계값
```

**Cascading (순차적 모델 체인):**
```
m₁ → m₂ → ... → mₙ
if Quality(mᵢ, q) < τ then try mᵢ₊₁

최적 비용:
  E[Cost] = Cost(m₁) + Σ P(skip m₁..ᵢ) × Cost(mᵢ₊₁)
```

**Cascade Routing (통합 프레임워크):**
Routing과 Cascading을 결합한 **이론적 최적 전략**.
- 좋은 quality estimator가 핵심 성공 요인
- 단순 routing이나 cascading보다 consistently 우월

---

## [2] HybridFlow: Resource-Adaptive Subtask Routing
- **arXiv:** 2512.22137
- **저자:** Jiangwen Dong, Jiayu Li, Tianhang Zheng, Wanyu Lin (Zhejiang University)
- **상태:** v4 (Jan 2026)

### 수학적 핵심 기여

**Benefit-Cost Utility Model:**

```
Utility(s, m) = α × AccuracyGain(s, m) - β × Cost(s, m) - γ × Latency(s, m)

Routing Decision:
  if Utility(edge) ≥ Utility(cloud) → edge
  else → cloud

where:
  s = subtask
  m = model
  α, β, γ = 학습 가능한 가중치 (budget-adaptive)
```

**실험 결과:** GPQA, MMLU-Pro, AIME24, LiveBench-Reasoning에서 cost-accuracy tradeoff 개선.

---

## [3] LLMBridge: Cost-Aware LLM Proxy
- **arXiv:** 2410.11857
- **저자:** Noah Martin et al. (Tufts University, USA)
- **상태:** 12개월 실사용 검증 (WhatsApp Q&A, 14.7K requests)

### 세 가지 최적화 전략

1. **Model Selection:** 프롬프트 복잡도에 따라 모델 라우팅
2. **Context Management:** 컨텍스트 길이 지능적 축소
3. **Semantic Caching:** 로컬 모델 + vector DB

**비용-품질 트레이드오프 인터페이스:**
```
Application → CostPreference(x) → LLMBridge → Model Selection
                                        ↓
                                  max(Utility) = Quality - λ × Cost
```

---

## 통합 프레임워크: Step 3 설계 원칙

### 핵심 방정식

```
ModelSelection(q) = argmin_{m} Cost(m)
  s.t. Quality(m, q) ≥ τ_routing  (Routing: cascade-routing optimal)

SwitchCondition:
  if TokenBudget_remaining < TokenBudget_threshold × CacheHitRate:
    switch_to_cheaper_model(m_{cheap})
  elif CacheHitRate < cache_hit_threshold:
    expand_cache_or_improve_embeddings
```

### 가격 기반 결정 테이블 (앞선 ADR-0005 확장)

| 모델 비용 ($/M input) | 캐시 정책 | 라우팅 전략 |
|:--------------------:|:---------:|:-----------:|
| < $0.5 | Dedup only | 단일 모델 (고정) |
| $0.5 ~ $5 | 경량 semantic cache | Cascade (저비용→고비용 순) |
| $5 ~ $15 | vCache 동적 임계값 | Cascade routing (ETH optimal) |
| > $15 | 전체 캐시 시스템 | HybridFlow utility model |

### 적용: Alchemy

현재 DeepSeek V4 기준 ($0.14/M):
- 캐시 불필요 (ADR-0005 결정 유지)
- 모델 라우팅 불필요 (단일 모델)
- **Step 3은 최소 구성으로:**
  1. Quality estimator (간단한 fallback: "LLM이 실패하면 재시도")
  2. Token 카운터 (근사치) 
  3. Context trim (4K 초과 시 자르기)

미래 고비용 모델 전환 시:
- 위 테이블 따라 캐시/라우팅 점진적 활성화
- ETH의 cascade routing optimality proofs 활용
