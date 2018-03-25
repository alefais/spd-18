//
// Created by Alessandra Fais
// SPD course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

/*
 * Simple message passing between processes in a ring.
 * NOTE: MPI_Status struct set.
 */
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int world_size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 2) return -1;

    int world_rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) {
        char* msg = "hello";
        char* res = (char*) malloc(sizeof(char) * 6);
        MPI_Status status;
        status.MPI_SOURCE = 0;
        status.MPI_TAG = 0;
        int count = 0;

        MPI_Send(msg, 6, MPI_CHAR, world_rank + 1, 0, MPI_COMM_WORLD);
        printf("Process %d/%d has sent %s\n", world_rank, world_size, msg);

        MPI_Recv(res, 6, MPI_CHAR, world_size - 1, 0, MPI_COMM_WORLD, &status);
        printf("Process %d/%d received %s\n", world_rank, world_size, res);

        MPI_Get_count(&status, MPI_CHAR, &count);
        printf("Status is {source: %d, tag: %d, count: %d}\n", status.MPI_SOURCE, status.MPI_TAG, count);

        free(res);
    } else if (world_rank >= 1) {
        char* msg = "hello";
        char* res = (char*) malloc(sizeof(char) * 6);
        MPI_Status status;
        int count = 0;

        MPI_Recv(res, 6, MPI_CHAR, world_rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("Process %d/%d received %s\n", world_rank, world_size, res);

        MPI_Get_count(&status, MPI_CHAR, &count);
        printf("Status is {source: %d, tag: %d, count: %d}\n", status.MPI_SOURCE, status.MPI_TAG, count);

        MPI_Send(msg, 6, MPI_CHAR, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
        printf("Process %d/%d has sent %s\n", world_rank, world_size, msg);

        free(res);
    }

    MPI_Finalize();

    return 0;
}

