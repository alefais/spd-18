//------------------------------------------------------------------------------
//
// Kernel:  m_mul
//
// Purpose: Compute the matrix multiplication
//
//          C = A * B
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
                int k;
                int i = get_global_id(0);
                int j = get_global_id(1);
                float tmp;

                if (i < N && j < N) {
                    tmp = 0.0f;
                    for (k = 0; k < N; k++) {
                        tmp += A[i * N + k] * B[k * N + j];
                    }
                    C[i * N + j] = tmp;
                }
            }