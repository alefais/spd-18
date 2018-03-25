//
// Created by Alessandra Fais
// SPD course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <mpi.h>
#include <printf.h>

void process_source(int rank, MPI_Comm comm, int world_size);
void process_i(int rank, MPI_Comm comm, int world_size);
void process_sink(int rank, MPI_Comm comm, int world_size);
void print_data(int* buf, int size);

/*
 * Simple message passing between processes in a pipeline.
 * NOTE: function definition for the processes and computation over a buffer of integers.
 */
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int world_size = 0;
    int world_rank = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 2) return -1;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) {
        process_source(world_rank, MPI_COMM_WORLD, world_size);
    } else if (world_rank >= 1 && world_rank != world_size - 1) {
        process_i(world_rank, MPI_COMM_WORLD, world_size);
    } else if (world_rank == world_size - 1) {
        process_sink(world_rank, MPI_COMM_WORLD, world_size);
    }

    MPI_Finalize();

    return 0;
}

void process_source(int rank, MPI_Comm comm, int world_size) {
    int x[10] = { 0 };
    MPI_Send(x, sizeof(int), MPI_INT, rank + 1, 0, comm);
    printf("Process %d/%d has sent:\t", rank, world_size);
    print_data(x, 10);
}

void process_i(int rank, MPI_Comm comm, int world_size) {

    int count = 0;
    int buf[10];
    MPI_Status status;

    MPI_Recv(buf, 10, MPI_INT, rank - 1, 0, comm, &status);
    printf("Process %d/%d received:\t", rank, world_size);
    print_data(buf, 10);

    MPI_Get_count(&status, MPI_CHAR, &count);
    printf("Status is {source: %d, tag: %d, count: %d}\n", status.MPI_SOURCE, status.MPI_TAG, count);

    // Computation
    for (int i = 0; i < 10; i++) {
        buf[i] += 10;
    }

    MPI_Send(buf, 10, MPI_INT, rank + 1, 0, comm);
    printf("Process %d/%d has sent:\t", rank, world_size);
    print_data(buf, 10);
}

void process_sink(int rank, MPI_Comm comm, int world_size) {

    int count = 0;
    int buf[10];
    MPI_Status status;

    MPI_Recv(buf, 10, MPI_INT, rank - 1, 0, comm, &status);
    printf("Process %d/%d received:\t", rank, world_size);
    print_data(buf, 10);

    MPI_Get_count(&status, MPI_CHAR, &count);
    printf("Status is {source: %d, tag: %d, count: %d}\n", status.MPI_SOURCE, status.MPI_TAG, count);

}

void print_data(int* buf, int size) {
    printf("{ ");
    for (int i = 0; i < size - 1; i++) {
        printf("%d ", buf[i]);
    }
    printf("%d }\n", buf[size - 1]);
}