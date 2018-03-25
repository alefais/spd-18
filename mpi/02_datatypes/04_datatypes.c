//
// Created by Alessandra Fais
// SPD course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void initMatrix(int* matrix, int size);
void printMatrix(int* matrix, int size);
void printCol(int *matrix, int size, int src, int dst);
void printStatus(MPI_Status status, MPI_Datatype vector);
void process_0(int comm_size, MPI_Comm comm, MPI_Datatype col);
void process_i(int rank, int comm_size, MPI_Comm comm, MPI_Datatype col);

/*
 * Build data type for one column of a square matrix (N x N).
 * Process 0 scatters the columns of the matrix to the other
 * processes in the communicator and then gathers the results;
 * the generic process i does some computation on the received col.
 * Assumption: the number of columns in the matrix is exactly the
 * number of processes in the communicator minus 1.
 */
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    if (comm_size < 2) {
        fprintf(stderr, "Must use at least two processes for this example\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = comm_size - 1; // matrix dimension
    MPI_Datatype col;
    MPI_Type_vector(n, 1, n, MPI_INT, &col);
    MPI_Type_commit(&col);

    if (rank == 0) {
        process_0(comm_size, MPI_COMM_WORLD, col);
    } else if (rank >= 1) {
        process_i(rank, comm_size, MPI_COMM_WORLD, col);
    }

    MPI_Finalize();
}

void process_0(int comm_size, MPI_Comm comm, MPI_Datatype col) {
    int n = comm_size - 1;
    int matrix[n][n];
    MPI_Status status;
    initMatrix(matrix[0], n);
    printMatrix(matrix[0], n);

    for (int i = 0; i < n; i++) {
        MPI_Send(&matrix[0][i], 1, col, i + 1, 0, comm);
        //printCol(matrix, n, 0, i + 1);
    }

    for (int j = 0; j < n; j++) {
        MPI_Recv(&matrix[0][j], 1, col, j + 1, 0, comm, &status);
        //printStatus(status, col);
    }

    printMatrix(matrix[0], n);
}

void process_i(int rank, int comm_size, MPI_Comm comm, MPI_Datatype col) {
    int n = comm_size - 1;
    int* buf = (int*) malloc(sizeof(int) * n * n);
    MPI_Status status;

    MPI_Recv(buf, 1, col, 0, 0, comm, &status);
    printCol(buf, n, 0, rank);

    // computation
    for (int i = 0; i < n; i++) {
        buf[i * n] = rank;
    }
    printCol(buf, n, rank, 0);

    MPI_Send(buf, 1, col, 0, 0, comm);
    free(buf);
}

void initMatrix(int* matrix, int size) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[(i * size) + j] = count;
            count++;
        }
    }
}

void printMatrix(int* matrix, int size) {
    printf("Printing matrix in process 0\n");
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d\t", matrix[(i * size) + j]);
        }
        printf("\n");
    }
}

void printCol(int* matrix, int size, int src, int dst) {
    printf("Printing column received by %d from %d: \n", dst, src);
    for (int i = 0; i < size; i++) {
        printf("%d\n", matrix[i * size]);
    }
}

void printStatus(MPI_Status status, MPI_Datatype vector) {
    int count = 0;
    printf("Status struct: tag %d - source %d", status.MPI_TAG, status.MPI_SOURCE);
    MPI_Get_count(&status, vector, &count);
    printf(" - count %d\n", count);
}