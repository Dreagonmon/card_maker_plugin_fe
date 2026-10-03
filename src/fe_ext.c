#include "fe_ext.h"
#include "fe.c"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define SMALL_STRING_BUF_SIZE (256)
#define STRING_POOL_BUF_SIZE (128 * 1024)
static char string_buffer[SMALL_STRING_BUF_SIZE];
static char string_pool[STRING_POOL_BUF_SIZE];

typedef struct buf {
  const char *src;
  size_t len;
  size_t pos;
} buf_t;

static char readbuf(__attribute__((__unused__)) fe_Context *ctx, void *udata) {
  buf_t *buf = udata;
  if (buf->pos >= buf->len) {
    return '\0';
  }
  return buf->src[buf->pos++];
}

static char readvfp(__attribute__((__unused__)) fe_Context *ctx, void *vfp) {
  int val = vfile_getc((VFile)vfp);
  return (val < 0) ? '\0' : (char)(val & 0xFF);
}

static void writefestr(fe_Context *ctx, void *udata, char chr) {
  fe_Object **tail = udata;
  *tail = buildstring(ctx, (*tail), chr);
}

fe_Object *fe_readbuf(fe_Context *ctx, const char *src, size_t len) {
  buf_t buf = {src, len, 0};
  return fe_read(ctx, readbuf, &buf);
}

fe_Object *fe_readstr(fe_Context *ctx, const char *src) {
  return fe_readbuf(ctx, src, strlen(src));
}

fe_Object *fe_readvfp(fe_Context *ctx, VFile vfp) {
  return fe_read(ctx, readvfp, (void *)vfp);
}

void fe_dump_global_symbles(fe_Context *ctx) {
  fe_Object *obj;
  for (obj = ctx->symlist; !isnil(obj); obj = cdr(obj)) {
    fe_Object *sym = car(obj);
    fe_Object *pair = cdr(sym);
    fe_Object *name = car(pair);
    fe_Object *value = cdr(pair);
    int symtyp = type(value);
    if (symtyp != FE_TPRIM && symtyp != FE_TCFUNC && sym != ctx->t) { // filter builtin vars
      fe_tostring(ctx, sym, string_buffer, SMALL_STRING_BUF_SIZE);
      printf("%s: %s = ", string_buffer, typenames[symtyp]);
      fe_tostring(ctx, value, string_buffer, SMALL_STRING_BUF_SIZE);
      printf("%s\n", string_buffer);
    }
  }
}

void fe_clean_global_nil_symbles(fe_Context *ctx) {
  fe_Object *parent = NULL;
  fe_Object *obj = ctx->symlist;
  while (!isnil(obj)) {
    fe_Object *sym = car(obj);
    fe_Object *value = cdr(cdr(sym));
    if (type(value) == FE_TNIL) {
      // nil, delete symbol from list
      if (parent) {
        cdr(parent) = cdr(obj);
      } else {
        ctx->symlist = cdr(obj);
      }
      obj = cdr(obj);
      continue;
    }
    // next
    parent = obj;
    obj = cdr(obj);
  }
}

unsigned int count_free_pair(fe_Context *ctx) {
  unsigned int count = 0;
  fe_Object *obj = ctx->freelist;
  while (!isnil(obj)) {
    count++;
    obj = cdr(obj);
  }
  return count;
}

void fe_collect_garbage(fe_Context *ctx) {
  fe_clean_global_nil_symbles(ctx);
  collectgarbage(ctx);
}

/* Extend fe functions */

/* (> a b) if a > b, return true. */
static fe_Object *f_gt(fe_Context *ctx, fe_Object *arg) {
  fe_Number a = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
  fe_Number b = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
  return fe_bool(ctx, (a > b));
}
/* (>= a b) if a >= b, return true. */
static fe_Object *f_gte(fe_Context *ctx, fe_Object *arg) {
  fe_Number a = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
  fe_Number b = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
  return fe_bool(ctx, (a >= b));
}
/* (type obj) get type number of an obj */
static fe_Object *f_type(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj = fe_nextarg(ctx, &arg);
  return fe_number(ctx, type(obj));
}
/* (stksz) get current gc stack size */
static fe_Object *f_stacksize(fe_Context *ctx, fe_Object *arg) {
  return fe_number(ctx, ctx->gcstack_idx);
}
/* (dumpglobal) get current gc stack size */
static fe_Object *f_dumpglobal(fe_Context *ctx, fe_Object *arg) {
  fe_dump_global_symbles(ctx);
  return fe_bool(ctx, 0); // nil
}
/* (memfree) get free pair count */
static fe_Object *f_memfree(fe_Context *ctx, fe_Object *arg) {
  return fe_number(ctx, count_free_pair(ctx));
}
/* (strlen obj) get type number of an obj */
static fe_Object *f_strlen(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj = fe_nextarg(ctx, &arg);
  checktype(ctx, obj, FE_TSTRING);
  int length = 0;
  while (!isnil(obj)) {
    int i;
    for (i = 0; i < STRBUFSIZE && strbuf(obj)[i]; i++) {
      // do nothing, just count
    }
    length += i;
    obj = cdr(obj);
  }
  return fe_number(ctx, length);
}
/* (strappend str chr chr chr ...) append char(number as uint_8) to the end of
 * str */
