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
                  << " par_deg      (note: 0 sequential execution, >0 parallel execution)"
                  << std::endl;
        return 1;
    }

    int nw = (argv[1][0] == '0') ? 0 : 1;

    int width = 800;
    int height = 800;
    int max_iter = 100;

    MandelBrot mb(width, height, nw, max_iter);

    mb.compute();

    std::string filename("mandel.ppm");
    mb.save_img(filename);

    return 0;
}