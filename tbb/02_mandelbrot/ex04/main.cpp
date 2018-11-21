/*
 * Alessandra Fais
 * SPD course - MCSN - University of Pisa
 * A.A. 2017/18
 */

#include <iostream>
#include "tbb/tick_count.h"
#include "mandel.h"

int main(int argc, char* argv[]) {

    if (argc < 4) {
        std::cout << "Usage"
                  << argv[0]
                  << " width height max_iter"
                  << std::endl;
        return 1;
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    int max_iter = atoi(argv[3]);

    MandelBrot mb(width, height, max_iter);

    std::cout << "Mandelbrot parallel_for execution takes ";
    tbb::tick_count t0 = tbb::tick_count::now();

    mb.compute(1);

    tbb::tick_count t1 = tbb::tick_count::now();
    double t = (t1 - t0).seconds();
    std::cout << t << " s with " << tbb::task_scheduler_init::default_num_threads() << " threads." << std::endl;

    mb.print_table();

    std::cout << "Mandelbrot parallel_for execution takes ";
    tbb::tick_count t2 = tbb::tick_count::now();

    mb.compute(0);

    tbb::tick_count t3 = tbb::tick_count::now();
    t = (t3 - t2).seconds();
    std::cout << t << " s with 1 thread." << std::endl;

    mb.print_table();

    return 0;
}