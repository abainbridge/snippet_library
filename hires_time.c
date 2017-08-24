// Platform headers
#include <sys/time.h>

// Standard headers
#include <stdint.h>
#include <stdlib.h>


double gettime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)((int64_t)tv.tv_sec * 1000000 + tv.tv_usec) / 1000000.;
}
