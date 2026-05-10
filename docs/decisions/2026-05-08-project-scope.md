---
date: 2026-05-08
type: decision
tags: [architecture, scope]
status: active
---

# Alchemy 프로젝트 개요

## 맥락
Nous Research의 Hermes Agent(Python 기반)를 C 언어로 재구현.
Python의 추상화 오버헤드를 제거하고 바이너리 크기와 성능을 극한으로 최적화.

## 결정
- 언어: **C** (Python 대체)
- 외부 의존성: **libc + OpenSSL**만 허용
- TCP socket, HTTP 파서, JSON 파서: 전부 직접 구현
- 모든 모듈: 구현 → 성능측정 → 기록 3단계 필수

## 결과
바이너리 크기 수백KB~수MB 목표, 의존성 거의 제로.
구현 시간은 증가하지만 학습과 최적화에 유리.
