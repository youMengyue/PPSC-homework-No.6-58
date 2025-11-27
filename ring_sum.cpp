/*
 * MPI Ring Pattern Demo
 * Student Name: Fan Zhanhong
 * Student ID: 58
 * Pattern: 5 - Ring Exchange
 * Data Type: 3 - long long int
 */

#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Define the data type as specified: long long int
    long long local_value = (long long)world_rank * 1000;
    long long token;

    // Determine neighbors in the ring topology
    // Next process: (rank + 1) % size
    // Previous process: (rank - 1 + size) % size
    int next_rank = (world_rank + 1) % world_size;
    int prev_rank = (world_rank - 1 + world_size) % world_size;
    int tag = 0;

    if (world_rank == 0) {
        std::cout << "[Process 0] MPI Ring Pattern Demo by Fan Zhanhong (ID: 58)" << std::endl;
        std::cout << "[Process 0] Topology: Ring size = " << world_size << ". Data Type: long long int." << std::endl;
        
        // Handle the special case where there is only 1 process
        if (world_size == 1) {
            std::cout << "[Process 0] Only 1 process. Result is local value: " << local_value << std::endl;
            std::cout << "[Process 0] Verification: SUCCESS" << std::endl;
        } else {
            // Master process starts the ring communication
            token = local_value;
            std::cout << "[Process 0] Starting token with value: " << token << std::endl;
            
            // Send token to the next process (Rank 1)
            MPI_Send(&token, 1, MPI_LONG_LONG, next_rank, tag, MPI_COMM_WORLD);
            
            // Wait to receive the final result from the last process (Rank N-1)
            MPI_Recv(&token, 1, MPI_LONG_LONG, prev_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            std::cout << "[Process 0] Final Result received from Process " << prev_rank << ": " << token << std::endl;

            // Verification
            // Expected sum = sum(i * 1000) for i in 0 to N-1
            //              = 1000 * sum(i)
            //              = 1000 * (N-1)*N / 2
            long long expected_sum = 1000LL * (long long)(world_size - 1) * (long long)world_size / 2;
            
            if (token == expected_sum) {
                std::cout << "[Process 0] Verification: SUCCESS (Expected: " << expected_sum << ")" << std::endl;
            } else {
                std::cout << "[Process 0] Verification: FAILURE (Expected: " << expected_sum << ", Got: " << token << ")" << std::endl;
            }
        }
    } else {
        // Worker processes
        // 1. Receive token from previous process
        MPI_Recv(&token, 1, MPI_LONG_LONG, prev_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        std::cout << "[Process " << world_rank << "] Received token " << token 
                  << ", added " << local_value 
                  << ", sending " << (token + local_value) << " to Process " << next_rank << std::endl;
        
        // 2. Update token
        token += local_value;
        
        // 3. Send token to next process
        MPI_Send(&token, 1, MPI_LONG_LONG, next_rank, tag, MPI_COMM_WORLD);
    }

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}
