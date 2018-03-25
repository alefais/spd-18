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
void printRow(int* row, int size, int src, int dst);
void printStatus(MPI_Status status, MPI_Datatype vector);
void process_0(int comm_size, MPI_Comm comm, MPI_Datatype row);
void process_i(int rank, int comm_size, MPI_Comm comm, MPI_Datatype row);

/*
 * Build data type for one row of a square matrix (N x N).
 * Process 0 scatters the rows of the matrix to the other
 * processes in the communicator and then gathers the results;
 * the generic process i does some computation on the received row.
 * Assumption: the number of rows in the matrix is exactly the
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

    MPI_Datatype row;
    int n = comm_size - 1;
    MPI_Type_contiguous(n, MPI_INT, &row);
    MPI_Type_commit(&row);

    if (rank == 0) {
        process_0(comm_size, MPI_COMM_WORLD, row);
    } else if (rank >= 1) {
        process_i(rank, comm_size, MPI_COMM_WORLD, row);
    }

    MPI_Finalize();
}

void process_0(int comm_size, MPI_Comm comm, MPI_Datatype row) {
    int n = comm_size - 1;
    int* matrix = (int *) malloc(sizeof(int) * n * n);
    MPI_Status status;
    initMatrix(matrix, n);
    printMatrix(matrix, n);

    for (int i = 0; i < n; i++) {
        MPI_Send(&matrix[i * n], 1, row, i + 1, 0, comm);
    }

    for (int j = 0; j < n; j++) {
        MPI_Recv(&matrix[j * n], 1, row, j + 1, 0, comm, &status);
        //printStatus(status, row);
    }

    printMatrix(matrix, n);
    free(matrix);
}

void process_i(int rank, int comm_size, MPI_Comm comm, MPI_Datatype row) {
    int n = comm_size - 1;
    int* buf = (int*) malloc(sizeof(int) * n);
    MPI_Status status;

    MPI_Recv(buf, 1, row, 0, 0, comm, &status);
    printRow(buf, n, 0, rank);
    //printStatus(status, row);

    // computation
    for (int i = 0; i < n; i++) {
        buf[i] = rank;
    }
    printRow(buf, n, rank, 0);

    MPI_Send(buf, 1, row, 0, 0, comm);
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

void printRow(int *row, int size, int src, int dst) {
    printf("Printing row received by %d from %d: \n", dst, src);
    for (int i = 0; i < size; i++) {
        printf("%d \t", row[i]);
    }
    printf("\n");
}

void printStatus(MPI_Status status, MPI_Datatype vector) {
    int count = 0;
    MPI_Get_count(&status, vector, &count);
    printf("Status struct: tag %d - source %d - count %d\n", status.MPI_TAG, status.MPI_SOURCE, count);
}