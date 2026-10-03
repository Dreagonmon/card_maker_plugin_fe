#ifndef __FE_EXT_H
#define __FE_EXT_H

#include "fe.h"
#include "app.h"

fe_Object *fe_readbuf(fe_Context *ctx, const char *src, size_t len);
fe_Object *fe_readstr(fe_Context *ctx, const char *src);
fe_Object *fe_readvfp(fe_Context *ctx, VFile vfp);

void fe_dump_global_symbles(fe_Context *ctx);
void fe_clean_global_nil_symbles(fe_Context *ctx);
unsigned int count_free_pair(fe_Context *ctx);
void fe_collect_garbage(fe_Context *ctx);
void register_extended_functions(fe_Context *ctx);

#endif