---
date: 2026-05-08
type: process
tags: [documentation, workflow]
status: active
---

# H-Log: 결정 및 기록 프로세스

## 맥락
Discord 채팅으로 모든 결정이 이루어짐. 모든 대화가 곧 결정이자 행동.
기록은 GitHub에 남겨 포트폴리오로 활용.

## 프로세스

### 기록 포맷
모든 결정/기록은 `docs/decisions/` 아래 Markdown 파일.
YAML frontmatter로 메타데이터 관리, 바디는 경량 ADR 형식.

### 파일명 규칙
```
YYYY-MM-DD-{category}-{slug}.md
```
예: `2026-05-08-decision-llm-providers.md`

### frontmatter 필드
| 필드 | 필수 | 설명 |
|------|:----:|------|
| date | ✅ | 결정일자 |
| type | ✅ | decision / benchmark / implementation / process / research |
| tags | | 카테고리 태그 |
| status | ✅ | active / superseded |
| discord | | Discord 메시지 링크 (있으면) |

### 자동화 (AI 담당)
1. Discord에서 결정 감지 → 즉시 md 파일 생성
2. `docs/decisions/README.md` 인덱스 업데이트
3. git add → commit (Conventional Commit) → push

### 인덱스 구조
`docs/decisions/README.md`에 최신 결정 10개를 역순으로 표시.
전체 목록은 파일 시스템 탐색으로 대체.

### 스크린샷 정책
- 실행 결과는 **시각적 기록 우선**
- `docs/screenshots/` 디렉토리에 PNG 저장
- benchmark 문서에서 `![설명](../screenshots/파일명.png)` 형식으로 참조
- browser_vision으로 캡처 가능하면 스크린샷 + 텍스트 로그 함께 기록
- 터미널 출력은 `docs/screenshots/*-output.txt`에 텍스트로도 저장

## 결과
진수님은 아무것도 안 해도 됨. 기록은 자동으로 GitHub에 남음.
모든 결정 이력이 시각적/텍스트로 체계적으로 관리되어 포트폴리오 가치 상승.
