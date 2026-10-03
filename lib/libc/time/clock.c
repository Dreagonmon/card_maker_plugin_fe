#include <wasmenv.h>
#include <time.h>

long long clock(void)
{
    double s = wasmenv_clock_ms() / 1000.0;
    return (long long) (s * CLOCKS_PER_SEC);
}
