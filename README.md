![Alchemy Hero](docs/screenshots/alchemy-hero.png)

<div align="center">

> C99 AI Agent Framework — Pure C, zero Python, 136KB.

<div style="display:flex; justify-content:space-between; width:100%; margin:8px 0;">
  <a href="https://github.com/Caixa-git/alchemy"><img src="https://img.shields.io/badge/build-passing-22c55e?style=flat-square&labelColor=1a1a2e" alt="Build"></a>
  <a><img src="https://img.shields.io/badge/lang-C99-00599C?style=flat-square&labelColor=1a1a2e" alt="Lang"></a>
  <a><img src="https://img.shields.io/badge/size-136KB-8b5cf6?style=flat-square&labelColor=1a1a2e" alt="Size"></a>
  <a><img src="https://img.shields.io/badge/LOC-19K-f59e0b?style=flat-square&labelColor=1a1a2e" alt="LOC"></a>
  <a><img src="https://img.shields.io/badge/modules-32-ec4899?style=flat-square&labelColor=1a1a2e" alt="Modules"></a>
</div>

<p>
  <img src="docs/screenshots/alchemy-breath.gif" alt="{◆} Breathing">
</p>

</div>

---

## Why

Python AI agents are fat. A basic LangChain install pulls 200MB of dependencies. Claude Code embeds a Node.js runtime. Codex needs a GPU cloud. Alchemy exists because **an AI agent doesn't need a virtual machine** — it needs a UNIX process with OpenSSL, 136KB of native code, and a philosophy of doing less to run better.

Built for:
- **Embedded/edge** — no Python, no node, no runtime
- **Low-resource servers** — 136KB binary, ~2MB RSS
- **Education** — complete AI agent in readable C99
- **Portfolio** — proof that systems-level code is still the right answer

## Features

| Layer | Module | Status |
|-------|--------|:------:|
| **Identity** | 6-layer system prompt + LOCKED XOR encryption | ✅ |
| **Permissions** | `.alchemy/settings.json` — allow/ask/deny matrix | ✅ |
| **Hooks** | Pre/Post tool — function pointer chain | ✅ |
| **LLM Router** | Multi-provider + auto-fallback + SSE streaming | ✅ |
| **Context** | Sliding window with auto-truncation + visualization | ✅ |
| **Memory** | File-based KV store with session snapshots | ✅ |
| **Skill System** | File-based skills with level, XP, and automatic evolution | ✅ |
| **Skill Evolution** | Dreyfus model (Novice → Expert) + level-up notifications | ✅ |
| **RPG Status** | `/rpgstatus` — HTML character sheet with HP/MP/XP bars | ✅ |
| **Subagent** | `fork()+pipe()` isolated execution | ✅ |
| **MCP Client** | JSON-RPC 2.0 over stdio transport | ✅ |
| **Plugin Loader** | `dlopen/dlsym` + auto-discovery | ✅ |
| **Slash Commands** | 14 built-in commands (help, status, skill, model, ...) | ✅ |
| **Cost Tracking** | Per-model pricing, token counters, JSON reports | ✅ |
| **TUI** | ANSI status bar + progress + message area | ✅ |
| **Test Infra** | Mock LLM server + virtual filesystem | ✅ |
| **Browser Automation** | Firefox/Camoufox headless — screenshot, DOM, click | ✅ |
| **Dashboard** | HTML identity dashboard generator | ✅ |
| **Discord Gateway** | WebSocket REST + event handling | ✅ |

## Quick Start

```bash
git clone https://github.com/Caixa-git/alchemy.git
cd alchemy
make -j$(nproc)              # full build (46 C files, ~5s)
./build/alchemy              # CLI mode
```

Or launch the RPG status sheet:

```bash
./build/alchemy              # start the REPL
/rpgstatus                   # generate HTML character sheet
# opens: ~/alchemy_rpg_status.html
```

## Commands

| Command | Description |
|---------|-------------|
| `/help` | Show all available commands |
| `/status` | Show agent status (skills, tools, model, session) |
| `/model` | List available models and providers |
| `/session` | Show current session info |
| `/router` | Show provider routing details |
| `/stats` | Show agent statistics (JSON) |
| `/cost` | Show estimated token cost |
| `/new` | Reset session context |
| `/retry` | Retry the last message |
| `/clear` | Clear terminal screen |
| `/dashboard` | Generate HTML identity dashboard |
| `/rpgstatus` | Generate RPG-style HTML status sheet |
| `/skill` | Skill management (create, merge, suggest, category) |
| `/debug` | Toggle debug mode |
| `/exit` | Exit the REPL |

Alternatively, just type anything to chat with the AI agent.

## Architecture

