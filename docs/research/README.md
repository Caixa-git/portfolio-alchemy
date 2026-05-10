# 연구 자료 모음

| 문서 | 내용 |
|------|------|
| [01-lightweight-tcpip.md](01-lightweight-tcpip.md) | 초경량 TCP/IP 스택 설계 (uIP, lwIP) |
| [02-binary-minimization.md](02-binary-minimization.md) | C 바이너리 최소화 기법 |
| [03-memory-optimization.md](03-memory-optimization.md) | 메모리 최적화 전략 (pool, zero-copy) |
| [04-extreme-optimization-cases.md](04-extreme-optimization-cases.md) | 극한 최적화 창의적 사례 |
| [05-decision-recording-methods.md](05-decision-recording-methods.md) | 개발 결정/기록 방법론 (ADR, Decision Log, Docs as Code 등) |

## 핵심 교훈
1. **하나의 버퍼, 하나의 상태 머신** — 단순함이 최적화의 시작
2. **정적 할당 우선** — malloc은 메모리 풀로 대체
3. **함수 1개로 N가지 일** — 중복 로직 제거 = 용량 감소
4. **생각의 전환** — 데이터를 저장하지 말고 실시간 생성하라
