#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <time.h>
#include "newSleep.h"

void game_sleep(float seconds)
{
    struct timespec request;
    struct timespec remaining;
    int whole_seconds;

    if (seconds <= 0.0f) {
        return;
    }

    whole_seconds = (int)seconds;
    request.tv_sec = (time_t)whole_seconds;
    request.tv_nsec = (long)((seconds - (float)whole_seconds) * 1000000000.0f);

    while (nanosleep(&request, &remaining) == -1 && errno == EINTR) {
        request = remaining;
    }
}
