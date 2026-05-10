# AI Agent 시스템 프롬프트/스킬/유저 정보 층위 분석

## 1. 현재 alchemy 상태

| 항목 | 현재 상태 | 문제점 |
|------|----------|--------|
| **시스템 프롬프트** | `prompt.txt` — 1줄 "You are Alchemy..." | 너무 얕음, 계층 없음 |
| **스킬** | `skills/*.skill` — 3개의 단순 텍스트 파일 | 포맷 통일 안 됨, 트리거 조건 없음 |
| **유저 정보** | 없음 | 사용자 식별/맞춤 불가 |
| **메모리** | `alchemy.mem` — 바이너리 | 구조화 안 됨, 검색 불가 |

## 2. 3대 플랫폼 비교 분석

### Claude Code — 가장 정교한 계층 구조

| 층위 | 위치 | 내용 | 특징 |
|------|------|------|------|
| **Identity** | `~/.claude/CLAUDE.md` | 글로벌 ID, 항상 적용 | 최우선 로드 |
| **Project Context** | `./CLAUDE.md` | 프로젝트별 아키텍처/규칙 | git-tracked |
| **Rules (모듈형)** | `.claude/rules/*.md` | 개별 규칙 파일 (20개까지 가능) | **핵심 강점** |
| **Local Override** | `.claude/CLAUDE.local.md` | 개인 오버라이드 | gitignored |
| **Skills (자동 발동)** | `.claude/skills/*.md` | 자연어 매칭 시 자동 실행 | Agent가 스스로 판단 |
| **Subagents** | `.claude/agents/*.md` | 전담 role 정의 | `@agent-name` 호출 |
| **Auto Memory** | `~/.claude/projects/*/memory/` | 대화에서 자동 학습 | 25KB 제한 |
| **Custom Commands** | `.claude/commands/*.md` | `/deploy` 같은 슬래시 명령 | 수동 호출 |
| **Settings** | `.claude/settings.json` | 권한/훅/MCP 설정 | 허용/차단 리스트 |

**강점:** 
- 규칙 디렉토리로 **모듈화**가 가장 잘 됨
- 스킬이 **자연어 매칭으로 자동 발동** (트리거 기반)
- 훅 시스템으로 **Pre/Post 이벤트 처리**
- `/compact`로 컨텍스트 관리 내장

**단점:**
- CLAUDE.md 하나에 다 때려넣으면 오히려 독
- 규칙 간 우선순위 충돌 모호함

### Codex — 가장 단순

| 층위 | 위치 | 내용 | 특징 |
|------|------|------|------|
| **Project Context** | `./AGENTS.md` | 프로젝트 설명 | 유일한 설정 파일 |
| **Auth** | `~/.codex/auth.json` | OAuth 인증 | CLI 전용 |

**강점:** 심플함, 러닝커브 낮음
**단점:** 층위 분화 거의 없음, 스킬/메모리 개념 없음

### Hermes Agent — 가장 체계적인 내부 구조

| 층위 | 파일 | 내용 | 특징 |
|------|------|------|------|
| **Identity (정체성)** | `SOUL.md` | AI의 성격/말투/원칙 | **alchemy에 가장 적합** |
| **User Profile** | `USER.md` (memory target=user) | 사용자 정보/선호도 | 별도 관리 |
| **Agent Notes** | `MEMORY.md` (memory target=memory) | 환경/규칙/교훈 | 영속 메모리 |
| **Skills (절차서)** | `skills/*/SKILL.md` | 도구 사용법 + 트리거 조건 | YAML frontmatter |
| **Procedural Memory** | `kanban.db`, session_search | 과거 세션 검색 | 자동 검색 |
| **Pipeline** | `alchemy` (pipeline SKILL.md) | Socrates→Router→Pipeline→Library | 추론 흐름 제어 |

**강점:**
- **SOUL.md**가 정체성/말투/원칙을 명확히 분리
- USER/MEMORY 분리로 사용자 정보와 에이전트 노트 분리
- 스킬에 YAML frontmatter로 트리거/의존성/버전 명시
- 파이프라인 구조로 여러 스킬 조합 가능

**단점:**
- 스킬 간 우선순위가 모호할 수 있음
- 대규모 프로젝트에서 스킬 수가 많아지면 관리 복잡

## 3. 최적 층위 구조 제안 (alchemy + 3개 플랫폼 장점 결합)

### 제안: 6계층 아키텍처

```
Level 0: Identity           — 절대 변하지 않는 정체성 (LOCKED)
Level 1: User Profile       — 사용자 정보/선호도 (가변)
Level 2: Project Context    — 프로젝트 구조/규칙
Level 3: Skills (모듈)      — 작업별 절차서 (트리거 조건 포함)
Level 4: Session Memory     — 현재 대화 맥락
Level 5: Persistent Memory  — 영속적 학습/교훈
```

### 각 계층 상세 규칙

#### Level 0: Identity (LOCKED)
```
위치: system_prompt/identity.h (C 코드 내장)
포맷: C 문자열 상수, 컴파일 타임 고정
특징: 
  - XOR 암호화 (Rocky 방식 차용)
  - 실행 중 절대 변경 불가
  - AI의 성격/말투/핵심 원칙 포함
  - "You are Alchemy ..." 형태

규칙:
  - 절대 하드코딩 (런타임 로딩 금지)
  - 바이너리 내 암호화 저장
  - 변경 시 재컴파일 필요
  - persona_decrypt()로만 접근 가능
```

