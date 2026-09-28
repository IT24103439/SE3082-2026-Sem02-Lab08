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

    // Synchronize and begin timing
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // Divide points among processes
    long long local_points = TOTAL_POINTS / size;
    if (rank == size - 1) {
        local_points += (TOTAL_POINTS % size); // handle remainder on last rank
    }

    // Seed the random number generator uniquely per rank to avoid duplicate sequences
    unsigned int seed = static_cast<unsigned int>(time(NULL)) ^ (rank * 104729);

    long long local_circle_count = 0;
    for (long long i = 0; i < local_points; ++i) {
        // Generate uniform random doubles between 0.0 and 1.0 using thread-safe rand_r
        double x = static_cast<double>(rand_r(&seed)) / RAND_MAX;
        double y = static_cast<double>(rand_r(&seed)) / RAND_MAX;

        if (x * x + y * y <= 1.0) {
            local_circle_count++;
        }
    }

    // Gather results onto rank 0 using Point-to-Point communication
    long long total_circle_count = local_circle_count;

    if (rank != 0) {
        // Worker processes send their local counts to rank 0
        MPI_Send(&local_circle_count, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    } else {
        // Rank 0 receives counts from each worker process
        for (int source = 1; source < size; ++source) {
            long long received_count = 0;
            MPI_Recv(&received_count, 1, MPI_LONG_LONG, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_circle_count += received_count;
        }
    }

    // Stop timing
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    if (rank == 0) {
        double pi_estimate = 4.0 * static_cast<double>(total_circle_count) / TOTAL_POINTS;
        std::cout << "Processes          : " << size << "\n";
        std::cout << "Total Points       : " << TOTAL_POINTS << "\n";
        std::cout << "Points Inside Circle: " << total_circle_count << "\n";
        std::cout << "Estimated Pi       : " << pi_estimate << "\n";
        std::cout << "Execution Time     : " << elapsed_time << " seconds\n";
    }

    MPI_Finalize();
    return 0;
}