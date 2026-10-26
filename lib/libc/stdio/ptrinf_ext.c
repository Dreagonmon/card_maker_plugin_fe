#include <wasmenv.h>

void putchar_(char c) {
    wasmenv_putc(c);
}

#include <stdint.h>
#include <string.h>

// 假设 long double 为 IEEE 754 128位四精度，double 为 64位双精度
double __trunctfdf2(long double a) {
    // 1. 提取 long double 的位模式 (128位)
    uint64_t lo, hi;
    memcpy(&lo, (uint8_t*)&a, 8);
    memcpy(&hi, (uint8_t*)&a + 8, 8);

    uint64_t sign = (hi >> 63) & 1;
    int32_t exp = (int32_t)((hi >> 48) & 0x7FFF); // 15位指数
    uint64_t mant_hi = hi & 0x0000FFFFFFFFFFFF;    // 48位尾数高位
    uint64_t mant_lo = lo;                         // 64位尾数低位

    // 2. 处理特殊情况
    if (exp == 0x7FFF) { // Inf 或 NaN
        uint64_t d_sign = sign << 63;
        uint64_t d_exp  = 0x7FFULL << 52;
        if (mant_hi == 0 && mant_lo == 0) { // Infinity
            uint64_t result = d_sign | d_exp;
            return *(double*)&result;
        } else { // NaN，置 quiet 位
            uint64_t result = d_sign | d_exp | (1ULL << 51);
            return *(double*)&result;
        }
    }

    // 3. 处理零和次正规数
    if (exp == 0) {
        // long double 的次正规数太小，转为 double 时通常舍入为 0
        uint64_t result = sign << 63;
        return *(double*)&result;
    }

    // 4. 转换规格化数
    // long double 指数偏置为 16383，double 为 1023
    int32_t e = exp - 16383; // 无偏指数

    // 处理溢出
    if (e > 1023) {
        uint64_t result = (sign << 63) | (0x7FFULL << 52); // Inf
        return *(double*)&result;
    }
    // 处理下溢（转为次正规数或 0）
    if (e < -1022) {
        // 简化处理：直接舍入为 0
        uint64_t result = sign << 63;
        return *(double*)&result;
    }

    // 规格化 double：无偏指数范围 [-1022, 1023]
    uint64_t d_sign = sign << 63;
    uint64_t d_exp  = (uint64_t)(e + 1023) << 52;

    // long double 尾数有 112 位（显式 112 位），double 尾数有 52 位
    // 需要从 112 位中截取高 52 位，并进行舍入
    uint64_t d_mant = (mant_hi >> 4) & 0x000FFFFFFFFFFFFF; // 取高 52 位（近似）

    // 简化的舍入：看被截掉的低位
    uint64_t round_bits = (mant_hi & 0xF) << 60 | mant_lo;
    if (round_bits > (1ULL << 63)) { // 大于一半
        d_mant++;
        if (d_mant == 0) { // 尾数溢出，指数进位
            d_exp += 1ULL << 52;
        }
    } // 忽略等于一半的 ties-to-even 情况

    uint64_t result = d_sign | d_exp | d_mant;
    return *(double*)&result;
}