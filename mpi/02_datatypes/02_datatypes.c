//
// Created by Alessandra Fais
// SPD course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10

void printMatrix(int* matrix, int src, int dst);
void printStatus(MPI_Status status, MPI_Datatype vector);
void process_0(int rank, int comm_size, MPI_Comm comm, MPI_Datatype matrix);
void process_i(int rank, int comm_size, MPI_Comm comm, MPI_Datatype matrix);

/*
 * Build data type for a square matrix (N x N).
 * Process i in the ring receives a matrix from process (i - 1), he
 * changes one row and he sends the modified matrix to process (i + 1).
 */
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 2) {
        fprintf(stderr, "Must use at least two processes for this example\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Datatype matrix;
    MPI_Type_contiguous(N * N, MPI_INT, &matrix);
    MPI_Type_commit(&matrix);

    if (rank == 0) {
        process_0(0, world_size, MPI_COMM_WORLD, matrix);
    } else if (rank >= 1) {
        process_i(rank, world_size, MPI_COMM_WORLD, matrix);
    }

    MPI_Finalize();
}

void process_0(int rank, int comm_size, MPI_Comm comm, MPI_Datatype matrix) {
    int A[N][N] = {5};
    MPI_Status status;

    MPI_Send(A[0], 1, matrix, 1, 0, comm);

    MPI_Recv(*A, 1, matrix, comm_size - 1, 0, comm, &status);
    printMatrix(*A, comm_size - 1, rank);
    printStatus(status, matrix);
}

void process_i(int rank, int comm_size, MPI_Comm comm, MPI_Datatype matrix) {
    int* buf = (int*) malloc(sizeof(int) * N * N);
    MPI_Status status;
    int idx = rank % N;

    MPI_Recv(buf, 1, matrix, rank - 1, 0, comm, &status);
    printMatrix(buf, rank - 1, rank);
    printStatus(status, matrix);

    // computation
    for (int i = 0; i < N; i++) {
        buf[(idx * N) + i] = rank;
    }

    MPI_Send(buf, 1, matrix, (rank + 1) % comm_size, 0, comm);

    free(buf);
}

void printMatrix(int* matrix, int src, int dst) {
    printf("Printing matrix received by %d from %d: \n", dst, src);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d\t", matrix[(i * N) + j]);
        }
        printf("\n");
    }
}

void printStatus(MPI_Status status, MPI_Datatype vector) {
    int count = 0;
    MPI_Get_count(&status, vector, &count);
    printf("Status struct: tag %d - source %d - count %d\n", status.MPI_TAG, status.MPI_SOURCE, count);
}