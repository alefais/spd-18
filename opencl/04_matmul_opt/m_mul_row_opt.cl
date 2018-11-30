//------------------------------------------------------------------------------
//
// Kernel:  m_mul
//
// Purpose: Compute the matrix multiplication (each work-item copies its row of
//          A in the private memory)
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
                int j, k;
                int i = get_global_id(0);
                float A_copy[N];
                float tmp;

                if (i < N) {
                    for (k = 0; k < N; k++) {
                        A_copy[k] = A[i * N + k];
                    }

                    for (j = 0; j < N; j++) {
                        tmp = 0.0f;
                        for (k = 0; k < N; k++) {
                            tmp += A_copy[k] * B[k * N + j];
                        }
                        C[i * N + j] = tmp;
                    }
                }
            }