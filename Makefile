# required packages: clang, llvm, wabt
# todo: using binaryen

BUILD = build
LIB_BUILD = build/lib
LIBC_A = $(LIB_BUILD)/libc.a
TARGET_NAME = card_maker_plugin_wren
TARGET_WAT = $(BUILD)/$(TARGET_NAME).wat
TARGET_WASM = $(BUILD)/$(TARGET_NAME).wasm

CC = clang
LD = wasm-ld
AR = llvm-ar
WASM2WAT = wasm2wat
SIZE = llvm-size
ECHO = echo
RM_F = rm -f
MAKE = make
DENO = deno

# include header
CFLAGS += -Ilib/fe/src
CFLAGS += -Ilib/openlibm/include
CFLAGS += -Ilib/env
CFLAGS += -Ilib/libc
CFLAGS += -Isrc

# source
RUNNER_SRC += $(wildcard scripts/*.js)
RUNNER_SRC += $(wildcard scripts/*.ts)
RUNNER_SRC += $(wildcard scripts/*.html)
# LIB_SRC += $(wildcard lib/fe/src/*.c) # imported in the source code.
LIB_SRC += $(wildcard lib/env/*.c)
LIB_SRC += $(wildcard lib/libc/*.c)
LIB_SRC += $(wildcard lib/libc/ctype/*.c)
LIB_SRC += $(wildcard lib/libc/errno/*.c)
LIB_SRC += $(wildcard lib/libc/string/*.c)
LIB_SRC += $(wildcard lib/libc/stdlib/*.c)
LIB_SRC += $(wildcard lib/libc/stdio/*.c)
LIB_SRC += $(wildcard lib/libc/time/*.c)
LIB_OBJ := $(LIB_SRC:%.c=$(LIB_BUILD)/%.o)
SRC += $(wildcard src/*.c)
OBJ := $(SRC:%.c=$(BUILD)/%.o)

# source definition
CFLAGS += -DWREN_OPT_RANDOM=0
CFLAGS += -DWREN_OPT_META=0
CFLAGS += -DPRINTF_ALIAS_STANDARD_FUNCTION_NAMES_HARD

# target and features
CFLAGS += --target=wasm32
CFLAGS += -std=gnu17 -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable
CFLAGS += -nostdlib
CFLAGS += -ffreestanding
CFLAGS += -fvisibility=hidden
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -flto=full
CFLAGS += -foptimize-sibling-calls
# wasm3 support features
# CFLAGS += -mmutable-globals
# CFLAGS += -mnontrapping-fptoint
# CFLAGS += -msign-ext
# CFLAGS += -mmultivalue
CFLAGS += -mbulk-memory

# optimization
ifdef DEBUG
CFLAGS += -O1 -g3
CFLAGS += -DDEBUG
else
# O1 will cause runtime
CFLAGS += -Os
LFLAGS += --compress-relocations
endif

# link
LFLAGS += --no-entry
LFLAGS += --strip-all
LFLAGS += --gc-sections
LFLAGS += --lto-O3
# stack and memory size
LFLAGS += --export-memory=memory
LFLAGS += --stack-first
LFLAGS += -z stack-size=262144
LFLAGS += --initial-memory=4194304 # 4M
# LFLAGS += --import-memory=env,memory # import memory
# LFLAGS += --initial-memory=1048576 # auto generated
# LFLAGS += --max-memory=1048576 # no limit


all: $(TARGET_WASM) $(TARGET_WAT)

lib/openlibm/libopenlibm.a:
	cd lib/openlibm && $(MAKE) USECLANG=1 ARCH=wasm32
	cd ../../

$(TARGET_WASM): $(OBJ) $(LIBC_A) lib/openlibm/libopenlibm.a
	@$(ECHO) Linking...
	@$(LD) $^ $(LFLAGS) -o $@
	@$(SIZE) $(TARGET_WASM)
	@$(ECHO) done.

$(TARGET_WAT): $(TARGET_WASM)
	@$(WASM2WAT) -o $(TARGET_WAT) $(TARGET_WASM)

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) -c -MMD $(CFLAGS) $< -o $@

$(LIBC_A): $(LIB_OBJ)
	@$(AR) -rc $(LIBC_A) $(LIB_OBJ)

$(LIB_BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) -c -MMD $(CFLAGS) $< -o $@

clean:
	@$(ECHO) Cleaning...
	@$(RM_F) $(TARGET_WASM)
	@$(RM_F) $(TARGET_WAT)
	@$(RM_F) $(OBJ)
	@$(ECHO) done.

cleanlib:
	@$(ECHO) Cleaning library...
	@$(RM_F) $(LIB_OBJ)
	@$(RM_F) $(LIBC_A)
	@$(MAKE) -C lib/openlibm clean
	@$(ECHO) done.

wasm: $(TARGET_WASM)

wat: $(TARGET_WAT)

lib: $(LIBC_A)

runner_scripts: $(RUNNER_SRC)
	@cp ./scripts/deno_cli.ts $(BUILD)/deno_cli.ts
	@cp ./scripts/wasmenv.ts $(BUILD)/wasmenv.ts

run: $(TARGET_WASM) $(TARGET_WAT) runner_scripts
	@$(ECHO) ======== program start ========
	@$(DENO) run --allow-read=$(BUILD) --allow-write=$(BUILD) $(BUILD)/deno_cli.ts
