//------------------------------------------------------------------------------
//
// Kernel:  m_mul
//
// Purpose: Compute the matrix multiplication (each work-item computes a full
//          row of C; we have locality and reuse on the row of A since the same
//          row is used to compute each C[i][j]; copying A[i] into the work-item
//          private memory avoids the overhead of pulling it from global memory
//          for each C[i][j]).
//          Since each work-item in a work-group uses the same columns of B, a
//          further optimization consists in storing B's columns into the work-group
//          local memory.
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
            __global float* C,
            __local float* B_local)     // Pass a pointer to local memory.
            {
                int j, k;

                int i = get_global_id(0);

                int i_loc = get_local_id(0);
                int n_loc = get_local_size(0);

                float A_private[N];
                float tmp;

                if (i < N) {

                    // Copy the row of A into work-item private memory.
                    for (k = 0; k < N; k++) {
                        A_private[k] = A[i * N + k];
                    }

                    // Use work-item private memory to access A element values.
                    for (j = 0; j < N; j++) {

                        // Copy the columns of B into work-group local memory.
                        for (k = i_loc; k < N; k += n_loc)
                            B_local[k] = B[k * N + j];

                        barrier(CLK_LOCAL_MEM_FENCE);

                        tmp = 0.0f;
                        for (k = 0; k < N; k++) {
                            tmp += A_private[k] * B_local[k];
                        }
                        C[i * N + j] = tmp;
                    }
                }
            }