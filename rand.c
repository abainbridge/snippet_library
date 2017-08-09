#include <stdint.h>


// From https://software.intel.com/en-us/articles/fast-random-number-generator-on-the-intel-pentiumr-4-processor
uint32_t fastrand()
{
    uint32_t g_seed = 1234;
    g_seed = 214013 * g_seed + 2531011;
    return g_seed;
}


// From http://www.pcg-random.org/
uint32_t fastrand2()
{
    static uint64_t state = 0x853c49e6748fea9bull;
    static uint64_t inc = 0xda3e39cb94b95bdbull;
    uint64_t oldstate = state;
    state = oldstate * 6364136223846793005ull + (inc | 1);
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
