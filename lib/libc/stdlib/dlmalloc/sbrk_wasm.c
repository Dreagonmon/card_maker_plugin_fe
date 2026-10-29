// Minimal sbrk() for wasm32, used as dlmalloc's MORECORE.
// Grows the linear memory with the wasm memory.grow instruction;
// never shrinks (wasm memory can only grow), so MORECORE_CANNOT_TRIM.
#include <stddef.h>
#include <stdint.h>

#define WASM_PAGE_SIZE 65536u

extern char __heap_base;

static uintptr_t cur_break;

static uintptr_t mem_bytes(void) {
  return (uintptr_t)__builtin_wasm_memory_size(0) * WASM_PAGE_SIZE;
}

void *sbrk(ptrdiff_t incr) {
  if (!cur_break) {
    uintptr_t base = (uintptr_t)&__heap_base;
    // start page-aligned so dlmalloc's first segment is page aligned
    cur_break = (base + WASM_PAGE_SIZE - 1) & ~(uintptr_t)(WASM_PAGE_SIZE - 1);
  }
  uintptr_t old = cur_break;
  if (incr == 0)
    return (void *)old;
  if (incr < 0) {
    // wasm memory is never given back; pretend we trimmed nothing.
    return (void *)old;
  }
  uintptr_t newb = old + (uintptr_t)incr;
  uintptr_t have = mem_bytes();
  if (newb > have) {
    size_t pages = (size_t)((newb - have + WASM_PAGE_SIZE - 1) / WASM_PAGE_SIZE);
    if (__builtin_wasm_memory_grow(0, pages) == (size_t)-1)
      return (void *)-1;
  }
  cur_break = newb;
  return (void *)old;
}
