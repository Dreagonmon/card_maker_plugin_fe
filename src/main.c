#include "fe.h"
#include "fe_ext.h"
#include <stdio.h>
#include <stdlib.h>
#include <wasmenv.h>

int gc_pos = 0;
fe_Context *ctx = NULL;

// static void eval_string(const void *text) {
//   // compile
//   fe_Object *inst = fe_readstr(ctx, (char *)text);
//   // eval
//   fe_Object *res = fe_eval(ctx, inst);
//   /* clean
//    * restore GC stack which would now contain both the read object and
//    * result from evaluation */
//   fe_restoregc(ctx, gc_pos);
//   fe_collect_garbage(ctx);
// }

// WASM_EXPORT("eval_vfile")
// void eval_vfile(const void *vfile) {
//   // compile
//   fe_Object *inst = fe_readvfp(ctx, vfile);
//   // eval
//   fe_Object *res = fe_eval(ctx, inst);
//   /* clean
//    * restore GC stack which would now contain both the read object and
//    * result from evaluation */
//   fe_restoregc(ctx, gc_pos);
//   fe_collect_garbage(ctx);
// }

WASM_EXPORT("eval")
void eval() {
  int local_gc = fe_savegc(ctx);
  // get vfile name
  fe_Object *objs[2];
  objs[0] = fe_symbol(ctx, "getstr");
  objs[1] = fe_string(ctx, "script_file");
  fe_Object *filename = fe_eval(ctx, fe_list(ctx, objs, 2));
  // check
  if (fe_isnil(ctx, filename)) {
    printf("Error: string variable \"script_file\" is nil.\n");
    return;
  }
  // open vfile
  objs[0] = fe_symbol(ctx, "vfopen");
  objs[1] = filename;
  fe_Object *vfp_obj = fe_eval(ctx, fe_list(ctx, objs, 2));
  // check
  if (fe_isnil(ctx, vfp_obj)) {
    printf("Error: failed to open script file.\n");
    return;
  }
  void *vfile = fe_toptr(ctx, vfp_obj);

  // gc
  fe_restoregc(ctx, local_gc);

  // eval file
  //   compile
  fe_Object *inst = fe_readvfp(ctx, vfile);
  //   eval
  fe_Object *res = fe_eval(ctx, inst);

  // gc
  fe_restoregc(ctx, local_gc);

  // close file
  local_gc = fe_savegc(ctx);
  objs[0] = fe_symbol(ctx, "vfclose");
  objs[1] = fe_ptr(ctx, vfile);
  fe_eval(ctx, fe_list(ctx, objs, 2));

  // gc
  fe_restoregc(ctx, local_gc);
  fe_collect_garbage(ctx);
}

WASM_EXPORT("_start")
void _start() {
  // no dym mem, get remain
  extern void __heap_base;
  uintptr_t heap_start = (uintptr_t)&__heap_base;
  // extern void __heap_end;
  // uintptr_t heap_end = (uintptr_t)&__heap_end;
  uintptr_t heap_end = __builtin_wasm_memory_size(0) * (64 * 1024);
  // align to pointer size
  heap_start = heap_start - (heap_start % sizeof(void *));
  size_t remaining_bytes = heap_end - heap_start;
  // align to pointer size
  remaining_bytes = remaining_bytes - (remaining_bytes % sizeof(void *));
  // use remained mem to init
  ctx = fe_open((void *)heap_start, remaining_bytes);
  register_extended_functions(ctx);
  // printf("free pair after open: %u\n", count_free_pair(ctx));

  /* evaluate */
  // eval_string("(do (print (stksz)) (let myvar (list 1 2 3)) (print (car
  // myvar)) (print (stksz)) 0)"); printf("free pair after eval: %u\n",
  // count_free_pair(ctx));

  // printf("==== dump global symbols ====\n");
  // fe_dump_global_symbles(ctx);

  /* close context */
  // fe_close(ctx);
}