#### Level 1: User Profile
```
위치: ~/.alchemy/users/<user_id>.json
포맷: JSON (가독 + 기계 파싱)
필드:
  - name, role, preferences
  - language (ko/en 기본)
  - work_style (fast_iteration / heads_down)
  - communication_style (banmal / jondae)
  - pet_peeves (거짓과장 금지 등)
  - skill_preferences (선호 도구/패턴)

규칙:
  - Level 0보다 우선순위 낮음 (오버라이드 불가)
  - JSON 스키마 검증 필수
  - 사용자 명시적 허가 후 저장
  - gitignored (개인정보)
```

#### Level 2: Project Context
```
위치: ALCHEMY.md (프로젝트 루트)
포맷: Markdown + YAML frontmatter
필드:
  - architecture: 프로젝트 구조 개요
  - conventions: 코딩 규칙 (네이밍, 포맷팅)
  - key_commands: 빌드/테스트/배포 명령어
  - dependencies: 외부 의존성
  - design_decisions: 주요 설계 결정과 이유
  - decision_log: docs/decisions/ 의사결정 기록 링크

규칙:
  - git-tracked (팀 공유)
  - 50줄 이내 유지
  - 상세 내용은 docs/ 참조
  - 변경 시 커밋 메시지에 반영
```

#### Level 3: Skills (모듈형 절차서)
```
위치: .alchemy/skills/<name>.md
포맷: YAML frontmatter + Markdown
예시:
  ---
  name: build
  trigger: "빌드|컴파일|make|build error"
  priority: 10 (높을수록 우선)
  depends: [env]
  ---
  # Build Skill
  ## Steps
  1. `make clean && make`
  2. 에러 발생 시 로그 확인
  3. 수정 후 재빌드

규칙:
  - Claude Code의 .claude/rules/ 차용
  - 트리거 조건으로 자연어 매칭 (자동 발동)
  - priority 필드로 충돌 해결
  - 각 스킬은 단일 책임 (SRP)
  - 20줄 이내로 간결 유지
  - C 코드의 alchemy_skill.c와 연동
```

#### Level 4: Session Memory
```
위치: alchemy_context.c (ring buffer)
포맷: C 구조체 배열
특징:
  - 현재 대화 메시지 링 버퍼
  - 최근 N개 메시지 유지
  - context_window.c와 연동

규칙:
  - 윈도우 크기: 20KB 제한
  - 50% 초과 시 자동 truncation
  - 중요 메시지는 Level 5로 승격
```

#### Level 5: Persistent Memory
```
위치: alchemy_memory.c + 파일 저장
포맷: JSON Lines (한 줄 = 하나의 사실)
필드:
  - discovered_facts: 학습한 정보
  - user_preferences: 사용자 선호도
  - project_learnings: 프로젝트 교훈
  - error_patterns: 반복 오류 해결법

규칙:
  - Hermes Agent의 memory target=memory 차용
  - Key-Value 저장 (검색 가능)
  - 중복 저장 금지 (덮어쓰기)
  - 용량: 100KB 제한
  - 주기적 compaction
```

## 4. 시스템 프롬프트 조립 순서

최종 시스템 프롬프트는 다음 순서로 조립:

```
[Level 0: Identity]           ← C 코드 내장, LOCKED
    ↓ (이어붙이기)
[Level 1: User Profile]       ← JSON 로드, 가변
    ↓ (이어붙이기)
[Level 2: Project Context]    ← ALCHEMY.md 로드
    ↓ (트리거 매칭)
[Level 3: Skills]             ← 자연어 매칭된 스킬만 포함
    ↓ (주입)
[Level 4: Session Memory]     ← ring buffer에서 추출
    ↓ (주입)
[Level 5: Persistent Memory]  ← KV 저장소에서 검색
```

## 5. 비교표: 3개 플랫폼 vs 제안안

| 기능 | Claude Code | Codex | Hermes Agent | **제안 (alchemy)** |
|------|:-----------:|:-----:|:------------:|:-------------------:|
| Identity (LOCKED) | ❌ | ❌ | ✅ (SOUL.md) | **✅ (C 암호화 내장)** |
| User Profile | ❌ | ❌ | ✅ (USER.md) | **✅ (JSON 분리)** |
| Project Context | ✅ (CLAUDE.md) | ✅ (AGENTS.md) | ❌ | **✅ (ALCHEMY.md)** |
| 모듈형 Rules | ✅ (.claude/rules/) | ❌ | ❌ | **✅ (.alchemy/skills/)** |
| 자동 트리거 스킬 | ✅ (.claude/skills/) | ❌ | ✅ (SKILL.md) | **✅ (YAML 트리거)** |
| Subagents | ✅ (.claude/agents/) | ❌ | ✅ (kanban) | **⏳ (C 함수 포인터)** |
| Session Memory | ✅ (자동) | ❌ | ✅ (ring buffer) | **✅ (alchemy_context)** |
| Persistent Memory | ✅ (자동 학습) | ❌ | ✅ (memory tool) | **✅ (alchemy_memory)** |
| Hooks (Pre/Post) | ✅ (8종) | ❌ | ❌ | **⏳ (C 함수 훅)** |
| 컨텍스트 컴팩션 | ✅ (/compact) | ❌ | ❌ | **⏳ (자동 truncation)** |

✅ = 있음 / ❌ = 없음 / ⏳ = 개발 필요

## 6. alchemy 적용 로드맵

### Phase 1 (즉시)
1. `system_prompt/identity.h` 생성 — LOCKED identity
2. `ALCHEMY.md` 생성 — project context (Rocky 방식 차용)
3. `.alchemy/skills/` 디렉토리 + 포맷 통일

### Phase 2 (단기)
4. `users/` 디렉토리 + JSON 스키마
5. Context window 컴팩션 로직
6. Persistent memory compaction

### Phase 3 (중기)
7. 자연어 트리거 매칭 엔진
8. Pre/Post 훅 시스템
9. 암호화된 identity 저장 (Rocky의 rocky_persona_enc.h 방식)
