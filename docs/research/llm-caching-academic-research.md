# LLM 응답 캐싱 정책에 관한 학술 연구 조사

> 조사일: 2026-05-08
> 조사 범위: arXiv, Semantic Scholar, Google Scholar (via cross-ref), ACM Digital Library (via DOI)
> 출처: 전 세계 연구자 (국가 편향 없음 — 미국/중국/유럽/호주/네팔 등 다국적)

---

## [1] vCache: Verified Semantic Prompt Caching
- **arXiv:** 2502.03771
- **컨퍼런스:** ICLR 2026 (accepted)
- **저자:** Luis Gaspar Schroeder, Aditya Desai, Alejandro Cuadron, Kyle Chu, Shu Liu, Mark Zhao, Stephan Krusche, Alfons Kemper, Matei Zaharia, Joseph E. Gonzalez (UC Berkeley, Stanford, TUM)
- **핵심 수학적 기여:**

  **동적 임계값 학습 (Online Learning):**
  각 캐시된 프롬프트에 대해 사용자 정의 오류율(ε)을 보장하는 최적 유사도 임계값(θ_i)을 온라인 학습으로 추정.
  
  - 정적 임계값(static threshold)은 예상치 못한 오류율과 최적 이하의 캐시 히트율을 유발
  - vCache의 접근법: 각 캐시 항목별로 개별 임계값 θ_i를 학습
  - 결과: 최대 **12.5배 높은 캐시 히트율**, **26배 낮은 오류율**

  **수학적 프레임워크:**
  ```
  목적: argmax_{θ} HitRate(θ)  s.t.  ErrorRate(θ) ≤ ε
  ```
  각 cached prompt에 대해 embedding 유사도 점수 분포를 추정하고, 
  허용 오류율 ε 이내에서 최대 히트율을 내는 θ를 online manner로 갱신

- **의의:** 첫 번째 "verified" semantic cache. 수학적 보장을 제공.

---

## [2] GPT Semantic Cache: Reducing LLM Costs and Latency via Semantic Embedding Caching
- **arXiv:** 2411.05276
- **저자:** Sajal Regmi, Chetan Phakami Pun (Nepal)
- **핵심 기여:**

  Redis in-memory 저장소에 query embedding 저장. 코사인 유사도 기반 semantic matching.
  
  **실험 결과:**
  - API 호출 **68.8% 감소**
  - 캐시 히트율: **61.6% ~ 68.8%**
  - Positive hit rate: **97% 이상** (캐시 응답의 신뢰성)
  
- **한계:** 정적 임계값 사용 (vCache가 지적한 문제). 단일 임계값으로 모든 쿼리 처리.

---

## [3] ConvoCache: Smart Re-Use of Chatbot Responses
- **arXiv:** 2406.18133
- **컨퍼런스:** Interspeech 2024
- **저자:** Conor Atkins, Ian Wood, Mohamed Ali Kaafar, Hassan Asghar, Nardine Basta, Michal Kepkowski (Macquarie University, Australia)
- **핵심 기여:**

  음성 챗봇을 위한 semantic caching 시스템. UniEval coherence threshold 90% 사용.
  
  **실험 결과:**
  - **89%** 프롬프트를 캐시로 응답 가능
  - 평균 지연시간 **214ms** (LLM 호출 대비 5배 이상 단축)
  - Prefetching: 80% 수준에서 63% 히트율 (한계적 유용성 확인)
  
  **의의:** 음성 인터페이스에서의 캐싱 실증 연구. 실사용 workload 기반 평가.

---

## [4] IC-Cache: Efficient Large Language Model Serving via In-context Caching
- **arXiv:** 2501.12689
- **DOI:** 10.1145/3731569.3764829 (ACM)
- **저자:** Yifan Yu, Yu Gan, Nikhil Sarda, Lillian Tsai, Jiaming Shen, Yanqi Zhou, Arvind Krishnamurthy, Fan Lai, Henry M. Levy, David Culler (University of Washington, Google, Microsoft)
- **핵심 기여:**

  단순 응답 캐싱이 아닌, **과거 응답을 in-context example로 재사용**하는 독특한 접근.
  - 70% 이상의 사용자 요청이 의미상 유사한 요청과 대응됨 (실제 workload 분석)
  
  **Cost-aware cache replay mechanism:**
  - 캐시된 example의 품질을 오프라인에서 정제(offline refinement)
  - 온라인 cache utility와 efficiency를 최대화
  
  **결과:**
  - LLM 서빙 throughput: **1.4~5.9배 향상**
  - 지연시간: **28~71% 감소**
  - 응답 품질 저하 없음

  **수학적 요소:** utility function 기반 cache admission/eviction.
  각 cache entry의 utility = (품질 향상) / (저장 비용 + 검색 비용)

