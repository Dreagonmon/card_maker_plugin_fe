// TODO: simple in-memory filesystem
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <wasmenv.h>

#define MODE_BIT_WRITE (1 << 0)
#define MODE_BIT_EXTEND (1 << 1)

// typedef struct FileItem {
//     FILE file;
//     struct FileItem *next;
// } FileItem;

// FileItem *fs = NULL;

// FILE* _find_file_by_name(const char* filename) {
//     FileItem *item = fs;
//     while (item != NULL) {
//         if (strcmp(item->file.name, filename) == 0) {
//             return &(item->file);
//         }
//         item = item->next;
//     }
//     return NULL;
// }

FILE* fopen(const char* filename, const char* mode) {
    return NULL;
}

int fgetc( FILE* stream ) {
    return EOF;
}

int fputc( int ch, FILE* stream ) {
    if (stream == stdin) {
        errno = EBADF;
        return -EBADF;
    } else if (stream == stdout || stream == stderr) {
        // printf
        wasmenv_putc((unsigned char)(ch & 0xFF));
        return ch;
    } else {
        // this is file stream
        return -EBADF;
    }
}

int fprintf(FILE* stream, const char* format, ...) {
    if (stream == stdin) {
        errno = EBADF;
        return -EBADF;
    } else if (stream == stdout || stream == stderr) {
        // printf
        va_list ap;
        va_start(ap, format);
        int ret = vprintf(format, ap);
        va_end(ap);
        return ret;
    } else {
        // this is file stream
        errno = EBADF;
        return -EBADF;
    }
}

