# 개발 결정/기록 방법론 연구

> 목표: Discord 채팅 기반 협업에서 결정을 실시간 기록하고 GitHub에 체계적으로 남기는 프로세스 설계
> 작성: 2026-05-08

---

## 1. Architecture Decision Record (ADR)

### 개요
Michael Nygard이 제안한 **ADR (Architecture Decision Record)** 는 중요한 아키텍처 결정을 문서화하는 방법. 각 결정은 하나의 Markdown 파일로, Git으로 버전 관리.

### Nygard 원본 템플릿
```markdown
# ADR-{N}: {결정 제목}

## Status
[Proposed | Accepted | Deprecated | Superseded]

## Context
왜 이 결정이 필요한가? 배경, 제약조건, 고려사항.

## Decision
무엇을 결정했는가? 어떤 선택을 했는가?

## Consequences
이 결정으로 인한 결과: 장점, 단점, 트레이드오프.
```

### ThoughtWorks 권장 템플릿 (변형)
```markdown
# {N}. {결정 제목}

## 날짜
YYYY-MM-DD

## 상태
[제안 | 승인 | 폐기 | 대체됨]

## 결정자
참여한 사람들

## 맥락
문제 설명, 대안들, 평가 기준

## 결정
선택한 옵션과 이유

## 결과
긍정적/부정적 영향, 마이그레이션 경로

## 관련 ADR
관련된 다른 결정 기록들
```

### 최신 ADR 변형들
| 변형 | 특징 |
|------|------|
| **Y-Statements** | "In the context of... facing... we decided for... to achieve... accepting..." 구조화된 한 문장 |
| **Markdown Any Decision (MAD)** | ADR과 비슷하지만 기술적/비기술적 결정 모두 포함 |
| **Event-Storming ADR** | 이벤트 스토밍 결과를 ADR로 변환 |
| **Lightweight ADR** | 상태(title) + 맥락 + 결정 + 영향 — 최소 4섹션만 유지 |

### 장점
- ✅ **명확한 구조**: 누가 봐도 이해 가능한 포맷
- ✅ **Git 기반**: 변경 이력 추적, PR 리뷰 가능
- ✅ **검색 가능**: 결정을 나중에 찾기 쉬움
- ✅ **확장성**: 프로젝트 규모와 관계없이 적용 가능

### 단점
- ❌ **초기 진입 장벽**: 템플릿 작성이 부담스러울 수 있음
- ❌ **최신성 유지 어려움**: 결정을 즉시 기록하지 않으면 누락
- ❌ **과도한 문서화 위험**: 사소한 결정까지 ADR로 남기면 관리 불가능
- ❌ **채팅 환경과의 괴리**: Discord에서 바로 ADR을 작성하기엔 포맷이 무거움

### 실제 사례
- **Kubernetes**: 모든 설계 결정을 KEP(Kubernetes Enhancement Proposal) → ADR 형식으로 관리
- **Azure DevOps**: `docs/decisions/` 디렉토리에 ADR 저장, CI에서 상태 검증
- **ThoughtWorks**: 모든 내부 프로젝트에 ADR 도입, 주간 리뷰

---

## 2. Decision Log / Engineering Diary

### 개요
개발 과정에서 마주친 결정들을 **로그 형식**으로 기록. 일기처럼 날짜별로 정리하거나, 결정별로 태그/카테고리화.

### 접근법

#### (A) Engineering Diary (날짜 기반)
```
## 2026-05-08

### 고민: JSON 파서 선택
- **고려안**: simdjson, cJSON, 직접 구현
- **선택**: 직접 구현 (의존성 제거 목적)
- **이유**: 바이너리 크기 200KB↓, 단순한 사용 패턴
- **결과**: 구현 2일 소요, 예상보다 30% 코드 증가

### 결정: TCP 전송 timeout 30초
- 이유: Discord API timeout 정책 준수
```

