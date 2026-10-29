#include <string.h>
#undef NULL // redefined in _walloc.h
#include <stdlib/_walloc.h> // implementation here
#ifndef NULL  
#define NULL ((void *) 0)
#endif

// malloc and free provided by walloc

void *calloc(size_t nitems, size_t size) {
    void *ret;

    ret = malloc((size_t)(size * nitems));

    if (ret) {
        memset(ret, 0x00, (size_t)(size * nitems));
    }

    return ret;
}

void *
realloc(void *ptr, size_t size) {

  if (!ptr) {
    return malloc(size);
  }

  if (size == 0) {
    free(ptr);
    return NULL;
  }

  struct page *page = get_page(ptr);
  unsigned chunk = get_chunk_index(ptr);
  uint8_t kind = page->header.chunk_kinds[chunk];

  size_t old_size;

  if (kind == LARGE_OBJECT) {
    struct large_object *obj = get_large_object(ptr);
    old_size = obj->size;
  } else {
    ASSERT(kind < SMALL_OBJECT_CHUNK_KINDS);
    old_size = (size_t) chunk_kind_to_granules((enum chunk_kind) kind) * GRANULE_SIZE;
  }

  // 原容量已经够用，直接原地返回。
  if (size <= old_size) {
    return ptr;
  }

  void *new_ptr = malloc(size);
  if (!new_ptr) {
    return NULL;
  }

  memcpy(new_ptr, ptr, old_size);
  free(ptr);
  return new_ptr;
}
