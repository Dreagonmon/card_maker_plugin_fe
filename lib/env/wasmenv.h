#ifndef __WASMENV_H
#define __WASMENV_H

#include <stdbool.h>
#include <stdint.h>

#define WASM_EXPORT(name) __attribute__((export_name(name)))
#define WASM_IMPORT(name) __attribute((import_module("env"), import_name(name)))

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------
//      Libc Required Functions
// ---------------------------

WASM_IMPORT("wasmenv_clock_ms")
/** Get the current clock in milliseconds.
 * recommand to set the following CSP headers to make `performance.now` more accurate:
 * ```
 * Cross-Origin-Opener-Policy: same-origin
 * Cross-Origin-Embedder-Policy: require-corp-origin
 * ```
 * @return The current clock (not time) in milliseconds.
 */
double wasmenv_clock_ms();

WASM_IMPORT("wasmenv_time_ms")
/** Get the current time in milliseconds.
 * recommand to set the following CSP headers to make `performance.timeOrigin + performance.now` more accurate:
 * ```
 * Cross-Origin-Opener-Policy: same-origin
 * Cross-Origin-Embedder-Policy: require-corp-origin
 * ```
 * @return The current time since unix epoch in milliseconds.
 */
double wasmenv_time_ms();

WASM_IMPORT("wasmenv_abort")
/** Interrupt program and stop execution.
 */
void wasmenv_abort();

WASM_IMPORT("wasmenv_putc")
/** Print a unicode character to the Console.
 * @param codepoint The unicode codepoint to print.
 */
void wasmenv_putc(uint32_t codepoint);

#ifdef __cplusplus
}
#endif

#endif // __WASMENV_H
