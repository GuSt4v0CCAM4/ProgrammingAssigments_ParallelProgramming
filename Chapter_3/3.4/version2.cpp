#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    int rank, comm_sz, valorLocal, sumaGlobal;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    valorLocal = rank + 1;
    int offset = 1;

    while (offset < comm_sz) {
        int compañero = rank ^ offset;
        if (compañero < comm_sz) {
            int valorRecibido;
            MPI_Sendrecv(&valorLocal, 1, MPI_INT, compañero, 0, &valorRecibido, 1, MPI_INT, compañero, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            valorLocal += valorRecibido;
        }

        offset *= 2;
    }

    cout << "Proceso " << rank << " - Suma global: " << valorLocal << endl;

    MPI_Finalize();
    return 0;
}
