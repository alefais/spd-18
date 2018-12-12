//------------------------------------------------------------------------------
//
// Kernel:  m_mul
//
// Purpose: Compute the matrix multiplication (only global memory is used and
//          each work-item computes a full row of C)
//
//          C = A * B
//
//          Note: the global dimensions are 1024x1024 (the whole problem space),
//                the local dimensions (of the work-group) are 128x128;
//                global memory is visible to all the work-groups, local memory
//                shared within a work-group, private memory is visible to the
//                single work-item.
//
// Input:
//          A and B constant float matrices of size N * N
//
// Output:
//          C float matrix of size N * N holding the product A * B
//
// Student: Alessandra Fais - SPD course @ UniPi
//
//------------------------------------------------------------------------------

__kernel void m_mul(
            const int N,
            __global float* A,
            __global float* B,
            __global float* C)
            {
                int j, k;
                int i = get_global_id(0);
                float tmp;

                if (i < N) {
                    for (j = 0; j < N; j++) {
                        tmp = 0.0f;
                        for (k = 0; k < N; k++) {
                            tmp += A[i * N + k] * B[k * N + j];
                        }
                        C[i * N + j] = tmp;
                    }
                }
            }