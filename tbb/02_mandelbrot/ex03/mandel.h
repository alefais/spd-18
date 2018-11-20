/*
 * Alessandra Fais
 * SPD course - MCSN - University of Pisa
 * A.A. 2017/18
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/task_scheduler_init.h"


using namespace tbb;
using namespace std;

class MandelBrot {
public:
    MandelBrot(const size_t& w, const size_t& h, const int& mi) :
                matrix(w, h), max_iter(mi), min(-2.0), max(2.0) { }

    long double* get_matrix() {
        return matrix.get();
    }

    void compute(int mode) {

        if (mode == 0) {

            tbb::task_scheduler_init init(1);  // Explicit number of threads

            std::vector<tbb_task> tasks;
            for (size_t x = 0; x < matrix.n_cols(); ++x) {
                for (size_t y = 0; y < matrix.n_rows(); ++y) {
                    tasks.push_back(tbb_task(x, y, matrix, min, max, max_iter));
                }
            }

            tbb::parallel_for(
                    tbb::blocked_range<size_t>(0, tasks.size()),
                    [&tasks](const tbb::blocked_range<size_t>& r) {
                        for (size_t i = r.begin(); i < r.end(); ++i)
                            tasks[i]();
                    });

        } else {
            tbb::parallel_for(
                    tbb::blocked_range<size_t >(0, matrix.n_cols()),
                    [&](const tbb::blocked_range<size_t>& r) {

                        for(size_t x = r.begin(); x != r.end(); ++x) {      // x = a
                            for (size_t y = 0; y < matrix.n_rows(); ++y) {  // y = b

                                long double a = x * (max - min) / matrix.n_cols() + min;
                                long double b = y * (max - min) / matrix.n_rows() + min;

                                matrix[x * y] = comp_pixel(a, b);   // Number of iterations required for (x, y).
                            }
                        }
                    }
            );
        }
    }

    void print_matrix() {
        std::stringstream ss;
        for (size_t x = 0; x < matrix.n_cols(); ++x) {
            ss << "| ";
            for (size_t y = 0; y < matrix.n_rows(); ++y) {
                ss << matrix[x * y] << " ";
            }
            ss << "|\n";
        }
        cout << "Matrix:\n" << ss.str();
    }

private:

    // --------------------------------------- Subclasses ---------------------------------------

    /*
     * Matrix defined as array of doubles.
     */
    class Matrix {
    public:

        Matrix(const size_t& _w, const size_t& _h) : w(_w), h(_h) {

            m = new long double[w * h];
            for (int i = 0; i < w * h; ++i)
                m[i] = 0;
        }

        Matrix(Matrix& matrix) {
            m = matrix.get();
        }

        ~Matrix() {
            if (m != nullptr)
                delete [] m;
        }

        long double* get() {
            return m;
        }

        size_t n_rows() {
            return h;
        }

        size_t n_cols() {
            return w;
        }

        const long double& operator [] (const size_t& i) const {
            return m[i];
        }

        long double& operator [] (const size_t& i) {
            return m[i];
        }

    private:
        long double* m;
        size_t w, h;
    };

    // --------------------------------------- Variables ----------------------------------------

    Matrix matrix;

    int max_iter;
    long double min;
    long double max;

    // ------------------------------------ Auxiliary Methods -----------------------------------

    /*
     * Computation of a single pixel.
     */
    int comp_pixel(long double a, long double b) {

        int count = 0;              // Number of iterations (determines the color of the pixel).

        long double c_real = a;     // Coordinates of the point in the complex plane.
        long double c_imag = b;

        // Iterate until max_iter number if iterations is reached or the computation diverges.
        for (int i = 0; i < max_iter; ++i) {

            long double temp_a = a * a - b * b;
            long double temp_b = 2 * a * b;
            a = temp_a + c_real;
            b = temp_b + c_imag;

            if (a * a + b * b > 4) break;
            count++;
        }

        return count;
    }

    struct tbb_task {
        tbb_task(size_t _x, size_t _y, Matrix& m, long double _min, long double _max, int mi) :
            x(_x), y(_y), matrix(m), min(_min), max(_max), max_iter(mi) { }

        void operator()() {
            long double a = x * (max - min) / matrix.n_cols() + min;
            long double b = y * (max - min) / matrix.n_rows() + min;

            int count = 0;
            long double c_real = a;
            long double c_imag = b;
            for (int i = 0; i < max_iter; ++i) {
                long double temp_a = a * a - b * b;
                long double temp_b = 2 * a * b;
                a = temp_a + c_real;
                b = temp_b + c_imag;

                if (a * a + b * b > 4) break;
                count++;
            }
            matrix[x * y] = count;   // Number of iterations required for (x, y).
        }

        size_t x, y;
        Matrix& matrix;
        long double min, max;
        int max_iter;
    };

};