#### (B) Decision Log (카테고리 기반)
```
## [JSON] 파서 라이브러리 선택
- 날짜: 2026-05-08
- 결정: 직접 구현
- 이유: 의존성 최소화, 단순한 메시지 포맷
- 대안: simdjson(GPL), cJSON(속도↓)

## [NETWORK] 연결 타임아웃
- 날짜: 2026-05-08
- 결정: 30초
- 이유: Discord API 제한
```

#### (C) 도구화
| 도구 | 설명 |
|------|------|
| **logseq** | 로컬 Markdown 기반 지식 관리, 결정 로그에 최적 |
| **Obsidian** | 그래프 뷰로 결정 간 관계 시각화 |
| **Notion** | 협업에 강하지만 Git 연동 불가 |
| **DIY GitHub Issue 템플릿** | 결정 로그용 이슈 템플릿 생성 |

### 장점
- ✅ **가벼움**: 템플릿 부담 없이 바로 작성 가능
- ✅ **자연스러운 흐름**: 채팅 → 복사/붙여넣기 → 로그
- ✅ **시간순 정리**: 결정의 맥락과 시간적 순서 파악 용이
- ✅ **완성도 부담 없음**: 초안 수준으로 남겨도 OK

### 단점
- ❌ **비구조화**: 나중에 검색/분류하기 어려움
- ❌ **일관성 부족**: 사람마다 작성 스타일이 달라짐
- ❌ **중복 위험**: 같은 결정을 여러 번 논의할 수 있음
- ❌ **Git과의 괴리**: GitHub PR과 결정 로그 연결이 어려움

### 실제 사례
- **Stripe**: 내부적으로 `decisions.log` 파일을 모든 저장소에 유지
- **Basecamp (Shape Up)**: 결정을 "Appetite" 단위로 기록하고 GitHub Issue에 연결
- **개인 오픈소스 프로젝트**: `CHANGELOG.md`에 결정 이유를 함께 작성

---

## 3. Documentation as Code (Docs as Code)

### 개요
문서를 **코드와 동일한 워크플로우**로 관리: Git 버전 관리, PR 리뷰, CI 테스트, 자동 배포.

### 핵심 원칙
1. **Markdown 중심**: 모든 문서를 Markdown으로 작성
2. **Git 저장소**: 코드와 문서를 같은 저장소에 유지 (또는 별도 docs 저장소)
3. **PR 리뷰**: 문서 변경도 코드 변경처럼 리뷰
4. **CI/CD**: 문서 빌드, 링크 검사, 포맷팅 자동화
5. **자동 배포**: docs.theirsite.com 같은 문서 사이트 자동 배포

### 결정 기록에의 적용
```yaml
# docs/decisions/YYYY-MM-DD-deterministic-json.md
---
title: "JSON 파서 결정론적 직렬화"
date: 2026-05-08
status: accepted
deciders: @caixa, @engineer2
type: architecture
---

## Context
...(Markdown 내용)
```

### 도구 체인
| 도구 | 용도 |
|------|------|
| **MkDocs / Docusaurus** | Markdown → 문서 사이트 |
| **Vale / textlint** | 문서 스타일/문법 검사 |
| **lychee** | 링크 데드 체크 |
| **Prettier** | Markdown 포맷팅 |
| **GitHub Actions** | 문서 CI/CD 파이프라인 |

### 장점
- ✅ **코드 품질 도구 재사용**: lint, CI, 리뷰 프로세스 그대로 적용
- ✅ **버전 일치**: 코드와 문서의 버전이 항상 일치
- ✅ **자동화**: 결정 기록이 올바른 포맷인지 CI에서 검증
- ✅ **PR 중심**: "결정" 자체가 PR로 이루어지게 강제

### 단점
- ❌ **설정 복잡도**: Vale, MkDocs, CI 설정에 시간 소요
- ❌ **협업 비기술자 배제**: 문서 작성에 Git/GitHub 이해 필요
- ❌ **실시간성 부족**: PR 프로세스가 결정 기록을 지연시킴

