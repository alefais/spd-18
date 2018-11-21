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
#include "tbb/concurrent_hash_map.h"

#include "image.h"

using namespace tbb;
using namespace std;

class MandelBrot {
public:
    MandelBrot(const size_t& w, const size_t& h, const int& mi) :
                matrix(w, h), max_iter(mi), min(-2.0), max(2.0), comp_time(-1) { }

    void compute(int mode) {
        if (mode == 0) {
            //tbb::task_scheduler_init init;  // Automatic number of threads
            tbb::task_scheduler_init init(1);  // Explicit number of threads

            std::vector<tbb_task> tasks;
            for (size_t y = 0; y < matrix.h; ++y) {
                for (size_t x = 0; x < matrix.w; ++x) {
                    tasks.push_back(tbb_task(x, y, matrix, min, max, max_iter, table));
                }
            }

            tbb::parallel_for(
                    tbb::blocked_range<size_t>(0, tasks.size()),
                    [&tasks](const tbb::blocked_range<size_t>& r) {
                        for (size_t i = r.begin(); i < r.end(); ++i)
                            tasks[i]();
                    });
            string filename("mandel_1.ppm");
            save_img(matrix.w, matrix.h, filename.c_str(), matrix.unrol(), 255);
        } else {
            tbb::parallel_for(
                    tbb::blocked_range<size_t >(0, matrix.h),
                    [&](const tbb::blocked_range<size_t>& r) {

                        for(size_t y = r.begin(); y != r.end(); ++y) {
                            for (size_t x = 0; x < matrix.w; ++x) {

                                tbb::tick_count t1 = tbb::tick_count::now();

                                long double a = x * (max - min) / matrix.w + min;
                                long double b = y * (max - min) / matrix.h + min;

                                // Number of iterations required for (x, y).
                                int iter = comp_pixel(a, b);
                                matrix.m[y][x] = iter;

                                // Update map containing the number of iterations.
                                IterTable::accessor acc;
                                if (table.insert(acc, iter))
                                    acc->second = 1;
                                else
                                    acc->second += 1;
                                acc.release();

                                if (iter == max_iter) {
                                    tbb::tick_count t2 = tbb::tick_count::now();
                                    comp_time = (t2 - t1).seconds();
                                }
                            }
                        }
                    }
            );
            string filename("mandel_n.ppm");
            save_img(matrix.w, matrix.h, filename.c_str(), matrix.unrol(), 255);
        }
    }

    void print_matrix() {
        std::stringstream ss;
        for (size_t x = 0; x < matrix.h; ++x) {
            ss << "| ";
            for (size_t y = 0; y < matrix.w; ++y) {
                ss << matrix.m[x][y] << " ";
            }
            ss << "|\n";
        }
        cout << "Matrix:\n" << ss.str();
    }

    void print_unrolled_matrix() {
        int* u_matrix = matrix.unrol();
        std::stringstream ss;
        for (size_t y = 0; y < matrix.h; ++y) {
            ss << "| ";
            for (size_t x = 0; x < matrix.w; ++x) {
                ss << u_matrix[y * matrix.w + x] << " ";
            }
            ss << "|\n";
        }
        cout << "U_matrix:\n" << ss.str();
    }

    void print_table() {
        int max_it = 0;
        int max_it_counter = 0;
        std::stringstream ss;
        for (IterTable::const_iterator it = table.begin(); it != table.end() ; ++it) {
            if (it->first > max_it) {
                max_it = it->first;
                max_it_counter = it->second;
            }
            ss << "<" << it->first << ", " << it->second << ">\n";
        }
        cout //<< "Table:\n" << ss.str()
             << "Max number of iterations is "
             << max_it
             << " and has been computed for "
             << max_it_counter
             << " times.\n";
    }

    void print_time(int max_it, int max_it_counter) {
        if (max_iter == max_it) {
            cout << "The computation time for "
                 << max_iter
                 << " iterations is estimated as "
                 << (comp_time)
                 << " s.\n";
        }
    }

private:

    // --------------------------------------- Subclasses ---------------------------------------

    /*
     * Matrix class.
     */
    class Matrix {
    public:

        Matrix(const size_t& _w, const size_t& _h) : w(_w), h(_h), m(_h) {
            for (size_t i = 0; i < h; ++i) {
                vector<int> row(w);
                m.at(i) = row;
            }
        }

        Matrix(Matrix& matrix) {
            m = matrix.m;
        }

        int* unrol() {
            int* matrix = new int [h * w];
            for (size_t y = 0; y < h; ++y) {
                for (size_t x = 0; x < w; ++x) {
                    matrix[y * w + x] = m[y][x];
                }
            }
            return matrix;
        }

        vector<vector<int>> m;
        size_t w, h;
    };

    /*
     * Table definition.
     */
    struct compare {
        static size_t hash(const int& value) {
            return (value * 17) % 13;
        }

        static bool equal(const int& v1, const int& v2) {
            return v1 == v2;
        }
    };

    typedef tbb::concurrent_hash_map<int, int, compare> IterTable;

    // --------------------------------------- Variables ----------------------------------------

    Matrix matrix;

    int max_iter;
    long double min;
    long double max;

    IterTable table;

    double comp_time;

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

    /*
     * Single TBB task.
     */
    struct tbb_task {
        tbb_task(size_t _x, size_t _y, Matrix& m, long double _min, long double _max, int mi, IterTable t) :
            x(_x), y(_y), matrix(m), min(_min), max(_max), max_iter(mi), table(t) { }

        void operator()() {
            long double a = x * (max - min) / matrix.w + min;
            long double b = y * (max - min) / matrix.h + min;

            int iter = 0;
            long double c_real = a;
            long double c_imag = b;
            for (int i = 0; i < max_iter; ++i) {
                long double temp_a = a * a - b * b;
                long double temp_b = 2 * a * b;
                a = temp_a + c_real;
                b = temp_b + c_imag;

                if (a * a + b * b > 4) break;
                iter++;
            }

            // Number of iterations required for (x, y).
            matrix.m[y][x] = iter;

            // Update map containing the number of iterations.
            IterTable::accessor acc;
            if (table.insert(acc, iter))
                acc->second = 1;
            else
                acc->second += 1;
            acc.release();
        }

        size_t x, y;
        Matrix& matrix;
        long double min, max;
        int max_iter;
        IterTable& table;
    };
};