#ifndef _STDIO_H
#define _STDIO_H

#include <stdint.h>
#include <stdio/printf.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _IO_FILE {
  char *name;
  void *content;
  size_t fp;
  size_t buf_size;
} FILE;

#define EOF (-1)
#define FOPEN_MAX (8)
#define FILENAME_MAX (255)
#define SEEK_SET (0)
#define SEEK_CUR (1)
#define SEEK_END (2)
#define stdin ((FILE *)0)
#define stdout ((FILE *)1)
#define stderr ((FILE *)2)

FILE *fopen(const char *filename, const char *mode);
int fgetc(FILE *stream);
int fputc(int ch, FILE *stream);
int fprintf(FILE *stream, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
