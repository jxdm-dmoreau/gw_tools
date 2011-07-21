#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define INFO(...) printf(__VA_ARGS__)


int main(int argc, char **argv)
{
   
    INFO("%20s | %25d | %25d | %2d bits|\n", "INTEGER", INT_MIN, INT_MAX, sizeof(int)*8);

    INFO("%20s | %25ld | %25ld | %2d bits|\n", "LONG INTEGER", LONG_MIN, LONG_MAX,
            sizeof(long int)*8);

    INFO("%20s | %25lld | %25lld | %2d bits|\n", "LONG LONG INTEGER", LLONG_MIN, LLONG_MAX,
            sizeof(long long int)*8);

    INFO("%20s | %25u | %25u | %2d bits|\n", "UNSIGNED", 0, UINT_MAX,
            sizeof(unsigned)*8);

    INFO("%20s | %25lu | %25lu | %2d bits|\n", "LONG UNSIGNED", 0L, ULONG_MAX,
            sizeof(long unsigned)*8);

    INFO("%20s | %25llu | %25llu | %2d bits|\n", "LONG LONG UNSIGNED", 0LL, ULLONG_MAX,
            sizeof(long long unsigned)*8);
    
    return EXIT_SUCCESS;
}



