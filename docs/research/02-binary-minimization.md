# C 바이너리 최소화 기법

## 적용 순서 (효과 큰 순서)

### 1. 컴파일러 플래그
```makefile
# 최대 용량 최적화 조합
CFLAGS = -Os \
         -ffunction-sections -fdata-sections \
         -fno-exceptions -fno-unwind-tables \
         -fno-stack-protector \
         -flto

LDFLAGS = -Wl,--gc-sections -Wl,--strip-all
```

### 2. 링커 스크립트
```ld
SECTIONS {
    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.debug*)
        *(.ARM.attributes)
    }
}
```

### 3. 코드 레벨 절약
- **인라인 vs 함수**: 2회 이하 호출은 인라인, 3회 이상은 함수 분리
- **전역 변수 통합**: 여러 int8_t를 하나의 비트필드로 결합
- **Lookup Table vs 계산**: 64바이트 이하는 계산, 이상은 LUT

## 기대 감소율
| 기법 | 감소율 | 누적 |
|------|--------|------|
| -Os | 30% | 30% |
| + LTO | 14%p | 44% |
| + function-sections + gc-sections | 18%p | 62% |
| + strip | 18%p | 80% |
| + 링커 스크립트 | 1%p | 81% |