### 실제 사례
- **Google**: 내부 "g3doc" 시스템 — 코드와 문서를 하나의 저장소에서 관리
- **Netflix**: docs.netflix.com — 모든 엔지니어링 문서를 Markdown + Git 관리
- **GitLab**: 자체 Docs as Code 사례, documentation.gitlab.com

---

## 4. Conventional Commits + Changelog

### 개요
커밋 메시지를 구조화하여 결정과 변경사항을 기록하고, 이를 기반으로 `CHANGELOG.md`를 자동 생성.

### Conventional Commits 포맷
```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

| 타입 | 의미 | 결정 기록 용도 |
|------|------|----------------|
| `feat` | 새 기능 | 기능 관련 결정 |
| `fix` | 버그 수정 | 버그 수정 결정 |
| `docs` | 문서 변경 | ADR/결정 로그 추가 |
| `decision` | **(확장)** | 결정 기록 전용 |
| `adr` | **(확장)** | ADR 추가/변경 |

### 결정 기록 예시
```
decision(json): 직접 구현한 파서 채택

simdjson(GPL), cJSON(성능↓) 대신 직접 구현 결정.
의존성 0, 바이너리 200KB 감소 효과.

ADR: ADR-003
Discord: #dev-json 링크
Reviewed-by: @caixa
```

### Changelog 자동 생성
**도구**: `git-cliff`, `standard-version`, `semantic-release`

```toml
# cliff.toml (git-cliff 설정)
[changelog]
header = "# Changelog\n"
body = """
### 결정 사항
{% for group, commits in commits | group_by(attribute="group") %}
#### {{ group }}
{% for commit in commits %}
- {{ commit.message }}
{% endfor %}
{% endfor %}
"""
```

### 장점
- ✅ **개발 워크플로우에 내장**: 별도 문서화 없이 커밋만으로 기록
- ✅ **자동 생성**: Changelog가 자동으로 업데이트
- ✅ **Git과 일체화**: 결정과 변경사항이 함께 추적됨
- ✅ **Discord 연동 용이**: GitHub → Discord webhook으로 커밋 자동 공유

### 단점
- ❌ **커밋 메시지 길이 제한**: 자세한 결정 맥락 기록 어려움
- ❌ **Git에 익숙해야 함**: 비개발자(PM, 디자이너) 사용 어려움
- ❌ **Force push 위험**: 기록이 사라질 수 있음
- ❌ **결정의 "왜"보다 "무엇"에 집중**: 배경 설명 부족

### 실제 사례
- **Angular**: Conventional Commits 원조, 모든 커밋이 이 포맷
- **ESLint**: `docs:` 타입으로 결정 기록 커밋 별도 관리
- **Kubernetes**: 커밋 메시지에 `SIG-XXX` 레이블로 결정 소속 표시

---

## 5. YAML Frontmatter + Markdown

### 개요
Markdown 파일 상단에 **YAML frontmatter**를 추가하여 기계가 읽기 쉬운 메타데이터와 사람이 읽기 쉬운 본문을 결합.

### 표준 포맷
```markdown
---
title: "TCP 연결 타임아웃 30초 설정"
id: DEC-042
date: 2026-05-08
status: accepted
type: configuration
tags: [network, timeout, discord-api]
deciders: [@caixa, @senior-dev]
discord: "https://discord.com/channels/.../..."  # ← 채팅 기록 링크
github: "#42"  # ← PR/Issue 링크
supersedes: DEC-015
---

## Context
...(일반 Markdown 내용)

## Decision
...(일반 Markdown 내용)

