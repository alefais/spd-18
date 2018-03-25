//
// Created by Alessandra Fais
// SPD course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <mpi.h>
#include <printf.h>
#include <stdlib.h>

void process_0(int rank, MPI_Comm comm, int world_size, char* data);
void process_i(int rank, MPI_Comm comm, int world_size, char* data);

/*
 * Simple message passing between processes in a ring.
 * NOTE: function definition for the processes.
 */
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int world_size = 0;
    int world_rank = 0;
    char* data_0 = NULL;
    char* data_i = NULL;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 2) return -1;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    data_0 = "Hello, this is process zero!";
    data_i = malloc(sizeof(char) * 64);

    if (world_rank == 0) {
        process_0(world_rank, MPI_COMM_WORLD, world_size, data_0);
    } else if (world_rank >= 1) {
        process_i(world_rank, MPI_COMM_WORLD, world_size, data_i);
    }

    MPI_Finalize();
    free(data_i);

    return 0;
}

void process_0(int rank, MPI_Comm comm, int world_size, char* data) {

    int count = 0;
    int dim_buf = 64;
    char* res = (char*) malloc(sizeof(char) * dim_buf);
    MPI_Status status;
    status.MPI_SOURCE = 0;
    status.MPI_TAG = 0;

    MPI_Send(data, dim_buf, MPI_CHAR, rank + 1, 0, comm);
    printf("Process %d/%d has sent data: <%s>\n", rank, world_size, data);

    MPI_Recv(res, dim_buf, MPI_CHAR, world_size - 1, 0, comm, &status);
    printf("Process %d/%d received data: <%s>\n", rank, world_size, res);

    MPI_Get_count(&status, MPI_CHAR, &count);
    printf("Status is {source: %d, tag: %d, count: %d}\n", status.MPI_SOURCE, status.MPI_TAG, count);

    free(res);
}

void process_i(int rank, MPI_Comm comm, int world_size, char* data) {

    int count = 0;
    int dim_buf = 64;
    char* res = (char*) malloc(sizeof(char) * dim_buf);
    MPI_Status status;

    MPI_Recv(res, dim_buf, MPI_CHAR, rank - 1, 0, comm, &status);
    printf("Process %d/%d received data: <%s>\n", rank, world_size, res);

    MPI_Get_count(&status, MPI_CHAR, &count);
    printf("Status is {source: %d, tag: %d, count: %d}\n", status.MPI_SOURCE, status.MPI_TAG, count);

    snprintf(data, dim_buf, "Hello, this is neighbour process %d!", rank);
    MPI_Send(data, dim_buf, MPI_CHAR, (rank + 1) % world_size, 0, comm);
    printf("Process %d/%d has sent data: <%s>\n", rank, world_size, data);

    free(res);
}