//
// Created by Alessandra Fais
// SPD course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define MAX_AMOUNT 100

void dynamic_receive(int my_rank, int partner_rank, MPI_Comm comm, int comm_size);
void dynamic_send(int my_rank, int partner_rank, MPI_Comm comm, int comm_size);
void process_0(MPI_Comm comm, int comm_size);
void process_i(int my_rank, MPI_Comm comm, int comm_size);

/*
 * Simple message passing between processes in a ring.
 * NOTE: function definition for the processes, status object usage, dynamic buffer allocation (mpi tutorial).
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

    if (rank == 0) {
        process_0(MPI_COMM_WORLD, comm_size);
    } else if (rank >= 1) {
        process_i(rank, MPI_COMM_WORLD, comm_size);
    }

    MPI_Finalize();
}

void dynamic_receive(int my_rank, int partner_rank, MPI_Comm comm,  int comm_size) {
    MPI_Status status;
    MPI_Probe(partner_rank, 0, comm, &status);

    int count = 0;
    MPI_Get_count(&status, MPI_INT, &count);

    int* buffer = (int*)malloc(sizeof(int) * count);
    MPI_Recv(buffer, count, MPI_INT, partner_rank, 0, comm, MPI_STATUS_IGNORE);
    printf("Process %d/%d dynamically received %d numbers from %d\n", my_rank, comm_size, count, partner_rank);
    free(buffer);
}

void dynamic_send(int my_rank, int partner_rank, MPI_Comm comm,  int comm_size) {
    int data[MAX_AMOUNT];
    int data_size = 0;
    srand(time(NULL));
    data_size = rand() / (float)RAND_MAX * MAX_AMOUNT;

    MPI_Send(data, data_size, MPI_INT, partner_rank, 0, comm);
    printf("Process %d/%d sent %d numbers to %d\n", my_rank, comm_size, data_size, partner_rank);
}

void process_0(MPI_Comm comm, int comm_size) {
    dynamic_send(0, 1, comm, comm_size);
    dynamic_receive(0, comm_size - 1, comm, comm_size);
}

void process_i(int my_rank, MPI_Comm comm,  int comm_size) {
    dynamic_receive(my_rank, my_rank - 1, comm, comm_size);
    dynamic_send(my_rank, (my_rank + 1) % comm_size, comm, comm_size);
}