## Consequences
...(일반 Markdown 내용)
```

### 스키마 정의 (JSON Schema)
```json
{
  "$schema": "https://json-schema.org/draft-07/schema#",
  "type": "object",
  "required": ["title", "date", "status", "type"],
  "properties": {
    "title": { "type": "string" },
    "date": { "type": "string", "format": "date" },
    "status": {
      "type": "string",
      "enum": ["proposed", "accepted", "rejected", "deprecated", "superseded"]
    },
    "type": {
      "type": "string",
      "enum": ["architecture", "configuration", "process", "tooling", "api", "other"]
    },
    "tags": { "type": "array", "items": { "type": "string" } },
    "discord": { "type": "string", "format": "uri" },
    "github": { "type": "string" }
  }
}
```

### 활용 시나리오
| 도구 | 활용 |
|------|------|
| **GitHub Actions** | PR 시 frontmatter 검증 (누락 필드 체크) |
| **11ty / Hugo** | frontmatter로 문서 사이트 자동 생성 |
| **Obsidian** | 태그/필터로 결정 탐색 |
| **logseq** | frontmatter를 속성으로 변환 |
| **Custom 스크립트** | `find . -name "*.md" | xargs yq` 로 전체 결정 분석 |

### 장점
- ✅ **기계 가독성 + 인간 가독성** 동시 만족
- ✅ **검색/필터링/분석 가능**: "status=accepted"인 결정만 조회 등
- ✅ **GitHub Actions로 유효성 검증**: 잘못된 결정 기록 방지
- ✅ **확장 용이**: 필요한 메타데이터는 frontmatter에 추가
- ✅ **Discord 링크 포함**: 채팅 기록과 결정 기록 1:1 연결

### 단점
- ❌ **YAML 문법 실수 위험**: 들여쓰기 오류로 파일이 망가짐
- ❌ **도구 지원 필요**: frontmatter 검증 CI 설정 필요
- ❌ **Markdown 렌더러 호환성**: 일부 뷰어에서 frontmatter가 보임
- ❌ **초기 템플릿 설정 필요**: 일관된 작성을 위한 템플릿 필수

### 실제 사례
- **Hugo/11ty 정적 사이트**: 모든 문서의 frontmatter 기반
- **ThoughtWorks ADR 저장소**: GitHub Pages + Jekyll + YAML frontmatter
- **Logseq/Zettelkasten**: 모든 노트가 frontmatter로 메타데이터 관리

---

## 6. 경량 프로세스 (Lightweight Process)

### 개요
"번거롭지 않으면서도 기록이 남는" 실용적인 접근법. 완벽한 문서화보다 **지속 가능한 습관**에 초점.

### 접근법 A: 3-line Rule
Discord 결정 메시지를 3줄로 요약 → GitHub에 기록

```
#discord → docs/decisions/raw/
매 결정을 3줄로 요약:

1. 문제: Discord API timeout 문제
2. 결정: 30초로 설정
3. 이유: API 문서 기준
```

### 접근법 B: Decision.js — Bot 자동화
Discord bot이 `!decision` 명령어로 결정을 수집하여 자동 PR 생성

```
!decision
> **문제**: JSON 파서 라이브러리 선택
> **결정**: 직접 구현
> **이유**: GPL 라이선스 회피, 의존성 0
> **참여**: @caixa @dev2
```

→ Bot이 GitHub Issue or PR 생성 + `docs/decisions/`에 파일 추가

### 접근법 C: 이슈 템플릿 → PR
결정이 필요한 순간 GitHub Issue를 생성 (템플릿 사용)
→ 논의는 Issue 댓글
→ 결정되면 PR로 ADR/결정 로그 생성

```
Issue Template: "decisions.yml"
---
name: 결정 기록
about: 결정이 필요한 사항을 기록합니다
title: "[결정] "
---
### ❓ 결정할 내용
### 📎 참고 자료
### 👥 결정자
### ⏰ 데드라인
```

### 접근법 D: Post-Channel GitHub Sync
Discord 전용 채널(`#decisions`)의 모든 메시지를 정기적으로 GitHub에 동기화

