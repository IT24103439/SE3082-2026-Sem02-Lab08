#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const long long N = 10000000LL;

    // Start timer
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // Divide the workload evenly among processes
    long long chunk = N / size;
    long long start = rank * chunk + 1;
    long long end = (rank == size - 1) ? N : (rank + 1) * chunk;

    // Compute partial sum locally
    long long local_sum = 0;
    for (long long i = start; i <= end; ++i) {
        local_sum += i;
    }

    // Reduce all local sums to global_sum on rank 0
    long long global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Stop timer
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    // Find the maximum time across all processes for accurate benchmarking
    double max_time = 0.0;
    MPI_Reduce(&elapsed_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Number of processes: " << size << "\n";
        std::cout << "Computed Sum       : " << global_sum << "\n";
        std::cout << "Expected Sum       : " << (N * (N + 1)) / 2 << "\n";
        std::cout << "Execution Time     : " << max_time << " seconds\n";
    }

    MPI_Finalize();
    return 0;
}