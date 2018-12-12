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
//           set as a constant, ORDER (see matmul.hpp).
//
//  Student: Alessandra Fais - SPD course @ UniPi
//
//------------------------------------------------------------------------------

#include "matmul.hpp"
#include "matrix_lib.hpp"
#include "util.hpp"
#include <err_code.h>
#include "device_picker.hpp"

typedef enum { SEQ, EL, ROW, ROW_OPT, ROW_COL_OPT, BLOCKED_OPT } comp_t;

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cout << "Usage"
                  << argv[0]
                  << " comp_type\n"
                  << "comp_type can assume the following values:\n"
                  << "- 0 sequential computation\n"
                  << "- 1 one element per work-item\n"
                  << "- 2 one row per work-item\n"
                  << "- 3 one row per work-item, A row in private memory\n"
                  << "- 4 one row per work-item, A row in private memory, B cols in local memory\n";
                  //<< "- 5 \n";
        return EXIT_FAILURE;
    }

    int arg = atoi(argv[1]);
    if (arg < 0 || arg > 4) {
        std::cout << "Choose a valid computation:\n"
                  << "comp_type can assume the following values:\n"
                  << "- 0 sequential computation\n"
                  << "- 1 one element per work-item\n"
                  << "- 2 one row per work-item\n"
                  << "- 3 one row per work-item, A row in private memory\n"
                  << "- 4 one row per work-item, A row in private memory, B cols in local memory\n";
                  //<< "- 5 \n";
        return EXIT_FAILURE;
    }

    comp_t comp = static_cast<comp_t>(arg);

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

        // ====================================================
        // TODO se ti va vedi come si sistema
        cl_device_id dev_id;
        cl_device_info dev_info;
        //clGetDeviceInfo(dev_id, dev_info);
        

        // ====================================================

        std::vector<cl::Device> chosen_device;
        chosen_device.push_back(device);
        cl::Context context(chosen_device);
        cl::CommandQueue queue(context, device);

        if (comp == SEQ) {    /** Sequential matrix multiplication */
            init_mat(N, h_A, h_B, h_C); // Initialize matrices.

            timer.reset();

            printf("\n===== Sequential, matrix multiplication (dot product), order %d on host CPU =====\n", N);
            for (int i = 0; i < COUNT; i++) { // COUNT actually set to 1
                start_time = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;

                seq_mat_mul_dot(N, h_A, h_B, h_C);  // Computation.

                run_time = (static_cast<double>(timer.getTimeMilliseconds()) / 1000.0) - start_time;

                analyze_results(N, h_C, run_time);  // Check results.
            }
        } else if (comp == EL) {    /** OpenCL matrix multiplication - Naive */
            // Setup the buffers, initialize matrices, and write them into global memory.
            init_mat(N, h_A, h_B, h_C);
            d_a = cl::Buffer(context, h_A.begin(), h_A.end(), true);
            d_b = cl::Buffer(context, h_B.begin(), h_B.end(), true);
            d_c = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * size);

            timer.reset();

            // Create the compute program from the source buffer.
            cl::Program program(context, util::loadProgram("m_mul_el.cl"), true);
            printf("\n===== OpenCL, matrix multiplication, C(i,j) per work item, order %d, single element =====\n", N);

            // Create the compute kernel from the program.
            cl::make_kernel<int, cl::Buffer, cl::Buffer, cl::Buffer> el_m_mul(program, "m_mul");

            // Do the multiplication COUNT times.
            for (int i = 0; i < COUNT; i++) { // COUNT actually set to 1
                zero_mat(N, h_C);

                start_time = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;

                // Execute the kernel over the entire range of C matrix elements (compute a dot product for each element).
                // OpenCL runtime will figure out a local work group size (local work group size is set to NULL).
                cl::NDRange global(N, N);
                el_m_mul(cl::EnqueueArgs(queue, global), N, d_a, d_b, d_c);

                queue.finish();

                run_time  = (static_cast<double>(timer.getTimeMilliseconds()) / 1000.0) - start_time;

                cl::copy(queue, d_c, h_C.begin(), h_C.end());   // Retrieve result from device.

                analyze_results(N, h_C, run_time);  // Check results.
            }
        } else if (comp == ROW) {   /** OpenCL matrix multiplication - C row per work-item */
            // Setup the buffers, initialize matrices, and write them into global memory.
            init_mat(N, h_A, h_B, h_C);
            d_a = cl::Buffer(context, h_A.begin(), h_A.end(), true);
            d_b = cl::Buffer(context, h_B.begin(), h_B.end(), true);
            d_c = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * size);

            timer.reset();

            // Create the compute program from the source buffer.
            cl::Program program(context, util::loadProgram("m_mul_row.cl"), true);
            printf("\n===== OpenCL, matrix multiplication, C(i,j) per work item, order %d, single row =====\n", N);

            // Create the compute kernel from the program.
            cl::make_kernel<int, cl::Buffer, cl::Buffer, cl::Buffer> row_m_mul(program, "m_mul");

            // Do the multiplication COUNT times.
            for (int i = 0; i < COUNT; i++) { // COUNT actually set to 1
                zero_mat(N, h_C);

                start_time = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;

                cl::NDRange global(N);
                row_m_mul(cl::EnqueueArgs(queue, global), N, d_a, d_b, d_c);

                queue.finish();

                run_time  = (static_cast<double>(timer.getTimeMilliseconds()) / 1000.0) - start_time;

                cl::copy(queue, d_c, h_C.begin(), h_C.end());   // Retrieve result from device.

                analyze_results(N, h_C, run_time);  // Check results.
            }
        } else if (comp == ROW_OPT) {   /** OpenCL matrix multiplication - C row per work-item, A row in private memory */
            // Setup the buffers, initialize matrices, and write them into global memory.
            init_mat(N, h_A, h_B, h_C);
            d_a = cl::Buffer(context, h_A.begin(), h_A.end(), true);
            d_b = cl::Buffer(context, h_B.begin(), h_B.end(), true);
            d_c = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * size);

            timer.reset();

            // Create the compute program from the source buffer.
            cl::Program program(context, util::loadProgram("m_mul_row_opt.cl"), true);
            printf("\n===== OpenCL, matrix multiplication, C(i,j) per work item, order %d, single row private mem =====\n", N);

            // Create the compute kernel from the program.
            cl::make_kernel<int, cl::Buffer, cl::Buffer, cl::Buffer> rowApriv_m_mul(program, "m_mul");

            // Do the multiplication COUNT times.
            for (int i = 0; i < COUNT; i++) { // COUNT actually set to 1
                zero_mat(N, h_C);

                start_time = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;

                cl::NDRange global(N);
                cl::NDRange local(N / 16);
                rowApriv_m_mul(cl::EnqueueArgs(queue, global, local), N, d_a, d_b, d_c);

                queue.finish();

                run_time  = (static_cast<double>(timer.getTimeMilliseconds()) / 1000.0) - start_time;

                cl::copy(queue, d_c, h_C.begin(), h_C.end());   // Retrieve result from device.

                analyze_results(N, h_C, run_time);  // Check results.
            }
        } else if (comp == ROW_COL_OPT) {   /** OpenCL matrix multiplication - C row per work-item, A row in private memory, B cols in local memory */
            // Setup the buffers, initialize matrices, and write them into global memory.
            init_mat(N, h_A, h_B, h_C);
            d_a = cl::Buffer(context, h_A.begin(), h_A.end(), true);
            d_b = cl::Buffer(context, h_B.begin(), h_B.end(), true);
            d_c = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * size);

            timer.reset();

            // Create the compute program from the source buffer.
            cl::Program program(context, util::loadProgram("m_mul_row_col_opt.cl"), true);
            printf("\n===== OpenCL, matrix multiplication, C(i,j) per work item, order %d, single row private + local mem =====\n", N);

            // Create the compute kernel from the program.
            cl::make_kernel<int, cl::Buffer, cl::Buffer, cl::Buffer, cl::LocalSpaceArg> colBloc_m_mul(program, "m_mul");

            // Do the multiplication COUNT times.
            for (int i = 0; i < COUNT; i++) { // COUNT actually set to 1
                zero_mat(N, h_C);

                start_time = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;

                cl::NDRange global(N);
                cl::NDRange local(N / 16);
                cl::LocalSpaceArg local_mem = cl::Local(sizeof(float) * N);
                colBloc_m_mul(cl::EnqueueArgs(queue, global, local), N, d_a, d_b, d_c, local_mem);

                queue.finish();

                run_time  = (static_cast<double>(timer.getTimeMilliseconds()) / 1000.0) - start_time;

                cl::copy(queue, d_c, h_C.begin(), h_C.end());   // Retrieve result from device.

                analyze_results(N, h_C, run_time);  // Check results.
            }
        } else if (comp == BLOCKED_OPT) {

            printf("\n===== OpenCL, matrix multiplication, C(i,j) per work item, order %d, blocked =====\n", N);

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