```python
# 간단한 sync 스크립트 컨셉
# Discord #decisions 채널 → docs/decisions/YYYY-MM-DD-title.md 자동 변환
```

### 장점
- ✅ **실용성 극대화**: 기록에 드는 비용이 거의 0에 가까움
- ✅ **지속 가능**: 복잡한 프로세스보다 오래 감
- ✅ **Discord 네이티브**: 채팅 흐름을 방해하지 않음
- ✅ **점진적 개선**: 필요에 따라 점점 정교하게 발전 가능

### 단점
- ❌ **비정형 데이터**: 나중에 검색/분석 어려움
- ❌ **중복/누락**: 같은 결정이 여러 번 기록되거나 누락됨
- ❌ **봇 의존성**: 자동화 안 하면 사람이 직접 해야 함

### 실제 사례
- **Basecamp**: "Decisions" 전용 채널, 중요한 결정만 기록
- **Google (내부)**: 일부 팀은 "Design Doc Lite" — 1페이지 분량 결정 문서
- **GitHub 내부**: `!decision` 슬래시 커맨드로 Slack → GitHub Issue 연동

---

## 7. 최종 권장: Discord → GitHub 결정 기록 파이프라인

> 우리 환경(Discord 채팅 기반 협업)에 가장 적합한 조합

### 🏆 추천 조합
```
YAML Frontmatter + Markdown (저장 포맷)
    + 경량 Discord Bot (!decision 명령어) (수집)
    + GitHub Actions (검증 + 인덱싱) (자동화)
    + Conventional Commits (PR merge 시) (추적)
```

### 단계별 프로세스

#### Phase 1: Discord에서 결정 (실시간)
```
채널: #decisions

@user: JSON 파서 simdjson 쓸까요?
@caixa: GPL이라 꺼려짐. 직접 구현이 낫겠다
@user: 동의. 의존성 제거가 목표니까.

!decision
문제: JSON 파서 선택
결정: 직접 구현 (simdjson 대신)
이유: GPL 라이선스 회피, 의존성 0 목표 유지
태그: #json #parser #architecture
참여자: @caixa @user
```

#### Phase 2: Bot이 GitHub에 기록
Discord bot이 `!decision` 메시지를 감지 →:

1. `docs/decisions/raw/2026-05-08-json-parser.md` 생성
2. PR 생성 (또는 직접 push)

```markdown
---
title: "JSON 파서 직접 구현"
date: 2026-05-08
status: accepted
type: architecture
tags: [json, parser, dependency]
deciders: [@caixa, @user]
discord: "https://discord.com/channels/..."

discord-snippet: |
  @caixa: GPL이라 꺼려짐. 직접 구현이 낫겠다
  @user: 동의. 의존성 제거가 목표니까.
---

## Context
프로젝트 의존성 최소화 목표. simdjson은 GPL 라이선스.
cJSON은 속도가 느리고 메모리 관리가 복잡.

## Decision
직접 구현. 단순한 JSON 메시지 포맷이므로 유지보수 부담 적음.

## Consequences
- 장점: 라이선스 문제 없음, 바이너리 크기 감소
- 단점: 구현 시간 2~3일 소요
```

#### Phase 3: GitHub Actions 자동 검증
```yaml
# .github/workflows/validate-decisions.yml
name: Validate Decisions
on:
  pull_request:
    paths: ['docs/decisions/**']

jobs:
  validate:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Validate YAML frontmatter
        run: |
          for f in docs/decisions/raw/*.md; do
            python -c "
import yaml, sys
with open('$f') as fh:
    # Extract YAML frontmatter
    lines = fh.read().split('---')
    if len(lines) < 3:
        print(f'❌ Missing frontmatter in $f')
        sys.exit(1)
    data = yaml.safe_load(lines[1])
    required = ['title', 'date', 'status', 'type']
    for field in required:
        if field not in data:
            print(f'❌ Missing required field: {field} in $f')
            sys.exit(1)
    valid_statuses = ['proposed','accepted','rejected','deprecated','superseded']
    if data['status'] not in valid_statuses:
        print(f'❌ Invalid status in $f')
        sys.exit(1)
    print(f'✅ Valid: $f')
"
          done
```

