#include <stdlib.h>
#include <time.h>
#include "random.h"

void init_random(void)
{
    srand((unsigned int)time(NULL));
}

int random_in_range(int low, int high)
{
    if (high <= low) {
        return low;
    }
    return low + (rand() % (high - low + 1));
}
