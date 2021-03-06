#include <float.h>
#include <math.h>
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


// Xoroshiro128+
static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

uint64_t rand64() {
	static uint64_t s[2] = { 0x853c49e6748fea9bull, 0xda3e39cb94b95bdbull };
	const uint64_t s0 = s[0];
	uint64_t s1 = s[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
	s[1] = rotl(s1, 36); // c

	return result;
}


double rand_gaussian(double mean, double variance)
{
    static const double epsilon = DBL_MIN;
    static const double two_pi = 2.0 * 3.14159265358979323846;

    static double z1 = 0.0;
    static int generate = 0;
    generate = 1 - generate;

    if (!generate) {
       return z1 * variance + mean;
    }

    double u1, u2;
    do {
       u1 = fastrand2() * (1.0 / 0xfffffffful);
       u2 = fastrand2() * (1.0 / 0xfffffffful);
    }
    while (u1 <= epsilon);

    double z0;
    z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
    z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
    return z0 * variance + mean;
}