static fe_Object *f_strappend(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj_str = fe_nextarg(ctx, &arg);
  // find tail
  fe_Object *tail = obj_str;
  while (!isnil(cdr(tail))) {
    tail = cdr(tail);
  }
  while (!isnil(arg)) {
    fe_Object *obj = car(arg);
    if (type(obj) == FE_TNUMBER) {
      // append char
      char chr = (unsigned char)fe_tonumber(ctx, obj);
      tail = buildstring(ctx, tail, chr);
    } else {
      tail = buildstring(ctx, tail, '?');
    }
    arg = cdr(arg);
  }
  return obj_str;
}
/* (strcat str ...) concat obj to the end of str */
static fe_Object *f_strcat(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj_str = fe_nextarg(ctx, &arg);
  // find tail
  fe_Object *tail = obj_str;
  while (!isnil(cdr(tail))) {
    tail = cdr(tail);
  }
  while (!isnil(arg)) {
    fe_Object *obj = car(arg);
    fe_write(ctx, obj, writefestr, &tail, 0);
    arg = cdr(arg);
  }
  return obj_str;
}
/* (vfopen filepath) open a virtual file, return a ptr, or nil if failed. */
static fe_Object *f_vfopen(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj_path = fe_nextarg(ctx, &arg);
  fe_tostring(ctx, obj_path, string_buffer, SMALL_STRING_BUF_SIZE);
  VFile vfp = vfile_open(string_buffer);
  if (vfp) {
    return fe_ptr(ctx, (void *)vfp);
  }
  return fe_bool(ctx, 0); // nil
}
/* (vfclose vfp) close a virtual file. */
static fe_Object *f_vfclose(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj_ptr = fe_nextarg(ctx, &arg);
  VFile vfp = (VFile)fe_toptr(ctx, obj_ptr);
  vfile_close(vfp);
  return fe_bool(ctx, 0); // nil
}
/* (vfgetc vfp) read a char from virtual file. return char as uint8 or -1 end. */
static fe_Object *f_vfgetc(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj_ptr = fe_nextarg(ctx, &arg);
  VFile vfp = (VFile)fe_toptr(ctx, obj_ptr);
  int c = vfile_getc(vfp);
  return fe_number(ctx, (fe_Number)c);
}
/* (setstr obj) set string variable, return true. */
static fe_Object *f_setstr(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj_name = fe_nextarg(ctx, &arg);
  fe_Object *arg_text = arg;
  fe_Object *obj_text = fe_nextarg(ctx, &arg);
  // name
  fe_tostring(ctx, obj_name, string_buffer, SMALL_STRING_BUF_SIZE);
  // value
  fe_tostring(ctx, obj_text, string_pool, STRING_POOL_BUF_SIZE);
  set_string(string_buffer, string_pool);
  return fe_bool(ctx, 0); // nil
}
/* (setnum obj) set number variable, return true. */
static fe_Object *f_setnum(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj_name = fe_nextarg(ctx, &arg);
  fe_Object *obj_num = fe_nextarg(ctx, &arg);
  // name
  fe_tostring(ctx, obj_name, string_buffer, SMALL_STRING_BUF_SIZE);
  // value
  fe_Number num = fe_tonumber(ctx, obj_num);
  set_number(string_buffer, (double)num);
  return fe_bool(ctx, 0); // nil
}
/* (getstr name) get string variable, return string if success, nil if failed */
static fe_Object *f_getstr(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj_name = fe_nextarg(ctx, &arg);
  // name
  fe_tostring(ctx, obj_name, string_buffer, SMALL_STRING_BUF_SIZE);
  // buf
  int len = get_string_length(string_buffer);
  if (len < 0) {
    return fe_bool(ctx, 0); // nil
  }
  len = get_string(string_buffer, string_pool, STRING_POOL_BUF_SIZE);
  // to string
  fe_Object *obj_text = fe_string(ctx, string_pool);
  return obj_text;
}
/* (getnum name) get number variable, return number if success, nil if failed */
static fe_Object *f_getnum(fe_Context *ctx, fe_Object *arg) {
  fe_Object *obj_name = fe_nextarg(ctx, &arg);
  // name
  fe_tostring(ctx, obj_name, string_buffer, SMALL_STRING_BUF_SIZE);
  // get
  double val = get_number(string_buffer);
  if (isnan(val)) {
    return fe_bool(ctx, 0); // nil
  }
  return fe_number(ctx, (fe_Number)val);
}

void register_extended_functions(fe_Context *ctx) {
  int gc = fe_savegc(ctx);
  // extended functions
  fe_set(ctx, fe_symbol(ctx, ">"), fe_cfunc(ctx, f_gt));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, ">="), fe_cfunc(ctx, f_gte));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "type"), fe_cfunc(ctx, f_type));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "stacksize"), fe_cfunc(ctx, f_stacksize));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "dumpglobal"), fe_cfunc(ctx, f_dumpglobal));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "memfree"), fe_cfunc(ctx, f_memfree));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "strlen"), fe_cfunc(ctx, f_strlen));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "strappend"), fe_cfunc(ctx, f_strappend));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "strcat"), fe_cfunc(ctx, f_strcat));
  fe_restoregc(ctx, gc);
  // application functions
  fe_set(ctx, fe_symbol(ctx, "vfopen"), fe_cfunc(ctx, f_vfopen));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "vfclose"), fe_cfunc(ctx, f_vfclose));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "vfgetc"), fe_cfunc(ctx, f_vfgetc));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "setstr"), fe_cfunc(ctx, f_setstr));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "setnum"), fe_cfunc(ctx, f_setnum));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "getstr"), fe_cfunc(ctx, f_getstr));
  fe_restoregc(ctx, gc);
  fe_set(ctx, fe_symbol(ctx, "getnum"), fe_cfunc(ctx, f_getnum));
  fe_restoregc(ctx, gc);
}
