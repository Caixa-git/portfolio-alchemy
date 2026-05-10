---
date: 2026-05-08
type: benchmark
tags: [step4, discord, websocket, gateway]
status: active
---

# Step 4 벤치마크: Discord Gateway

## 실행 결과 (gdh 토큰, 실제 Discord Gateway 연결)

```
WebSocket upgrade → 101 Switching Protocols ✅
OP 10 Hello → HB interval=41250ms ✅
Identify 전송 ✅
READY 수신 → Bot online! ✅
Heartbeat ACK (OP 11) 수신 ✅
```

## 바이너리 크기

| Step | text | data | bss  | dec   | hex  | strip |
|:----|:---:|:----:|:----:|:-----:|:----:|:-----:|
| step0 (TCP GET) | 3,992 | 690 | 104 | 4,786 | 12B2 | 17K |
| step1 (SSL GET) | 7,109 | 690 | 104 | 7,903 | 1EDF | 22K |
| step2 (LLM API) | 10,783 | 690 | 104 | 11,577 | 2D39 | 23K |
| **step4 (Discord)** | **20,762** | **690** | **111** | **21,563** | **543B** | **44K** |

증가: step2→step4: text +10K, strip +21K (WebSocket 프레임 + Gateway 프로토콜 + JSON 파싱)

## 버그 수정 내역

1. `clock()` → `time()`: WSL에서 CPU 시간이 아닌 wall clock으로 heartbeat 타이밍 수정
2. `"t":"` 오프셋 오류: `t += 4` → `t += 5` (5글자 `"t":"`를 올바르게 스킵, 이전엔 빈 문자열로 event 파싱)
3. Identify JSON 죽은 코드 제거: 첫 snprintf가 깨진 JSON을 썼다가 두 번째에서 덮어씀
4. 경고 제거: sign-compare, unused-parameter
