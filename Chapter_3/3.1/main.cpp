#include <mpi.h>
#include <iostream>
#define BIN_COUNT 5
using namespace std;

int find_bin(double value, double min, double bin_size) {
    int bin_index = (int)((value - min) / bin_size);
    if (bin_index >= BIN_COUNT) {
        bin_index = BIN_COUNT - 1;
    }
    return bin_index;
}

int main(int argc, char* argv[]) {
    int local_hist[BIN_COUNT] = {0};
    int global_hist[BIN_COUNT] = {0};
    double min = 0.0, max = 5.0;
    double bin_size = (max - min) / BIN_COUNT;

    double data[] = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};
    int data_count = 20;

    MPI_Init(&argc, &argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int local_data_size = data_count / world_size;
    double* local_data = new double[local_data_size];

    MPI_Scatter(data, local_data_size, MPI_DOUBLE, local_data, local_data_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_data_size; i++) {
        int bin = find_bin(local_data[i], min, bin_size);
        local_hist[bin]++;
    }

    MPI_Reduce(local_hist, global_hist, BIN_COUNT, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        cout << "Histogram:" << endl;
        for (int i = 0; i < BIN_COUNT; i++) {
            cout << "Bin " << i << ": " << global_hist[i] << endl;
        }
    }

    delete[] local_data;
    MPI_Finalize();
    return 0;
}
