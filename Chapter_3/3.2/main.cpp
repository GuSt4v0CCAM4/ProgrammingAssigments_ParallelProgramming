#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;
int main (int argc, char* argv[])
{
    int rank, size;
    long long int local_tiros_circulo = 0, global_tiros_circulo = 0, tirosPerProceso, totalTiros;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    if (rank == 0)
    {
        cin >> totalTiros;
    }
    MPI_Bcast (&totalTiros, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    tirosPerProceso = totalTiros / size;
    unsigned int seed = time(0) + rank;

    for (long long int tiros = 0; tiros < tirosPerProceso; tiros++)
    {
        double x = (rand_r(&seed) / (double)RAND_MAX) * 2.0 - 1.0;
        double y = (rand_r(&seed) / (double)RAND_MAX) * 2.0 - 1.0;
        double distCuadrado = x * x + y * y;
        if (distCuadrado <= 1)
        {
            local_tiros_circulo++;
        }
    }
    MPI_Reduce(&local_tiros_circulo, &global_tiros_circulo, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0)
    {
        double pi = 4.0 * global_tiros_circulo / ((double) totalTiros);
        cout <<"El Pi estimado es " << pi << endl;
    }
    MPI_Finalize();
    return 0;
}