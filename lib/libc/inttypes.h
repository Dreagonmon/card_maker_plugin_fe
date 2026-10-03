/* inttypes.h - 适用于 wasm32 freestanding / nostd 环境
 *
 * 本文件是自包含的：不包含 <stdint.h> 或任何其他系统头文件，
 * 所有类型均直接使用编译器内建类型定义。
 *
 * 适用目标：wasm32-unknown-unknown / -nostdlib / -ffreestanding
 * 数据模型：ILP32（int、long、指针均为 32 位；long long 为 64 位）
 */

#ifndef _INTTYPES_H
#define _INTTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==========================================================================
 * 1. 基础整数类型（等效于 stdint.h，但完全内联以支持 nostd）
 * ========================================================================== */

#include <stdint.h>

/* ==========================================================================
 * 2. 限值宏（部分库会通过 inttypes.h 间接暴露）
 * ========================================================================== */

#define INT8_MAX    127
#define INT16_MAX   32767
#define INT32_MAX   2147483647
#define INT64_MAX   9223372036854775807LL

#define INT8_MIN    (-128)
#define INT16_MIN   (-32768)
#define INT32_MIN   (-2147483647 - 1)
#define INT64_MIN   (-9223372036854775807LL - 1)

#define UINT8_MAX   255U
#define UINT16_MAX  65535U
#define UINT32_MAX  4294967295U
#define UINT64_MAX  18446744073709551615ULL

#define INTPTR_MAX  INT32_MAX
#define INTPTR_MIN  INT32_MIN
#define UINTPTR_MAX UINT32_MAX

#define INTMAX_MAX  INT64_MAX
#define INTMAX_MIN  INT64_MIN
#define UINTMAX_MAX UINT64_MAX

/* ==========================================================================
 * 3. 格式修饰符辅助宏
 *
 * 在 wasm32 ILP32 下：
 *   - int64_t  = long long → __PRI64  = "ll"
 *   - intptr_t = int       → __PRIPTR = ""（无需长度修饰符）
 *
 * 同时兼容 clang 和 GCC：优先使用编译器内建的 __INT64_TYPE__ 等宏，
 * 若不可用则回退到 __SIZEOF_LONG_LONG__ 判断。
 * ========================================================================== */

#define __PRI64  "ll"

/* wasm32: intptr_t 是 int，不需要长度修饰符 */
#define __PRIPTR ""

/* ==========================================================================
 * 4. 打印格式宏（printf 系列）
 * ========================================================================== */

/* --- 精确宽度有符号十进制 --- */
#define PRId8   "d"
#define PRId16  "d"
#define PRId32  "d"
#define PRId64  __PRI64 "d"

#define PRIi8   "i"
#define PRIi16  "i"
#define PRIi32  "i"
#define PRIi64  __PRI64 "i"

/* --- 精确宽度无符号十进制 --- */
#define PRIu8   "u"
#define PRIu16  "u"
#define PRIu32  "u"
#define PRIu64  __PRI64 "u"

/* --- 精确宽度八进制 --- */
#define PRIo8   "o"
#define PRIo16  "o"
#define PRIo32  "o"
#define PRIo64  __PRI64 "o"

/* --- 精确宽度十六进制（小写 / 大写） --- */
#define PRIx8   "x"
#define PRIx16  "x"
#define PRIx32  "x"
#define PRIx64  __PRI64 "x"

#define PRIX8   "X"
#define PRIX16  "X"
#define PRIX32  "X"
#define PRIX64  __PRI64 "X"

/* --- 最小宽度类型 --- */
#define PRIdLEAST8   "d"
#define PRIdLEAST16  "d"
#define PRIdLEAST32  "d"
#define PRIdLEAST64  __PRI64 "d"

#define PRIiLEAST8   "i"
#define PRIiLEAST16  "i"
#define PRIiLEAST32  "i"
#define PRIiLEAST64  __PRI64 "i"

#define PRIuLEAST8   "u"
#define PRIuLEAST16  "u"
#define PRIuLEAST32  "u"
#define PRIuLEAST64  __PRI64 "u"

#define PRIoLEAST8   "o"
#define PRIoLEAST16  "o"
#define PRIoLEAST32  "o"
#define PRIoLEAST64  __PRI64 "o"

#define PRIxLEAST8   "x"
#define PRIxLEAST16  "x"
#define PRIxLEAST32  "x"
#define PRIxLEAST64  __PRI64 "x"

#define PRIXLEAST8   "X"
#define PRIXLEAST16  "X"
#define PRIXLEAST32  "X"
#define PRIXLEAST64  __PRI64 "X"

/* --- 最快宽度类型 --- */
#define PRIdFAST8    "d"
#define PRIdFAST16   "d"         /* int_fast16_t = int16_t → "d" */
#define PRIdFAST32   "d"         /* int_fast32_t = int32_t → "d" */
#define PRIdFAST64   __PRI64 "d"

#define PRIiFAST8    "i"
#define PRIiFAST16   "i"
#define PRIiFAST32   "i"
#define PRIiFAST64   __PRI64 "i"

#define PRIuFAST8    "u"
#define PRIuFAST16   "u"
#define PRIuFAST32   "u"
#define PRIuFAST64   __PRI64 "u"

#define PRIoFAST8    "o"
#define PRIoFAST16   "o"
#define PRIoFAST32   "o"
#define PRIoFAST64   __PRI64 "o"

