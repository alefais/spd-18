//
// Created by Alessandra Fais
// SPD course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10

void printVector(int* vector, int src, int dst);
void printStatus(MPI_Status status, MPI_Datatype vector);
void process_0(int rank, int comm_size, MPI_Comm comm, MPI_Datatype row);
void process_i(int rank, int comm_size, MPI_Comm comm, MPI_Datatype row);

/*
 * Build data type for a vector of size N.
 * Process i in the ring receives a vector from process (i - 1), he
 * modified it and he sends the modified vector to process (i + 1).
 */
int main(int argc, char** argv) {
    int world_size;
    int rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 2) {
        fprintf(stderr, "Must use at least two processes for this example\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Datatype row;
    MPI_Type_contiguous(N, MPI_INT, &row);
    MPI_Type_commit(&row);

    if (rank == 0) {
        process_0(0, world_size, MPI_COMM_WORLD, row);
    } else if (rank >= 1) {
        process_i(rank, world_size, MPI_COMM_WORLD, row);
    }

    MPI_Finalize();
}

void process_0(int rank, int comm_size, MPI_Comm comm, MPI_Datatype row) {
    int A[N] = {5};
    MPI_Status status;

    MPI_Send(A, 1, row, 1, 0, comm);

    MPI_Recv(A, 1, row, comm_size - 1, 0, comm, &status);
    printVector(A, comm_size - 1, rank);
    printStatus(status, row);
}

void process_i(int rank, int comm_size, MPI_Comm comm, MPI_Datatype row) {
    int* buf = (int*) malloc(sizeof(int) * N);
    MPI_Status status;

    MPI_Recv(buf, 1, row, rank - 1, 0, comm, &status);
    printVector(buf, rank - 1, rank);
    printStatus(status, row);

    // computation
    buf[rank % N] += 1;

    MPI_Send(buf, 1, row, (rank + 1) % comm_size, 0, comm);

    free(buf);
}

void printVector(int* vector, int src, int dst) {
    printf("Printing vector received by %d from %d: \n", dst, src);
    for (int i = 0; i < N; i++) {
        printf("%d ", vector[i]);
    }
    printf("\n");
}

void printStatus(MPI_Status status, MPI_Datatype vector) {
    int count = 0;
    MPI_Get_count(&status, vector, &count);
    printf("Status struct: tag %d - source %d - count %d\n", status.MPI_TAG, status.MPI_SOURCE, count);
}