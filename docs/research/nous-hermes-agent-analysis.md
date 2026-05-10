# Hermes Agent 완전 분석

> Nous Research의 Hermes Agent 저장소 분석 결과
> 기반: GitHub (7,675 commits, 139k stars), 공식 문서

---

## 1. 전체 시스템 구조

```
                    ┌──────────────────────────┐
                    │     CLI / TUI / Gateway    │  ← 사용자 접점
                    └────────────┬─────────────┘
                                 │
                    ┌────────────▼─────────────┐
                    │     Core Engine           │
                    │  (context_engine, prompt) │
                    │  (context_compressor)     │
                    └────────────┬─────────────┘
                                 │
         ┌───────────────────────┼───────────────────────┐
         │                       │                       │
    ┌────▼────┐           ┌──────▼──────┐         ┌─────▼─────┐
    │ LLM     │           │  Memory     │         │  Skills   │
    │ Adapter │           │  System    │         │  System   │
    │ (6개)   │           │            │         │           │
    └─────────┘           └────────────┘         └───────────┘
                                 │
         ┌───────────────────────┼───────────────────────┐
         │                       │                       │
    ┌────▼────┐           ┌──────▼──────┐         ┌─────▼─────┐
    │ Tools   │           │  Gateway    │         │  Cron     │
    │ (40+)   │           │  (6개)      │         │  Scheduler│
    └─────────┘           └────────────┘         └───────────┘
```

## 2. 핵심 모듈 분석

### 2.1 LLM Adapters (agent/)
| 파일 | 역할 |
|------|------|
| `chat_completions.py` | OpenAI-compatible API (DeepSeek, OpenAI 등) |
| `anthropic_adapter.py` | Anthropic Claude 전용 |
| `gemini_native_adapter.py` | Google Gemini |
| `bedrock_adapter.py` | AWS Bedrock |
| `lmstudio_reasoning.py` | 로컬 LM Studio |
| `models_dev.py` | Model 개발/메타데이터 |
| `model_metadata.py` | 모델 정보 관리 |

### 2.2 Memory System
| 파일 | 역할 |
|------|------|
| `memory_manager.py` | 메모리 CRUD, 검색, 요약 |
| `memory_provider.py` | 메모리 저장소 인터페이스 |
| `context_engine.py` | 컨텍스트 윈도우 관리 |
| `context_compressor.py` | 컨텍스트 압축/요약 |

### 2.3 Skills System
| 파일 | 역할 |
|------|------|
| `skill_commands.py` | 스킬 명령어 처리 (/skill) |
| `skill_preprocessing.py` | 스킬 실행 전처리 |
| `skill_utils.py` | 스킬 유틸리티 |
| `curator.py` | 스킬 큐레이터 (생성/개선/병합) |

### 2.4 Tool System
| 도구셋 | 포함 도구 |
|--------|----------|
| browser | navigate, click, snapshot, vision 등 |
| terminal | shell, process, sudo |
| file | read, write, patch, search |
| web | web_search, web_extract |
| delegation | delegate_task |
| cron | cronjob |
| memory | session_search, memory |
| skills | skill_view, skill_manage |

### 2.5 Messaging Gateway
| 플랫폼 | 전송 방식 |
|--------|----------|
| Telegram | Bot API (long polling) |
| Discord | Gateway WebSocket |
| Slack | Socket Mode |
| WhatsApp | Cloud API |
| Signal | Signal Messenger |
| Email | IMAP/SMTP |

### 2.6 지원 기능
- **Cron Scheduler**: 자연어 스케줄링, 주기적 작업
- **Personality System**: SOUL.md로 성격 정의
- **Context Files**: AGENTS.md, CLAUDE.md 등 프로젝트 컨텍스트
- **Voice Mode**: 음성 인식/합성 (Telegram, Discord)
- **MCP Integration**: 외부 MCP 서버 연결
- **Security**: 명령 승인, DM 페어링, 컨테이너 격리

## 3. 기술 스택
- **언어**: Python 3.10+
- **LLM**: 200+ 모델 지원 (OpenRouter, Nous Portal)
- **검색**: SQLite FTS5 (세션 검색)
- **배포**: pip install, Docker, Modal, Daytona
- **설치**: 단일 curl 스크립트

## 4. 핵심 데이터 흐름
```
사용자 메시지
  → Gateway 수신
  → Context Engine (히스토리 + 메모리 + 스킬)
  → Prompt Builder (시스템 프롬프트 구성)
  → LLM Adapter (API 호출)
  → 응답 파싱 (tool_calls 감지)
  → Tool 실행 (함수 호출)
  → 결과를 다시 LLM에 전달
  → 최종 응답 생성
  → Gateway 전송
```
