#include <limits.h>

static unsigned long long
strtox(const char *s, char **p, int base, unsigned long long lim)
{
	const unsigned char *str = (const unsigned char *)s;
	const unsigned char *start = str;
	unsigned long long acc = 0;
	unsigned long long cutoff;
	unsigned long long cutlim;
	int any = 0;
	int overflow = 0;

	/* 1. 跳过前导空白字符 */
	while (*str == ' ' || *str == '\t' || *str == '\n' ||
	       *str == '\v' || *str == '\f' || *str == '\r')
		str++;

	/* 2. 可选的正负号（符号本身由调用方负责应用） */
	if (*str == '+' || *str == '-')
		str++;

	/* 3. 识别进制，并处理 "0x" 前缀 */
	if ((base == 0 || base == 16) &&
	    str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		int d = str[2];
		if ((d >= '0' && d <= '9') ||
		    (d >= 'a' && d <= 'f') ||
		    (d >= 'A' && d <= 'F')) {
			str += 2;
			base = 16;
		}
	} else if (base == 0) {
		base = (str[0] == '0') ? 8 : 10;
	}

	/* 4. 预计算溢出阈值，避免在循环中做除法 */
	cutoff = lim / (unsigned long long)base;
	cutlim = lim % (unsigned long long)base;

	/* 5. 逐位解析数字 */
	for (;; str++) {
		int c = *str;
		unsigned int d;
		if (c >= '0' && c <= '9')      d = (unsigned int)(c - '0');
		else if (c >= 'a' && c <= 'z') d = (unsigned int)(c - 'a' + 10);
		else if (c >= 'A' && c <= 'Z') d = (unsigned int)(c - 'A' + 10);
		else                           break;
		if (d >= (unsigned int)base)
			break;
		any = 1;

		if (overflow ||
		    acc > cutoff ||
		    (acc == cutoff && (unsigned long long)d > cutlim)) {
			overflow = 1;
		} else {
			acc = acc * (unsigned long long)base + (unsigned long long)d;
		}
	}

	/* 6. 设置结束指针。如果没有任何数字被成功解析，回退到原始起点 */
	if (p)
		*p = (char *)(any ? (const char *)str : (const char *)start);

	/* 7. 溢出时钳位到 lim */
	return overflow ? lim : acc;
}
unsigned long long strtoull(const char *restrict s, char **restrict p, int base)
{
	return strtox(s, p, base, ULLONG_MAX);
}

long long strtoll(const char *restrict s, char **restrict p, int base)
{
	return strtox(s, p, base, LLONG_MIN);
}

unsigned long strtoul(const char *restrict s, char **restrict p, int base)
{
	return strtox(s, p, base, ULONG_MAX);
}

long strtol(const char *restrict s, char **restrict p, int base)
{
	return strtox(s, p, base, 0UL+LONG_MIN);
}
