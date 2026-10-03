#ifndef _STRING_H
#define _STRING_H

#include <libc_const.h>
#include <features.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memchr(const void *src, int c, size_t n);
int memcmp(const void *vl, const void *vr, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest, int c, size_t n);

char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
char *strchr(const char *s, int c);
int strcmp(const char *l, const char *r);
int strncmp(const char *_l, const char *_r, size_t n);
char *strcpy(char *d, const char *s);
char *strncpy(char *d, const char *s, size_t n);
size_t strcspn(const char *s, const char *c);
size_t strlen(const char *s);
char *strpbrk(const char *s, const char *b);
char *strrchr(const char *s, int c);
size_t strspn(const char *s, const char *c);
char *strstr(const char *h, const char *n);
char *strtok(char *s, const char *sep);
char *strsep(char **str, const char *sep);

char *__strchrnul(const char *s, int c);

unsigned long long strtoull(const char *s, char **p, int base);
long long strtoll(const char *s, char **p, int base);
unsigned long strtoul(const char *s, char **p, int base);
long strtol(const char *s, char **p, int base);
double strtod(const char* str, char** endptr);

#ifdef __cplusplus
}
#endif

#endif
