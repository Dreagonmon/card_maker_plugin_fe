#ifndef __APP_H
#define __APP_H

#include <stdbool.h>
#include <stdint.h>

#ifndef WASM_EXPORT
#define WASM_EXPORT(name) __attribute__((export_name(name)))
#endif
#ifndef WASM_IMPORT
#define WASM_IMPORT(name) __attribute((import_module("env"), import_name(name)))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef const void * VFile;

// ---------------------------
//      Application Functions
// ---------------------------

WASM_IMPORT("vfile_open")
/** Open a virtual file, text mode, readonly.
 * @param path filepath, \0 ended string.
 * @return Virtual file pointer, or NULL if failed.
 */
VFile vfile_open(const char *path);

WASM_IMPORT("vfile_close")
/** Close a virtual file.
 * @param vfp Virtual file pointer.
 */
void vfile_close(VFile vfp);

WASM_IMPORT("vfile_getc")
/** Read a char(byte) ftom a virtual file.
 * @param vfp Virtual file pointer.
 * @return char, or -1 if EOF.
 */
int vfile_getc(VFile vfp);

WASM_IMPORT("set_string")
/** Set the string result.
 * @param name variable name, \0 ended string.
 * @param text variable value, \0 ended string.
 */
void set_string(const char *name, const char *text);

WASM_IMPORT("set_number")
/** Set the number result.
 * @param name variable name, \0 ended string.
 * @param value variable value, double number.
 */
void set_number(const char *name, double value);

WASM_IMPORT("get_string_length")
/** Set the string result.
 * @param name variable name, \0 ended string.
 * @return string length, ending \0 doesn't count. -1 if name not found.
 */
int32_t get_string_length(const char *name);

WASM_IMPORT("get_string")
/** Set the string result.
 * @param name variable name, \0 ended string.
 * @param buf char buf, \0 ended string.
 * @param buf_len char buf length.
 * @return written length, ending \0 doesn't count. -1 if name not found.
 */
int32_t get_string(const char *name, char *buf, int32_t buf_len);

WASM_IMPORT("get_number")
/** Set the string result.
 * @param name variable name, \0 ended string.
 * @return value, double number. NaN if name not found.
 */
double get_number(const char *name);

#ifdef __cplusplus
}
#endif

#endif // __APP_H
