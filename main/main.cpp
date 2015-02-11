#include <cstdio>
#include <stdlib.h>
#include <exception>

#include "sx.h"

using namespace sx;

int main(int argc, const char *argv[]) {
    try {
        dvec_d a = {1, 2, 3};
        double q = over(std::plus<double>(), a);
        printf("sum: %f\n", sum(a));

        for(int i: iota(SIZE a)) {
            printf("%d\n", i);
        }
        return EXIT_SUCCESS;
    } catch (std::exception &e) {
        fprintf(stderr, "Exception caught: %s\n", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        fprintf(stderr, "Unknown exception caught\n");
        return EXIT_FAILURE;
    }
}