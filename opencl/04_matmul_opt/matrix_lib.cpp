//------------------------------------------------------------------------------
//
//  Program: Matrix library for the multiplication driver
//
//  Purpose: This is a simple set of functions to manipulate
//           matrices used with the multiplication driver.
//
//  Usage:   The matrices are square and the order is
//           set as a defined constant, ORDER.
//
//  Student: Alessandra Fais - SPD course @ UniPi
//
//------------------------------------------------------------------------------

#include "matmul.hpp"

//------------------------------------------------------------------------------
//
//  Function to compute the matrix product (sequential algorithm, dot product).
//
//------------------------------------------------------------------------------
void seq_mat_mul_dot(int N, std::vector<float>& A, std::vector<float>& B, std::vector<float>& C) {
    int i, j, k;
    float tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            tmp = 0.0f;
            for (k = 0; k < N; k++) {
                tmp += A[i * N + k] * B[ k * N + j];     /* C(i,j) = sum(over k) A(i,k) * B(k,j) */
            }
            C[i * N + j] = tmp;
        }
    }
}

//------------------------------------------------------------------------------
//
//  Function to initialize the input matrices A and B.
//
//------------------------------------------------------------------------------
void init_mat(int N, std::vector<float>& A, std::vector<float>& B, std::vector<float>& C) {
    int i, j;

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			A[i * N + j] = AVAL;

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			B[i * N + j] = BVAL;

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			C[i * N + j] = 0.0f;
}

//------------------------------------------------------------------------------
//
//  Function to set a matrix to zero.
//
//------------------------------------------------------------------------------
void zero_mat(int N, std::vector<float>& C) {
    int i, j;

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			C[i * N + j] = 0.0f;
}

//------------------------------------------------------------------------------
//
//  Function to fill B_trans(M_dim, P_dim) with the transpose of B(P_dim, M_dim).
//
//------------------------------------------------------------------------------
void trans(int N, std::vector<float>& B, std::vector<float>& B_trans) {
    int i, j;

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		    B_trans[j * N + i] = B[i * N + j];
}

//------------------------------------------------------------------------------
//
//  Function to compute errors of the product matrix.
//
//------------------------------------------------------------------------------
float error(int N, std::vector<float>& C) {
    int i, j;
    float cval, errsq, err;
    cval = (float) N * AVAL * BVAL;
    errsq = 0.0f;

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            err = C[i * N + j] - cval;
            errsq += err * err;
        }
    }
    return errsq;
}

//------------------------------------------------------------------------------
//
//  Function to analyze and output results.
//
//------------------------------------------------------------------------------
void analyze_results(int N, std::vector<float>& C, double run_time) {
    float mflops;
    float errsq;
    
    mflops = 2.0 * N * N * N/(1000000.0f * run_time);
    printf(" %.2f seconds at %.1f MFLOPS \n", run_time, mflops);
    errsq = error(N, C);
    if (std::isnan(errsq) || errsq > TOL)
        printf("\n Errors in multiplication: %f\n", errsq);
    else
        printf("\n Matrix mul to find C = A * B:  %.0f out of %d results were correct.\n", N * N - errsq, N * N);
}

