# ADR-0005: LLM 토큰 최적화 — 응답 캐싱 및 모델 라우팅

**상태:** 결정됨
**날짜:** 2026-05-08
**영향:** Step 3 (LLM Context Manager) 설계

## 맥락 (Context)

Alchemy는 경량 C 기반 Hermes Agent 재구현 프로젝트. DeepSeek V4 Flash 모델 사용 중.

질문: **LLM API 응답을 캐싱하는 계층을 Step 3에 포함해야 하는가?**

이 결정은 다음 세 가지 요소의 트레이드오프 분석을 요구함:
1. 토큰 비용 절감 효과
2. 캐시 구현으로 인한 바이너리 용량 증가
3. 캐시 구현으로 인한 성능/메모리 오버헤드

## 학술적 근거

[상세 연구 보고서](../research/llm-caching-academic-research.md) 참조.
arXiv/Semantic Scholar/ACM을 통해 7편의 관련 논문 분석.

### 핵심 논문

1. **vCache (ICLR 2026, arXiv:2502.03771)** — UC Berkeley, Stanford
   - 각 캐시 항목별 동적 임계값 최적화 (online learning)
   - 오류율 보장 + 최대 12.5배 히트율 향상

2. **GPT Semantic Cache (arXiv:2411.05276)**
   - Redis + embedding 기반 semantic cache
   - API 호출 68.8% 감소, hit rate 61.6~68.8%

3. **ConvoCache (Interspeech 2024, arXiv:2406.18133)**
   - 음성 챗봇 대상 실증 연구
   - 89% 캐시 응답 가능, latency 214ms

4. **MeTMaP (arXiv:2402.14480)**
   - Vector matching 정확도: 최고 41.51%
   - Semantic cache의 false match 리스크 실증

## 경제성 분석

### 모델별 비용 시나리오

| 모델 | Input 가격 | 1회 비용 (2K in + 0.5K out) | 월 500메시지 |
|------|:----------:|:--------------------------:|:----------:|
| DeepSeek V4 | $0.14/M | $0.0005 | **$7.5** |
| Claude Sonnet 4 | $3/M | $0.006 | **$90** |
| Claude Opus | $15/M | $0.03 | **$450** |
| GPT-4.1 | $2/M | $0.004 | **$60** |

### 캐시 도입 비용

| 항목 | 추정값 |
|------|:------:|
| 바이너리 증가 | ~8KB (LRU + embedding 비교) |
| 메모리 사용 | ~100KB (100 entries 기준) |
| 캐시 조회 지연 | ~0.5ms (코사인 유사도 계산) |
| 구현 복잡도 | ~300줄 C 코드 |

### 손익분기점

```
월간 절감액 = 월간 토큰 비용 × 예상 캐시 히트율
캐시 도입 비용 (1회) = 구현 시간 + 유지보수

DeepSeek V4 ($0.14/M):
  절감 = $7.5 × 60% = $4.5/월
  vs 캐시 바이너리 8KB + 메모리 100KB = 미미하지만 불필요

Claude Opus ($15/M):
  절감 = $450 × 60% = $270/월
  → 캐시 도입 필수
```

## 학술 연구 추가 (2026-05-08)

모델 전환 조건에 대한 논문 기반 원칙:

### Cascade Routing Optimality (ETH Zurich, 2410.10347)
- Routing + Cascading을 결합한 통합 프레임워크가 이론적 최적
- Quality estimator가 핵심: "이 프롬프트를 저비용 모델이 처리 가능한가?"

### HybridFlow Utility Model (Zhejiang, 2512.22137)
```
Utility(s, m) = α × AccuracyGain - β × Cost - γ × Latency
```

### 모델 전환 결정 테이블 (LLMBridge + ETH + HybridFlow 통합)

| 모델 비용 ($/M input) | 캐시 정책 | 라우팅 전략 | 학술 근거 |
|:--------------------:|:---------:|:-----------:|:---------:|
| < $0.5 | Dedup only | 단일 모델 (고정) | — |
| $0.5 ~ $5 | 경량 semantic cache | Cascade (저비용→고비용) | ETH cascade optimality |
| $5 ~ $15 | vCache 동적 임계값 | Cascade routing | ETH unified framework |
| > $15 | 전체 캐시 시스템 | HybridFlow utility | Zhejiang utility model |

**현재 (DeepSeek V4, $0.14/M):** 모든 학술 기준으로 최적화 불필요 구간.

[상세 연구 보고서](../research/step3-model-routing-academic-research.md) 참조.

## 결정 (변경 없음)

**지금은 캐시를 도입하지 않는다.**
대신 다음만 구현:

1. **Dedup (20줄):** 동일 메시지 3초 내 반복 요청 무시
2. **캐시 인터페이스预留 (interface预留):** 향후 `cache_get(key)`, `cache_put(key, value, ttl)` 함수 시그니처만 정의

### 조건부 전환 규칙

모델을 더 비싼 것으로 교체할 때:
- `$0.5/M input` 이하: dedup만 유지
- `$0.5~$5/M`: 경량 semantic cache (LRU + static threshold)
- `$5/M` 이상: vCache 스타일 동적 임계값 학습 캐시

## 참고

- [연구 보고서](../research/llm-caching-academic-research.md)
- [트레이드오프 원본 대화](https://discord.com/channels/@me/1500466688968364175)
