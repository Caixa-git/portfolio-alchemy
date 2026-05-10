# C 레벨 메모리 최적화 전략

## 1. 메모리 풀 (Slab Allocator)
```c
// Alchemy 전용 메모리 풀
typedef struct {
    size_t   obj_size;  // 객체 크기
    int      capacity;  // 최대 개수
    char    *pool;      // 연속 메모리 블록
    int     *free_idx;  // free index 스택
    int      top;       // 스택 top
} mempool_t;

// 할당: O(1), 파편화 0%
void *mempool_alloc(mempool_t *mp) {
    if (mp->top < 0) return NULL;
    return mp->pool + mp->free_idx[mp->top--] * mp->obj_size;
}

// 해제: O(1), 반납만
void mempool_free(mempool_t *mp, void *obj) {
    int idx = ((char*)obj - mp->pool) / mp->obj_size;
    mp->free_idx[++mp->top] = idx;
}
```

## 2. Zero-copy Ring Buffer
```c
// 생산자-소비자 패턴 (lock-free, power-of-2 size)
typedef struct {
    uint8_t *buf;
    size_t   size;      // 반드시 2^n
    size_t   head;      // 생산자
    size_t   tail;      // 소비자
} ringbuf_t;
```

## 3. Structure Packing
```c
// 패딩 최소화: 큰 타입 → 작은 타입 순서 배치
struct alchemy_message {
    uint64_t timestamp;  // 8
    uint32_t length;     // 4
    uint16_t type;       // 2
    uint8_t  flags;      // 1
    uint8_t  pad;        // 1 (padding)
    char     data[];     // flexible array
} __attribute__((packed));
```

## 4. 정적 할당 vs 동적
**원칙: 무조건 정적 할당 우선**
- 컴파일 타임에 최대 크기가 정해진 버퍼는 static array
- 예외: 가변 길이 메시지만 동적 풀에서 할당
- malloc/free 사용 금지 (메모리 풀로 대체)
