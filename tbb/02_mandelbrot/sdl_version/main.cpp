/*
 * Alessandra Fais
 * SPD course - MCSN - University of Pisa
 * A.A. 2017/18
 */


/*
 * The Mandelbrot set is the set of all points c in the complex plane that do not go to infinity
 * when the quadratic function z ← z^2 + c is iterated. We can prove that once z leaves a circle
 * of radius 2 it will be guaranteed to diverge. If this happens, we can terminate the computation
 * early, so we will compute the function up to some max value K.
 *
 *      z0 = 0
 *      z(k) = z(k-1)^2 + c
 *      count(c) = min k : |z(k)| >= 2      where (0 <= k < K) and k is the number of iterations
 */

#include <iostream>

#include "graphics.h"

int main(int argc, char* argv[]) {

    std::cout << "Drawing the Mandelbrot set..." << std::endl;

    SDL_Mandelbrot mandel(200);
    int ret = mandel.draw();

    std::cout << "Closing application..." << std::endl;

    return ret;
}


