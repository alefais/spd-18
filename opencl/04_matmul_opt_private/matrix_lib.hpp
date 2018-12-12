//------------------------------------------------------------------------------
//
//  Program: Matrix library include file (function prototypes).
//
//  Student: Alessandra Fais - SPD course @ UniPi
//
//------------------------------------------------------------------------------

#ifndef __MATRIX_LIB_HDR
#define __MATRIX_LIB_HDR

#include <vector>

//------------------------------------------------------------------------------
//
//  Function to compute the matrix product (sequential algorithm, dot product).
//
//------------------------------------------------------------------------------
void seq_mat_mul_dot(int N, std::vector<float>& A, std::vector<float>& B, std::vector<float>& C);

//------------------------------------------------------------------------------
//
//  Function to initialize the input matrices A and B.
//
//------------------------------------------------------------------------------
void init_mat(int N, std::vector<float>& A, std::vector<float>& B, std::vector<float>& C);

//------------------------------------------------------------------------------
//
//  Function to set a matrix to zero.
//
//------------------------------------------------------------------------------
void zero_mat(int N, std::vector<float>& C);

//------------------------------------------------------------------------------
//
//  Function to fill B_trans(M_dim, P_dim) with the transpose of B(P_dim, M_dim).
//
//------------------------------------------------------------------------------
void trans(int N, std::vector<float>& B, std::vector<float>& B_trans);

//------------------------------------------------------------------------------
//
//  Function to compute errors of the product matrix.
//
//------------------------------------------------------------------------------
float error(int N, std::vector<float>& C);

//------------------------------------------------------------------------------
//
//  Function to analyze and output results.
//
//------------------------------------------------------------------------------
void analyze_results(int N, std::vector<float>& C, double run_time);
    
#endif
