#include <stdio.h>
#include <unistd.h>

int main()
{
    long clock_ticks = sysconf(_SC_CLK_TCK);
    double ms_per_tick = 1000.0 / clock_ticks;
    printf("Clock resolution: %.2f milliseconds\n", ms_per_tick);
    return 0;
}