#### Phase 4: PR Merge → Conventional Commit
```bash
# merge 시 커밋 메시지
decision(json): JSON 파서 직접 구현 결정

ADR: ADR-003
Discord: #decisions at 2026-05-08 14:32
```

#### Phase 5: 주간 인덱싱
주 1회 GitHub Action이 `docs/decisions/raw/`의 모든 결정을 모아 인덱스 생성:

```markdown
# 결정 인덱스 (자동 생성)

| 날짜 | 결정 | 유형 | 상태 | Discord |
|------|------|------|------|---------|
| 2026-05-08 | JSON 파서 직접 구현 | architecture | ✅ accepted | [링크](#) |
| 2026-05-07 | TCP timeout 30초 | configuration | ✅ accepted | [링크](#) |
```

### 요구사항 체크리스트

| 항목 | 도구/방법 |
|------|----------|
| Discord 명령어 | Discord Bot (Python: `discord.py` + `!decision` 슬래시 커맨드) |
| GitHub 기록 | `docs/decisions/raw/YYYY-MM-DD-slug.md` |
| 검증 | `GitHub Actions` + YAML frontmatter JSON Schema |
| 채팅→GitHub 연결 | Frontmatter의 `discord:` 필드 (메시지 링크) |
| 자동 changelog | `git-cliff` (Conventional Commits 파싱) |
| 인덱스/검색 | 정기 GitHub Action으로 `DECISION_INDEX.md` 생성 |
| PR 템플릿 | `docs/decisions/` 변경 시 자동 템플릿 |

### 마이그레이션 전략

1. **1주차**: Discord에 `#decisions` 채널 생성, 수동 기록 시작 (Phase 1만)
2. **2주차**: Discord Bot 개발/도입 (`!decision` 명령어)
3. **3주차**: GitHub Actions 검증 + CI 설정
4. **4주차**: Conventional Commits + 자동 인덱싱 완료

---

## 부록: 각 방법론 비교표

| 방법론 | 기록 비용 | 검색 가능성 | 자동화 | 채팅 친화성 | Git 친화성 | 지속성 |
|--------|----------|------------|--------|------------|-----------|--------|
| ADR (Nygard) | 중간 | 높음 | 중간 | 낮음 | 높음 | 높음 |
| Decision Log | 낮음 | 중간 | 낮음 | 높음 | 중간 | 중간 |
| Docs as Code | 높음 | 높음 | 높음 | 낮음 | 매우 높음 | 높음 |
| Conventional Commits | 낮음 | 낮음 | 높음 | 중간 | 높음 | 중간 |
| YAML Frontmatter | 중간 | 매우 높음 | 높음 | 중간 | 높음 | 높음 |
| **경량 프로세스 (권장)** | **매우 낮음** | 중간 | 중간 | **매우 높음** | 높음 | 중간 |
| **권장 조합** | **낮음** | **높음** | **높음** | **높음** | **높음** | **높음** |

## 부록: 참고 자료

- Michael Nygard, "Documenting Architecture Decisions" (2011) — https://cognitect.com/blog/2011/11/15/documenting-architecture-decisions
- ThoughtWorks ADR — https://www.thoughtworks.com/radar/techniques/lightweight-architecture-decision-records
- Y-Statements — https://www.ozimmer.ch/practices/2020/04/22/YSStatement.html
- Markdown Any Decision (MADR) — https://adr.github.io/madr/
- Conventional Commits — https://www.conventionalcommits.org/
- Docs as Code — https://www.writethedocs.org/guide/docs-as-code/
- git-cliff (Changelog Generator) — https://github.com/orhun/git-cliff
