/*
 * Alessandra Fais
 * SPD course - MCSN - University of Pisa
 * A.A. 2017/18
 */

#include <iostream>

#include "mandel.h"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cout << "Usage"
                  << argv[0]
                  << " par_deg      (note: 0 sequential execution, >0 tbb::parallel_for execution)\n"
                  << "optional arguments: width height max_iter\n";
        return 1;
    }

    int nw = (argv[1][0] == '0') ? 0 : 1;

    int width = (argc == 5) ? atoi(argv[2]) : 800;
    int height = (argc == 5) ? atoi(argv[3]) : 800;
    int max_iter = (argc == 5) ? atoi(argv[4]) : 200;

    MandelBrot mb(width, height, nw, max_iter);

    mb.compute();

    return 0;
}