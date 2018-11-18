/*
 * Alessandra Fais
 * SPD course - MCSN - University of Pisa
 * A.A. 2017/18
 */

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

using namespace std;

/*
 * Scaled vector addition algorithm - embarassingly data parallel computation
 *
 * Compile:
 *          g++-8 -std=c++11 -Wall -Wextra -O3 main.cpp -ltbb -o main
 *          icc -std=c++11 -Wall -Wextra -O3 main.cpp -ltbb -o main
 */
void saxpy_serial(size_t vec_size, float scale, const vector<float>& v_in, vector<float>& v_out) {
    for (size_t i = 0; i < vec_size; ++i) {
        v_out.at(i) = scale * v_in.at(i) + v_out.at(i);
    }
}

void saxpy_tbb(size_t vec_size, float scale, const vector<float>& v_in, vector<float>& v_out) {
    tbb::parallel_for(  // TBB tiling (sub-ranges processed into separated tasks).
        tbb::blocked_range<size_t >(0, vec_size),
        [&](const tbb::blocked_range<size_t>& r) {
            for (size_t i = r.begin(); i != r.end(); ++i) {
                v_out.at(i) = scale * v_in.at(i) + v_out.at(i);
            }
        }
    );
}

int main() {
    cout << "Starting map computation..." << endl;

    vector<float> v1, v2, v3;
    size_t size = 1000000000;
    float scale_factor = 2.0;

    for (size_t i = 0; i < size; ++i) {
        v1.push_back(i * scale_factor);
        v2.push_back(0);
        v3.push_back(0);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    saxpy_serial(size, scale_factor, ref(v1), ref(v2));
    auto t2 = std::chrono::high_resolution_clock::now();
    auto time1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    auto t3 = std::chrono::high_resolution_clock::now();
    saxpy_tbb(size, scale_factor, ref(v1), ref(v3));
    auto t4 = std::chrono::high_resolution_clock::now();
    auto time2 = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t4).count();

    /*
     * Print vector content

    stringstream ss;
    ss << "input vector: ";
    for (const auto& el : v1) {
        ss << el << " ";
    }
    ss << "\noutput vector seq: ";
    for (const auto& el : v2) {
        ss << el << " ";
    }
    ss << "\noutput vector tbb: ";
    for (const auto& el : v3) {
        ss << el << " ";
    }
    cout << "Results are:\n" << ss.str() << endl;

    */

    cout << "Compute map on " << size
         << " elements. Sequential time is " << time1
         << " ms, tbb parallel time is " << time2
         << " ms" << endl;

    return 0;
}