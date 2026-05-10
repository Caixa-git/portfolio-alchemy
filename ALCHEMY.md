# Alchemy — Project Context

> Level 2: Project Context (시스템 프롬프트 Layer 2)
> 자세한 내용: `docs/roadmap.md`

## Architecture

- Language: **C99** (libc + OpenSSL 3.0+)
- Pattern: Reactor + Microkernel
- Layer: Gateway → Agent → LLM + Tool
- Philosophy: **Library-first, Python zero** — 모든 기술은 `library/`에 C99로 직접 구현
- Binary: **136KB** (release) / 271KB (debug) — 19,221 LOC, 32 modules

## Directory Structure

```
alchemy/
├── library/           # 16 pure-C99 라이브러리
│   ├── json/          # DOM JSON 파서
│   ├── ds/            # 동적 배열, 문자열, 세션
│   ├── llm/           # 멀티 프로바이더 라우터 + MCP
│   ├── algo/          # 서브에이전트, 재시도, 슬래시, 플러그인, TUI, 테스트
│   └── browser/       # 헤드리스 브라우저 제어
├── src/
│   ├── core/          # Alchemy 코어 — 16개 모듈
│   ├── cli/           # REPL 엔트리포인트
│   └── gateway/       # Discord 게이트웨이 (WebSocket)
├── skills/            # 사용자 정의 스킬 파일 (.skill)
├── docs/
│   ├── decisions/     # 아키텍처 결정 기록 (ADR)
│   ├── design/        # 디자인 리뷰
│   └── research/      # 연구 참고자료
└── build/             # 빌드 출력

Source: 46 C + 39 H = 19,221 LOC
Binary: 136KB stripped / 271KB debug (~5s build)
```

## Key Commands

| 명령어 | 설명 |
|--------|------|
| `/help` | 전체 명령어 목록 |
| `/status` | 에이전트 상태 (스킬, 도구, 모델) |
| `/model` | 모델/프로바이더 목록 |
| `/rpgstatus` | RPG 스타일 HTML 상태 시트 생성 |
| `/skill` | 스킬 관리 (생성, 병합, 추천, 카테고리 변경) |
| `/dashboard` | HTML 대시보드 생성 |
| `/debug` | 디버그 모드 전환 |
| `/new` | 세션 초기화 |
| `/exit` | 종료 |

## Skill System

스킬은 `.skill` 파일로 관리되며 다음 상태를 가집니다:

- **Level** (0.0 ~ 10.0) — Dreyfus 모델 기반
- **XP** — 사용할수록 자동 증가
- **Evolution** — Novice → Advanced Beginner → Competent → Proficient → Expert
- **Level-up notification** — 레벨업 시 ANSI 박스 + 스탯 델타 표시
- **Merge** — 유사한 스킬 병합 가능
- **Auto-suggest** — 유사도 기반 병합 추천

## Dependencies

- libc (glibc/musl)
- OpenSSL 3.0+ (TLS + crypto)
- pthread
- libdl (plugin loader)
- **Zero Python, Zero Node.js, Zero Ruby**

## Build

```bash
make -j$(nproc)         # debug build (271KB)
make release -j$(nproc) # release build (136KB, stripped)
make clean              # clean all artifacts
```

## See Also

- [`README.md`](README.md) — full documentation
- [`docs/roadmap.md`](docs/roadmap.md) — development roadmap
- [`docs/decisions/`](docs/decisions/) — Architecture Decision Records