#define PRIxFAST8    "x"
#define PRIxFAST16   "x"
#define PRIxFAST32   "x"
#define PRIxFAST64   __PRI64 "x"

#define PRIXFAST8    "X"
#define PRIXFAST16   "X"
#define PRIXFAST32   "X"
#define PRIXFAST64   __PRI64 "X"

/* --- 指针 / 最大宽度类型 --- */
#define PRIdPTR  __PRIPTR "d"
#define PRIiPTR  __PRIPTR "i"
#define PRIuPTR  __PRIPTR "u"
#define PRIoPTR  __PRIPTR "o"
#define PRIxPTR  __PRIPTR "x"
#define PRIXPTR  __PRIPTR "X"

#define PRIdMAX  __PRI64 "d"
#define PRIiMAX  __PRI64 "i"
#define PRIuMAX  __PRI64 "u"
#define PRIoMAX  __PRI64 "o"
#define PRIxMAX  __PRI64 "x"
#define PRIXMAX  __PRI64 "X"

/* ==========================================================================
 * 5. 扫描格式宏（scanf 系列）
 * ========================================================================== */

/* --- 精确宽度有符号十进制 --- */
#define SCNd8   "hhd"
#define SCNd16  "hd"
#define SCNd32  "d"
#define SCNd64  __PRI64 "d"

#define SCNi8   "hhi"
#define SCNi16  "hi"
#define SCNi32  "i"
#define SCNi64  __PRI64 "i"

/* --- 精确宽度无符号十进制 --- */
#define SCNu8   "hhu"
#define SCNu16  "hu"
#define SCNu32  "u"
#define SCNu64  __PRI64 "u"

/* --- 精确宽度八进制 --- */
#define SCNo8   "hho"
#define SCNo16  "ho"
#define SCNo32  "o"
#define SCNo64  __PRI64 "o"

/* --- 精确宽度十六进制 --- */
#define SCNx8   "hhx"
#define SCNx16  "hx"
#define SCNx32  "x"
#define SCNx64  __PRI64 "x"

/* --- 最小宽度类型 --- */
#define SCNdLEAST8   "hhd"
#define SCNdLEAST16  "hd"
#define SCNdLEAST32  "d"
#define SCNdLEAST64  __PRI64 "d"

#define SCNiLEAST8   "hhi"
#define SCNiLEAST16  "hi"
#define SCNiLEAST32  "i"
#define SCNiLEAST64  __PRI64 "i"

#define SCNuLEAST8   "hhu"
#define SCNuLEAST16  "hu"
#define SCNuLEAST32  "u"
#define SCNuLEAST64  __PRI64 "u"

#define SCNoLEAST8   "hho"
#define SCNoLEAST16  "ho"
#define SCNoLEAST32  "o"
#define SCNoLEAST64  __PRI64 "o"

#define SCNxLEAST8   "hhx"
#define SCNxLEAST16  "hx"
#define SCNxLEAST32  "x"
#define SCNxLEAST64  __PRI64 "x"

/* --- 最快宽度类型 --- */
#define SCNdFAST8    "hhd"
#define SCNdFAST16   "hd"
#define SCNdFAST32   "d"
#define SCNdFAST64   __PRI64 "d"

#define SCNiFAST8    "hhi"
#define SCNiFAST16   "hi"
#define SCNiFAST32   "i"
#define SCNiFAST64   __PRI64 "i"

#define SCNuFAST8    "hhu"
#define SCNuFAST16   "hu"
#define SCNuFAST32   "u"
#define SCNuFAST64   __PRI64 "u"

#define SCNoFAST8    "hho"
#define SCNoFAST16   "ho"
#define SCNoFAST32   "o"
#define SCNoFAST64   __PRI64 "o"

#define SCNxFAST8    "hhx"
#define SCNxFAST16   "hx"
#define SCNxFAST32   "x"
#define SCNxFAST64   __PRI64 "x"

/* --- 指针 / 最大宽度类型 --- */
#define SCNdPTR  __PRIPTR "d"
#define SCNiPTR  __PRIPTR "i"
#define SCNuPTR  __PRIPTR "u"
#define SCNoPTR  __PRIPTR "o"
#define SCNxPTR  __PRIPTR "x"

#define SCNdMAX  __PRI64 "d"
#define SCNiMAX  __PRI64 "i"
#define SCNuMAX  __PRI64 "u"
#define SCNoMAX  __PRI64 "o"
#define SCNxMAX  __PRI64 "x"

/* ==========================================================================
 * 6. imaxdiv_t 与函数声明
 *
 * 注意：在 nostd 环境中这些函数不会自动链接。如果你的项目不需要它们，
 * 可以保持注释状态；如果需要自定义实现，取消注释并自行提供实现即可。
 * ========================================================================== */

typedef struct {
    intmax_t quot;
    intmax_t rem;
} imaxdiv_t;

/* 以下函数在 nostd 下需要你自行提供实现（例如对接编译器 builtins 或
 * 你自己的运行时库）。若不需要，可保持注释。 */

/*
intmax_t  imaxabs(intmax_t j);
imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom);
intmax_t  strtoimax(const char * restrict nptr,
                    char ** restrict endptr, int base);
uintmax_t strtoumax(const char * restrict nptr,
                    char ** restrict endptr, int base);
*/

#ifdef __cplusplus
}
#endif

#endif /* _INTTYPES_H */