// libc malloc/free/realloc/calloc backed by Doug Lea's dlmalloc 2.8.6
// (original release: ftp://gee.cs.oswego.edu/pub/misc/malloc.c, MIT-0).
// Configured for wasm32 freestanding: no mmap, single-threaded, memory
// obtained via sbrk() (dlmalloc/sbrk_wasm.c, wasm memory.grow based).
//
// Replaces the previous walloc backend (kept as malloc.c.walloc-disabled),
// which type-punned page metadata and needed -fno-strict-aliasing to
// survive clang >= 22 optimisations. dlmalloc accesses its headers through
// properly typed structs, so strict aliasing is safe.

#include <stddef.h>

#define HAVE_MMAP 0
/* default MORECORE is sbrk(); our wasm sbrk never shrinks */
#define MORECORE_CANNOT_TRIM 1
/* wasm32: max_align_t is 8-byte aligned */
#define MALLOC_ALIGNMENT ((size_t)8U)
/* wasm page is 64KiB; dlmalloc's own page guess would be 4KiB */
#define malloc_getpagesize ((size_t)65536U)
/* grow the heap in 64KiB steps at least */
#define DEFAULT_GRANULARITY ((size_t)65536U)
/* no stdio dependency for malloc_stats */
#define NO_MALLOC_STATS 1
/* freestanding: hide the unix headers dlmalloc would probe for */
#define LACKS_UNISTD_H
#define LACKS_FCNTL_H
#define LACKS_SYS_PARAM_H
#define LACKS_SYS_MMAN_H
#define LACKS_SYS_TYPES_H
#define LACKS_STRINGS_H
#define LACKS_TIME_H
/* our libc provides <errno.h> with errno + ENOMEM/EINVAL */
#include <errno.h>

#include "dlmalloc/dlmalloc.c"
/* no USE_DL_PREFIX: dlmalloc.c defines malloc/free/calloc/realloc directly */
