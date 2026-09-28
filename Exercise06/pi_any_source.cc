// pi_any_source.cc - Monte Carlo Pi using MPI_ANY_SOURCE
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const long long TOTAL_POINTS = 10000000LL;

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    long long local_points = TOTAL_POINTS / size;
    if (rank == size - 1) {
        local_points += (TOTAL_POINTS % size);
    }

    unsigned int seed = static_cast<unsigned int>(time(NULL)) ^ (rank * 104729);

    long long local_circle_count = 0;
    for (long long i = 0; i < local_points; ++i) {
        double x = static_cast<double>(rand_r(&seed)) / RAND_MAX;
        double y = static_cast<double>(rand_r(&seed)) / RAND_MAX;

        if (x * x + y * y <= 1.0) {
            local_circle_count++;
        }
    }

    long long total_circle_count = local_circle_count;

    if (rank != 0) {
        // Workers send their partial counts to rank 0 with tag 0
        MPI_Send(&local_circle_count, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    } else {
        // Rank 0 receives (size - 1) messages from whichever rank finishes first
        MPI_Status status;
        for (int i = 1; i < size; ++i) {
            long long received_count = 0;
            MPI_Recv(&received_count, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            total_circle_count += received_count;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    if (rank == 0) {
        double pi_estimate = 4.0 * static_cast<double>(total_circle_count) / TOTAL_POINTS;
        std::cout << "Processes           : " << size << "\n";
        std::cout << "Total Points        : " << TOTAL_POINTS << "\n";
        std::cout << "Points Inside Circle: " << total_circle_count << "\n";
        std::cout << "Estimated Pi        : " << pi_estimate << "\n";
        std::cout << "Execution Time      : " << elapsed_time << " seconds\n";
    }

    MPI_Finalize();
    return 0;
}