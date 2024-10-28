#include <mpi.h>
#include <iostream>
using namespace std;
int main(int argc, char* argv[])
{
    int rank, comm_sz, valorLocal, sumaGlobal = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    valorLocal = rank + 1;
    int num = 1;
    while (num < comm_sz)
    {
        if (rank % (2 * num) == 0 )
        {
            int emisor = rank + num;
            if (emisor < comm_sz)
            {
                int valorRecibido;
                MPI_Recv(&valorRecibido, 1, MPI_INT, emisor, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                valorLocal += valorRecibido;
            }
        } else if ((rank - num) % (2 * num) == 0)
        {
            int receptor = rank - num;
            MPI_Send(&valorLocal, 1, MPI_INT, receptor, 0, MPI_COMM_WORLD);
            break;
        }
        num *= 2;
    }
    if (rank == 0)
    {
        cout << "La suma total es " << valorLocal << endl;
    }
    MPI_Finalize();
    return 0;
}