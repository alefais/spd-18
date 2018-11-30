//------------------------------------------------------------------------------
//
//  Program: Matrix Multiplication driver
//
//  Purpose: This is a driver program to test various ways of computing
//           the product:
//
//                C  = A * B
//
//           A and B are set to constant matrices so we
//           can make a quick test of the multiplication.
//
//  Usage:   The matrices are constant matrices, square and the order is
//           set as a constant, ORDER (see mult.h).
//
//  Student: Alessandra Fais - SPD course @ UniPi
//
//------------------------------------------------------------------------------

#include "matmul.hpp"
#include "matrix_lib.hpp"
#include "util.hpp"
#include <err_code.h>
#include "device_picker.hpp"

std::string kernelsource = "__kernel void m_mul(                                    \n" \
                           "   const int N,                                         \n" \
                           "   __global float* A,                                   \n" \
                           "   __global float* B,                                   \n" \
                           "   __global float* C)                                   \n" \
                           "{                                                       \n" \
                           "    int k;                                              \n" \
                           "    float tmp;                                          \n" \
                           "    int i = get_global_id(0);                           \n" \
                           "    int j = get_global_id(1);                           \n" \
                           "    if (i < N && j < N) {                               \n" \
                           "        tmp = 0.0f;                                     \n" \
                           "        for (k = 0; k < N; k ++) {                      \n" \
                           "            tmp += A[i * N + k] * B[k * N + j];         \n" \
                           "        }                                               \n" \
                           "        C[i * N + j] = tmp;                             \n" \
                           "    }                                                   \n" \
                           "}                                                       \n" \
                           "\n";

int main(int argc, char* argv[]) {

    double start_time;      // Starting time
    double run_time;        // Timing
    util::Timer timer;

    int N = ORDER;          // A[N][N], B[N][N], C[N][N]
    int size = N * N;       // Number of elements in each matrix

    std::vector<float> h_A(size);   // Host memory for Matrix A
    std::vector<float> h_B(size);   // Host memory for Matrix B
    std::vector<float> h_C(size);   // Host memory for Matrix C

    cl::Buffer d_a, d_b, d_c;       // Matrices in device memory

    //--------------------------------------------------------------------------------
    // Create a context and queue
    //--------------------------------------------------------------------------------
    try {
        cl_uint deviceIndex = 0;
        parseArguments(argc, argv, &deviceIndex);

        // Get list of devices.
        std::vector<cl::Device> devices;
        unsigned numDevices = getDeviceList(devices);

        // Check device index in range.
        if (deviceIndex >= numDevices) {
          std::cout << "Invalid device index (try '--list')\n";
          return EXIT_FAILURE;
        }

        cl::Device device = devices[deviceIndex];

        std::string name;
        getDeviceName(device, name);
        std::cout << "\nUsing OpenCL device: " << name << "\n";

        std::vector<cl::Device> chosen_device;
        chosen_device.push_back(device);
        cl::Context context(chosen_device);
        cl::CommandQueue queue(context, device);

        //--------------------------------------------------------------------------------
        // Run sequential matmul
        //--------------------------------------------------------------------------------

        init_mat(N, h_A, h_B, h_C); // Initialize matrices.

        timer.reset();

        printf("\n===== Sequential, matrix multiplication (dot product), order %d on host CPU =====\n", N);
        for(int i = 0; i < COUNT; i++) {
            //zero_mat(N, h_C); // C matrix already initialized to 0

            start_time = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;

            seq_mat_mul_dot(N, h_A, h_B, h_C);

            run_time  = (static_cast<double>(timer.getTimeMilliseconds()) / 1000.0) - start_time;

            analyze_results(N, h_C, run_time);
        }

        //--------------------------------------------------------------------------------
        // Setup the buffers, initialize matrices, and write them into global memory.
        //--------------------------------------------------------------------------------

        // Reset A, B and C matrices (just to play it safe).
        init_mat(N, h_A, h_B, h_C);

        d_a = cl::Buffer(context, h_A.begin(), h_A.end(), true);

        d_b = cl::Buffer(context, h_B.begin(), h_B.end(), true);

        d_c = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * size);

        //--------------------------------------------------------------------------------
        // OpenCL matrix multiplication (naive).
        //--------------------------------------------------------------------------------

        timer.reset();

        // Create the compute program from the source buffer.
        cl::Program program(context, kernelsource, true);

        // Create the compute kernel from the program.
        cl::make_kernel<int, cl::Buffer, cl::Buffer, cl::Buffer> naive_m_mul(program, "m_mul");

        printf("\n===== OpenCL, matrix multiplication, C(i,j) per work item, order %d =====\n", N);

        // Do the multiplication COUNT times.
        for (int i = 0; i < COUNT; i++) {
            zero_mat(N, h_C);

            start_time = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;

            // Execute the kernel over the entire range of C matrix elements (compute a dot product for each element).
            // OpenCL runtime will figure out a local work group size (local work group size is set to NULL).
            cl::NDRange global(N, N);
            naive_m_mul(cl::EnqueueArgs(queue, global), N, d_a, d_b, d_c);

            queue.finish();

            run_time  = (static_cast<double>(timer.getTimeMilliseconds()) / 1000.0) - start_time;

            // Retrieve result from device.
            cl::copy(queue, d_c, h_C.begin(), h_C.end());

            analyze_results(N, h_C, run_time);
        }

    } catch (cl::Error err) {
        std::cout << "Exception\n";
        std::cerr << "ERROR: "
                  << err.what()
                  << "("
                  << err_code(err.err())
                  << ")"
                  << std::endl;
    }

    return EXIT_SUCCESS;
}
