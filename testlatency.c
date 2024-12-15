/*----------------------------------------------------------------------------*\
 *
 * testlatency.c
 * lucas@pamorana.net
 *
 * Test latency of signal delivery.
 *
\*----------------------------------------------------------------------------*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

volatile int recv = 0;

struct timespec beg, end;

void handler(int sig)
{
    clock_gettime(CLOCK_MONOTONIC, &end);
    recv = 1;
}

int main(void)
{
    long latency;
    signal(SIGUSR1, handler);
    clock_gettime(CLOCK_MONOTONIC, &beg);
    kill(getpid(), SIGUSR1);
    while (!recv); // spinlock to get highest possible precision
    latency \
        = (end.tv_sec  - beg.tv_sec) * 1000000000
        + (end.tv_nsec - beg.tv_nsec)
        ;
    printf("Latency: %ld ns\n", latency);
    return EXIT_SUCCESS;
}