```text
alchemy/
├── library/                     # 16 pure-C99 modules
│   ├── json/                    # DOM JSON parser, single-pass
│   ├── ds/                      # Dynamic array, string, session
│   ├── llm/                     # Multi-provider router + MCP
│   ├── algo/                    # Subagent, retry, slash, plugin,
│   │                            # cost, TUI, test, memory_auto,
│   │                            # worktree, task_queue, skill_sim
│   └── browser/                 # Headless browser control
├── src/
│   ├── core/                    # Agent core — 16 modules
│   │   ├── alchemy_agent.c      # Main agent loop
│   │   ├── alchemy_permit.c     # Permission engine
│   │   ├── alchemy_hook.c       # Pre/Post tool hooks
│   │   ├── alchemy_dashboard.c  # HTML dashboard generator
│   │   ├── alchemy_rpgstatus.c  # RPG status sheet generator
│   │   ├── alchemy_skill.c      # Skill management
│   │   ├── alchemy_skill_evolve.c # Skill leveling & evolution
│   │   └── ...
│   ├── cli/                     # Entry points (REPL)
│   └── gateway/discord/         # WebSocket Discord gateway
├── skills/                      # User-defined skill files (.skill)
├── docs/
│   ├── decisions/               # Architecture Decision Records
│   ├── design/                  # Design reviews
│   ├── research/                # Research references
│   └── screenshots/             # Visual evidence
└── build/alchemy                # 136KB release binary
```

The agent lifecycle flows through four layers:

```
[Gateway/Discord] → [Agent Core] → [LLM Router] → [Tool Execution]
                       ↕                ↕
                  [Context Window]  [Memory Store]
                       ↕
                 [Permission Gate]
```

## Design Decisions

| Principle | Rationale |
|-----------|-----------|
| **C99 only** | No runtime, no GC, predictable memory, 5s build |
| **Library-first** | Every new capability is a standalone C module in `library/` |
| **HTML output** | Both human-readable and LLM-parseable, no markdown ambiguity |
| **OpenSSL only** | The only external dependency — TLS + crypto in one lib |
| **Process isolation** | Subagents run in forked processes, not threads |
| **Zero malloc in hot path** | Ring buffers for streaming, static pools for tools |
| **Skill evolution** | Dreyfus competence model with visible XP progression |

## Status

> **v1.0.0 — Project Complete / 일시 중단** (May 2026)
>
> 이 프로젝트는 현재 버전(v1.0.0)까지 구현하고 **종료**합니다.
>
> **중단 사유:** LLM/Transformer 내부 구조, 멀티 에이전트 오케스트레이션, RAG/벡터 검색 등 기반 지식에 대한 추가 학습이 필요하다고 판단하여, 학습 완료 후 재개하기로 결정했습니다.
>
> **학습 계획:**
> - **LLM internals & Transformer architecture** — attention mechanisms, context window optimization, token efficiency
> - **Multi-agent systems** — orchestration, delegation, agent-to-agent communication patterns
> - **RAG & vector search** — embedding strategies, retrieval pipelines, hybrid search
>
> 이후 버전 개선은 학습 이후 재개 예정입니다. 자세한 로드맵은 [`docs/roadmap.md`](docs/roadmap.md)를 참고하세요.

```
Core Framework              ██████████ 100%  ✅
Slash Commands & TUI        ██████████ 100%  ✅
Skill System & Evolution    ██████████ 100%  ✅
RPG Status Sheet            ██████████ 100%  ✅
Discord Gateway             ██████████ 100%  ✅
Dashboard Generator         ██████████ 100%  ✅

| Binary:          136KB (release) / 271KB (debug)
| Source files:    46 C + 39 H = 19,221 LOC
| Libraries:       16 modules (json, ds, llm, algo, browser)
| Core modules:    16 (agent, skill, memory, context, ...)
| Dependencies:    libc + OpenSSL 3.0+ + pthread + dl
| Build time:      ~5s on modern x86_64
```

### What's next (post-study)

The full roadmap is at [`docs/roadmap.md`](docs/roadmap.md). After the study phase, priorities include:

- **Context compaction** — LLM-generated summaries to replace bloated conversation history (inspired by Codex CLI)
- **Fragment-based system prompt** — diff-only updates instead of full re-send
- **RAG integration** — vector search over codebase for self-modification
- **Multi-agent orchestration** — specialized sub-agents for code review, testing, deployment
- **Self-modification loop** — read → modify → build → test → repeat, fully autonomous

## References

- Prana et al. (2018). *Categorizing the Content of GitHub README Files.* arXiv:1802.06997
- Gaughan et al. (2025). *The Introduction of README and CONTRIBUTING Files in OSS.* arXiv:2502.18440
- Dreyfus & Dreyfus (1980). *A Five-Stage Model of the Mental Activities Involved in Directed Skill Acquisition.*
- Nous Research. *Hermes Agent — private AI operation system.* [hermes-agent.nousresearch.com](https://hermes-agent.nousresearch.com)
- *Architecture Decision Records.* [`docs/decisions/`](docs/decisions/)
- *Research Notes.* [`docs/research/`](docs/research/)

---

## About the Author

**WIJINSU (위진수)** — 8년차 C++ Windows 개발자. 쿠콘 핵심기술연구부에서 OpenSSL/PKI/HSM 기반 암호화 솔루션을 개발했습니다.

- **GitHub:** [Caixa-git](https://github.com/Caixa-git)

---

<p align="center"><sub>Built with C99, OpenSSL, and the belief that less is more.</sub></p>
