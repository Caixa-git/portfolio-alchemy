# Alchemy README 이미지 프롬프트

> 생성 도구: **ChatGPT (GPT Image 2)** — 두타입
> 참고: [awesome-gpt-image-2](https://github.com/YouMind-OpenLab/awesome-gpt-image-2)
> 
> **GPT Image 2 특징:**
> - 픽셀 퍼펙트 텍스트 렌더링 (한글/영문/일문 완벽)
> - JSON 구조 프롬프트로 레이아웃 정밀 제어
> - 상업용 일러스트 수준 출력
> - 크로스 이미지 일관성 (같은 스타일 유지)
>
> **프롬프트 작성법:**
> - 자연어 설명 ❌ → JSON 구조 ✅
> - `type`으로 이미지 종류 정의
> - `layout`으로 위치/개수/내용 구체적으로

---

## 1️⃣ 히어로 이미지 — README 최상단 배너

**위치:** 제목 아래, "Why" 섹션 위
**용도:** 3초 안에 "연금술 + C99 + AI" 전달
**크기:** 16:9 (1280×720)

### 프롬프트 (GPT Image 2 JSON)

```
{
  "type": "cinematic brand hero banner",
  "subject": "Alchemy — C99 AI Agent Framework",
  "style": "dark cinematic matte painting meets technical diagram, volumetric lighting, dust motes floating in light beams, precise geometric lines",
  "background": "deep midnight blue-black gradient (#0a0a1a to #1a1a2e)",
  "layout": {
    "left_section": {
      "position": "left 25%",
      "content": {
        "centerpiece": "glowing philosopher's stone — perfect circle inscribed in an equilateral triangle, surrounded by 3 concentric orbital rings, emits warm golden-green phosphorescent glow",
        "details": "ancient alchemical symbols faintly etched into the rings, 16th century illuminated manuscript aesthetic, sacred geometry precision"
      }
    },
    "center_section": {
      "position": "center 40%",
      "content": {
        "centerpiece": "C99 source code fragments arranged in concentric orbital rings around a glowing central nucleus",
        "code_fragments": [
          "#include <openssl/ssl.h>",
          "struct alchemy_agent {",
          "typedef struct {",
          "int router_select(",
          "void *ctx, int flags",
          "76KB — zero Python"
        ],
        "style": "retro terminal monospace font (IBM 3278 / VT220), glowing cyan (#00d4ff), semi-transparent, fragments at various orbital radii, some in focus some blurred (depth of field)"
      }
    },
    "right_section": {
      "position": "right 25%",
      "content": {
        "centerpiece": "simplified neural network topology — glowing nodes connected by thin luminous teal-blue lines, constellation-like dot pattern",
        "details": "a single small diamond at the center pulses with cooler teal-cyan light (#4fc3f7), minimalist AI representation"
      }
    },
    "overlay_text": {
      "position": "bottom-center, 10% from bottom",
      "text": "AI Agent Framework · C99 · 76KB",
      "style": "modern sans-serif, thin weight, letter-spaced, 30% opacity white, no background box"
    }
  },
  "color_palette": {
    "background": "#0a0a1a to #1a1a2e gradient",
    "alchemy_elements": "warm gold (#d4a574), amber (#c9953b), emerald green accents",
    "code_elements": "cyan (#00d4ff), cool white (#e8e8f0)",
    "network_ai": "teal-blue (#4fc3f7), violet undertones",
    "forbidden": "no bright reds, no neon gaming aesthetics, no aggressive colors"
  },
  "mood": "Ancient meets modern. The philosopher's stone reimagined as a C compiler. Mystery, rigor, elegance."
}
```

### 실행

ChatGPT에 JSON을 그대로 붙여넣기.  
"16:9 비율로 해줘" 하면 됨.  
결과가 마음에 안 들면 "code fragments를 더 선명하게", "더 어둡게", "배경에 별빛 입자 추가" 같은 단순 수정.

**저장:** `docs/screenshots/alchemy-hero.png`  
**압축:** `pngquant --quality=65-80 docs/screenshots/alchemy-hero.png`

---

## 2️⃣ 로고/아이콘 — GitHub Avatar + README 인라인

**용도:** GitHub 레포 아이콘, README 제목 옆, favicon
**크기:** 정사각형 (1024×1024 → 실제 사용 128×128)
**조건:** 작아도 읽히고, 흑백으로도 알아볼 수 있어야

### 프롬프트 (GPT Image 2 JSON)

```
{
  "type": "minimalist tech logo icon",
  "subject": "Alchemy brand mark",
  "style": "Apple-style flat design meets alchemical symbolism, precise 1-2px stroke geometric lines, rounded corners, vector-like clarity, no textures, no noise",
  "background": "perfect gradient #0d0d1a (top) to #1a1a2e (bottom)",
  "layout": {
    "centerpiece": {
      "description": "An alchemical alembic (distillation flask) whose silhouette is simultaneously a C programming curly brace '{' — the curve of the brace forms the neck of the flask, the straight edge forms its body",
      "stroke": "precise thin geometric lines, 1.5px width, warm gold (#d4a574) color",
      "fill": "transparent, only outlines visible"
    },
    "inner_element": {
      "description": "Inside the flask floats a single small diamond shape — represents both the philosopher's stone and an AI neural network node simultaneously",
      "gradient": "warm gold (#d4a574) to cyan (#00d4ff), top-left to bottom-right",
      "size": "approximately 20% of the flask interior"
    },
    "outer_ring": {
      "description": "A very subtle circular halo/orbit surrounds the icon at a distance, like an electron orbit",
      "opacity": "10%",
      "stroke": "1px, teal-blue (#4fc3f7)",
      "purpose": "adds depth without clutter"
    }
  },
  "padding": "30% margin around the central design",
  "constraints": [
    "recognizable at 128x128 pixels",
    "works in grayscale (design loses no meaning without color)",
    "dark optimized but shape is visible on white too",
    "no text, no letters, no watermarks",
    "pure visual symbol only"
  ],
  "mood": "Precision. Intelligence. Transformation. An AI framework logo designed by Jony Ive during his medieval phase."
}
```

### 실행

ChatGPT에 붙여넣고 "정사각형으로".  
여러 변형 생성해보고 마음에 드는 거 고르면 됨.

**저장:**
- 원본: `docs/assets/alchemy-icon.png` (1024×1024)
- 축소: `docs/assets/alchemy-icon-128.png` (128×128)
- 흑백 테스트: grayscale로 변환해봤을 때도 형태가 유지되는지 확인

---

## 3️⃣ 아키텍처 다이어그램 — README Architecture 섹션용

**용도:** GitHub README에서 "Architecture" 섹션에 삽입
**스타일:** Excalidraw-meets-Figma — 손그림 느낌의 깔끔한 기술 다이어그램
**배경:** 흰색/밝은 배경 (README 기본 테마 호환)

### 프롬프트 (GPT Image 2 JSON)

```
{
  "type": "technical architecture diagram infographic",
  "style": "clean figma-style diagram, rounded rectangles with subtle gradient fills, thin 1px connection lines with arrow tips, minimal shadows, light grid background (barely visible 5% opacity)",
  "background": "clean white (#fafafa), subtle light grid (#e0e0e0 at 30% opacity)",
  "layout": {
    "title": {
      "position": "top-center",
      "text": "Alchemy Architecture",
      "style": "modern sans-serif, bold, 24px, dark gray (#1a1a2e)"
    },
    "layers": [
      {
        "name": "Gateway",
        "position": "top row, center",
        "color": "warm gold (#d4a574) at 20% fill, #d4a574 border",
        "icon": "minimalist web socket / plug icon",
        "label": "Discord Gateway · WebSocket + REST",
        "connects_to": "Agent Core"
      },
      {
        "name": "Agent Core",
        "position": "second row, center",
        "color": "cyan (#00d4ff) at 15% fill, #00d4ff border",
        "width": "60% of page width (wider than others)",
        "sub_modules": [
          {"name": "Permission Engine", "color": "#d4a574"},
          {"name": "Hook System", "color": "#d4a574"},
          {"name": "Context Window", "color": "#4fc3f7"},
          {"name": "Memory Store", "color": "#4fc3f7"}
        ],
        "label": "Agent Core · 12 C99 modules",
        "connects_to": ["LLM Router", "Tool Execution"]
      },
      {
        "name": "LLM Router",
        "position": "third row, left 50%",
        "color": "teal (#4fc3f7) at 15% fill, #4fc3f7 border",
        "branches": [
          {"name": "DeepSeek", "icon": "minimalist cloud"},
          {"name": "Claude", "icon": "minimalist cloud"},
          {"name": "OpenAI", "icon": "minimalist cloud"}
        ],
        "label": "Multi-Provider · Auto Fallback · SSE Streaming"
      },
      {
        "name": "Tool Execution",
        "position": "third row, right 50%",
        "color": "cool gray (#8a8a9a) at 15% fill, #8a8a9a border",
        "icons": ["terminal icon", "file icon", "HTTP icon", "code icon"],
        "label": "Terminal · File I/O · HTTP · Code Execution"
      }
    ],
    "annotation": {
      "position": "right side, vertically centered",
      "text": "76KB\nZero Python",
      "style": "monospace font, 14px, teal (#4fc3f7), rotated -90 degrees or vertical text, letter-spaced"
    },
    "connection_lines": {
      "style": "1px solid, #888 at 50% opacity, small filled arrow at endpoint",
      "routing": "orthogonal (Manhattan) routing, right-angle turns"
    }
  },
  "color_palette": {
    "background": "#fafafa",
    "gateway": "#d4a574 (warm gold)",
    "core": "#00d4ff (cyan)",
    "llm": "#4fc3f7 (teal)",
    "tools": "#8a8a9a (cool gray)",
    "text": "#1a1a2e (dark navy)",
    "grid": "#e0e0e0"
  }
}
```

### 실행

ChatGPT에 붙여넣고 "16:9 비율, 흰 배경"으로 요청.  
README에 바로 넣을 거라서 GitHub 다크 모드에서도 보이게 `<picture>` 태그 고려.

**저장:** `docs/screenshots/alchemy-architecture.png`

---

## 4️⃣ GitHub Social Preview (선택)

README에 직접 넣는 건 아니지만, GitHub 레포 링크를 카톡/트위터/슬랙에 공유할 때 OG 이미지로 뜨는 1280×640 배너.

위 히어로 이미지와 같은 프롬프트 쓰고 `"aspect_ratio": "1280x640"`만 추가하면 됨.

---

## README 적용법

히어로 이미지와 로고가 준비되면:

```markdown
# ![Alchemy](docs/assets/alchemy-icon.png) Alchemy

![Alchemy Hero](docs/screenshots/alchemy-hero.png)

> C99 AI Agent Framework — Pure C, zero Python, 76KB.
```

또는 `<picture>` 태그로 다크/라이트 모드 분기:

```html
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/screenshots/alchemy-hero.png">
  <img src="docs/screenshots/alchemy-architecture.png" alt="Alchemy">
</picture>
```

---

## 프롬프트 작성 팁 (GPT Image 2)

| GPT Image 2 특징 | 프롬프트 전략 |
|:-----------------|:--------------|
| **JSON 구조 이해** | 자연어 설명 대신 `type`/`layout`/`style`/`color_palette` 필드 사용 |
| **픽셀 퍼펙트 텍스트** | `overlay_text`, `label`, `annotation`에 실제 텍스트 포함 가능 |
| **레이아웃 제어** | `position`, `count`, `width`로 각 요소 위치/크기 지정 |
| **크로스 이미지 일관성** | 여러 이미지 생성 시 `color_palette` 동일하게 유지 |
| **상업용 품질** | `style`에 "cinematic", "commercial-grade", "vector-like" 등의 단어 포함 |
| **복합 구조** | `sub_modules`, `branches`, `sections` 배열로 복잡한 구성도 가능 |
| **픽셀 단위 제어** | `stroke: "1.5px"`, `opacity: "10%"` 같은 세밀한 값 지정 가능 |
