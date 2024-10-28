#include <mpi.h>
#include <iostream>
#include <ctime>
using namespace std;

int main(int argc, char* argv[]) {
    int rank, comm_sz;
    const int PING_PONG_LIMIT = 10000;
    int ping_pong_count = 0;
    int message = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (comm_sz != 2) {
        if (rank == 0) {
            cerr << "Este programa requiere exactamente 2 procesos." << endl;
        }
        MPI_Finalize();
        return 1;
    }

    clock_t start_clock, end_clock;
    start_clock = clock();

    double start_time, end_time;
    start_time = MPI_Wtime();

    while (ping_pong_count < PING_PONG_LIMIT) {
        if (rank == 0) {
            MPI_Send(&message, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&message, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else if (rank == 1) {
            MPI_Recv(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        ping_pong_count++;
    }

    end_clock = clock();
    end_time = MPI_Wtime();

    if (rank == 0) {
        double clock_time = double(end_clock - start_clock) / CLOCKS_PER_SEC;
        double mpi_time = end_time - start_time;

        cout << "Tiempo medido con clock(): " << clock_time << " segundos" << endl;
        cout << "Tiempo medido con MPI_Wtime(): " << mpi_time << " segundos" << endl;
    }

    MPI_Finalize();
    return 0;
}
