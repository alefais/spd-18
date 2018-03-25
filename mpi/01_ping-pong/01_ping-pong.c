//
// Created by Alessandra Fais
// SPD course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

/*
 * Simple ping-pong program between two processes.
 */
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    //Check communicator

    // Check number of processes
    int world_size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Need at least 2 processes
    if (world_size < 2) return -1;

    // Pick the rank
    int world_rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) {
        char* msg = "ping";
        char* res = (char*) malloc(sizeof(char)*5);

        MPI_Send(msg, 5, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        printf("Process %d over %d has sent %s\n", world_rank, world_size, msg);

        MPI_Recv(res, 5, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d over %d received %s\n", world_rank, world_size, res);

        free(res);
    } else if (world_rank == 1) {
        char* res = (char*) malloc(sizeof(char)*5);
        char* msg = "pong";

        MPI_Recv(res, 5, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d over %d received %s\n", world_rank, world_size, res);

        MPI_Send(msg, 5, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        printf("Process %d over %d has sent %s\n", world_rank, world_size, msg);

        free(res);
    }

    MPI_Finalize();

    return 0;
}