---

## [5] LLMBridge: Reducing Costs to Access LLMs in a Prompt-Centric Internet
- **arXiv:** 2410.11857
- **저자:** Noah Martin, Abdullah Bin Faisal, Hiba Eltigani, Rukhshan Haroon, Swaminathan Lamelas, Fahad Dogar (Tufts University, USA)
- **핵심 기여:**

  LLM Proxy 아키텍처. 세 가지 최적화 전략:
  1. **Model Selection:** 프롬프트에 가장 적합한 모델로 라우팅
  2. **Context Management:** 컨텍스트 지능적 축소
  3. **Semantic Caching:** 로컬 모델 + vector DB로 프롬프트 서빙
  
  **실사용 검증:**
  - WhatsApp Q&A 서비스: **14.7K 요청, 100+ 사용자, 12개월 운영**
  - 대학교 강의실 환경: **500 requests/일**

  **트레이드오프 프레임워크:**
  비용-품질 간 trade-off를 high-level bidirectional interface로 표현.
  ```

---

## [6] VaryGen: LLMs for Test Input Generation for Semantic Caches
- **arXiv:** 2401.08138
- **컨퍼런스:** CAIN 2024 (International Conference on AI Engineering)
- **저자:** Zafaryab Rasool, Scott Barnett, David Willie, Stefanus Kurniawan, Sherwin Balugo, Srikanth Thudumu, Mohamed Abdelrazek (Deakin University, Australia)
- **핵심 기여:**

  Semantic cache의 **테스트 데이터 생성** 방법론.
  LLM의 reasoning 능력을 활용해 유사 질문-응답 쌍을 생성.
  
  **의의:** Semantic cache의 정확도 검증 프레임워크를 제공.

---

## [7] MeTMaP: Metamorphic Testing for Detecting False Vector Matching in LLM Augmented Generation
- **arXiv:** 2402.14480
- **저자:** Guanyu Wang, Yuekang Li, Yi Liu, Gelei Deng, Tianlin Li, Guosheng Xu, Yang Liu, Haoyu Wang, Kailong Wang (Nanyang Technological University, Singapore; Huazhong Univ. of Science and Technology, China)
- **핵심 기여:**

  Semantic cache의 핵심인 vector matching의 정확도 실증 분석.
  203개 벡터 매칭 설정(29개 임베딩 모델 × 7개 거리 메트릭) 평가.
  
  **결과:** 최고 정확도 **41.51%**에 불과 — false match 문제가 심각함을 입증.
  
  **의의:** Semantic cache 도입 시 false positive/false negative 리스크를 반드시 고려해야 함.

---

## 분석: 우리 프로젝트에의 적용

### 수학적 프레임워크 (vCache 기반)

```
비용 함수: Cost = λ₁ × tokens_saved + λ₂ × latency_reduced - λ₃ × error_rate

캐시 도입 결정 조건:
  E[saved_cost_per_request] × P(hit) > cache_operation_overhead
  
  where:
    saved_cost_per_request = token_cost(input + output)
    P(hit) = f(similarity_threshold, embedding_quality)
    cache_operation_overhead = embedding_cost + lookup_cost + memory_cost
```

### 결정 트리 (모델 가격 기반)

```
if token_cost < $0.5/M input tokens:
    → 캐시 불필요 (DeepSeek V4 = $0.14/M)
    → dedup만 (20줄)
    
if token_cost $0.5~$5/M:
    → 경량 semantic cache (선택적)
    → LRU + 유사도 임계값 0.92
    
if token_cost > $5/M:
    → vCache 스타일 동적 임계값
    → 필수 도입
```

### 결론

- **현재 (DeepSeek V4, $0.14/M):** 학술적 근거상 캐시 도입 불필요
- **미래 (더 비싼 모델로 전환 시):** vCache의 온라인 학습 접근법을 C로 재구현할 가치 있음
- **리스크:** MeTMaP 결과(41.51% 정확도)에서 보듯, semantic cache의 false match 위험 존재
- **중요한 인사이트:** vCache의 "각 캐시 항목별 개별 임계값" 개념은 캐시 메모리 오버헤드(8KB 바이너리, 100KB 메모리)와 트레이드오프가 있으나, 모델 비용이 높아질수록 필수적
