#include <stdlib.h>
#include <stdio.h>
#include <wasmenv.h>

void abort(void) {
    printf("[abort]\n");
    wasmenv_abort();
    // wasm doesn't support signals, so just trap to halt the program.
    __builtin_trap();
}

void exit(int status) {
    // just ignore the status
    printf("[exit] %d\n", status);
    wasmenv_abort();
    // wasm doesn't support signals, so just trap to halt the program.
    __builtin_trap();
}
