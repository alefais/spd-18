//------------------------------------------------------------------------------
//
// Name:       vadd.cpp
// 
// Purpose:    Elementwise addition of vectors
//
//             c = a + b; d = c + e; f = d + g
//
// Student:    Alessandra Fais - SPD course @ UniPi
//             
//------------------------------------------------------------------------------

#define __CL_ENABLE_EXCEPTIONS

#include "cl.hpp"
#include "util.hpp" // utility library
#include "err_code.h"

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

// Pick up device type from compiler command line or from the default type.
#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

//------------------------------------------------------------------------------

#define TOL    (0.001)   // tolerance used in floating point comparisons
#define LENGTH (1024)    // length of vectors

int main(void)
{
    std::vector<float> h_a(LENGTH);                // a vector 
    std::vector<float> h_b(LENGTH);                // b vector 	
    std::vector<float> h_c (LENGTH, 0xdeadbeef);   // c = a + b, from compute device

    // Additional vectors on the host.
    std::vector<float> h_d(LENGTH, 0xdeadbeef1);   // d vector, from compute device
    std::vector<float> h_e(LENGTH);                // e vector
    std::vector<float> h_f(LENGTH, 0xdeadbeef2);   // f vector, from compute device
    std::vector<float> h_g(LENGTH);                // g vector

    cl::Buffer d_a;                       // device memory used for the input a vector
    cl::Buffer d_b;                       // device memory used for the input b vector
    cl::Buffer d_c;                       // device memory used for the input/output c vector

    // Additional buffer objects.
    cl::Buffer d_d;                       // device memory used for the input/output d vector
    cl::Buffer d_e;                       // device memory used for the input e vector
    cl::Buffer d_f;                       // device memory used for the input/output f vector
    cl::Buffer d_g;                       // device memory used for the input g vector

    // Fill vectors a, b, e, g with random float values.
    int count = LENGTH;
    for(int i = 0; i < count; i++) {
        h_a[i] = rand() / (float)RAND_MAX;
        h_b[i] = rand() / (float)RAND_MAX;
        h_e[i] = rand() / (float)RAND_MAX;
        h_g[i] = rand() / (float)RAND_MAX;
    }

    try {
    	// Create a context.
        cl::Context context(DEVICE);

        // Load in kernel source, creating a program object for the context.
        cl::Program program(context, util::loadProgram("vadd.cl"), true);

        // Get the command queue.
        cl::CommandQueue queue(context);

        // Create the kernel functor.
        auto vadd = cl::make_kernel<
                cl::Buffer,
                cl::Buffer,
                cl::Buffer,
                cl::Buffer,
                cl::Buffer,
                cl::Buffer,
                cl::Buffer,
                int
                >
                (program, "vadd");

        d_a = cl::Buffer(context, begin(h_a), end(h_a), true);
        d_b = cl::Buffer(context, begin(h_b), end(h_b), true);
        d_e = cl::Buffer(context, begin(h_e), end(h_e), true);
        d_g = cl::Buffer(context, begin(h_g), end(h_g), true);

        d_c  = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * LENGTH);
        d_d  = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * LENGTH);
        d_f  = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * LENGTH);

        util::Timer timer;

        vadd(cl::EnqueueArgs(queue, cl::NDRange(count)),
                d_a,
                d_b,
                d_c,
                d_d,
                d_e,
                d_f,
                d_g,
                count);

        queue.finish();

        double rtime = static_cast<double>(timer.getTimeMicroseconds()) / 1000.0;
        printf("\nThe kernels ran in %lf milliseconds\n", rtime);

        // Copy c vector from device to host.
        cl::copy(queue, d_c, begin(h_c), end(h_c));

        // Test the results of c vector.
        int correct_c = 0;
        float tmp_c;
        for(int i = 0; i < count; i++) {
            tmp_c = h_a[i] + h_b[i];              // expected value for d_c[i]
            tmp_c -= h_c[i];                      // compute errors
            if(tmp_c * tmp_c < TOL * TOL) {       // correct if square deviation is less than tolerance squared
                correct_c++;
            }
            else {
                printf("tmp_c %f h_a %f h_b %f h_c %f \n",
                    tmp_c, h_a[i], h_b[i], h_c[i]);
            }
        }

        // Copy d vector from device to host.
        cl::copy(queue, d_d, begin(h_d), end(h_d));

        // Test the results of d vector.
        int correct_d = 0;
        float tmp_d;
        for(int i = 0; i < count; i++) {
            tmp_d = h_c[i] + h_e[i];              // expected value for d_d[i]
            tmp_d -= h_d[i];                      // compute errors
            if(tmp_d * tmp_d < TOL * TOL) {       // correct if square deviation is less than tolerance squared
                correct_d++;
            }
            else {
                printf("tmp_d %f h_c %f h_e %f h_d %f \n",
                        tmp_d, h_c[i], h_e[i], h_d[i]);
            }
        }

        // Copy f vector from device to host.
        cl::copy(queue, d_f, begin(h_f), end(h_f));

        // Test the results of f vector.
        int correct_f = 0;
        float tmp_f;
        for(int i = 0; i < count; i++) {
            tmp_f = h_d[i] + h_g[i];              // expected value for d_f[i]
            tmp_f -= h_f[i];                      // compute errors
            if(tmp_f * tmp_f < TOL * TOL) {       // correct if square deviation is less than tolerance squared
                correct_f++;
            }
            else {
                printf("tmp_f %f h_d %f h_g %f h_f %f \n",
                       tmp_f, h_d[i], h_g[i], h_f[i]);
            }
        }

        // Summarize results.
        printf("Vector add to find F = A+B+E+G:  %d out of %d results were correct.\n",
            correct_c + correct_d + correct_f,
            count * 3);
    }
    catch (cl::Error err) {
        std::cout << "Exception\n";
        std::cerr 
            << "ERROR: "
            << err.what()
            << "("
            << err_code(err.err())
           << ")"
           << std::endl;
    }
}
