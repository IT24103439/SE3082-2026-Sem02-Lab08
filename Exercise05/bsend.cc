// message2_bsend.cc - rank 0 sends three numbers using MPI_Bsend and a single variable
#include <mpi.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int number;

    if (rank == 0) {
        // Allocate buffer space: 3 messages of size sizeof(int) plus MPI overhead
        int buffer_size = 3 * (sizeof(int) + MPI_BSEND_OVERHEAD);
        void* buffer = malloc(buffer_size);
        MPI_Buffer_attach(buffer, buffer_size);

        for (int i = 0; i < 3; i++) {
            number = i * 10;
            // Bsend copies the variable into the buffer and returns immediately
            MPI_Bsend(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            std::cout << "Process 0 buffered and sent " << number << "\n";
        }

        // Detach buffer; blocks until all buffered data is safely sent
        MPI_Buffer_detach(&buffer, &buffer_size);
        free(buffer);
    } else if (rank == 1) {
        for (int i = 0; i < 3; i++) {
            MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Process 1 received " << number << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}