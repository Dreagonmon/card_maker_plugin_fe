
#ifndef _TIME_H
#define _TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCKS_PER_SEC  ((long long) 1000000)

long long clock(void);

#ifdef __cplusplus
}
#endif

#endif
