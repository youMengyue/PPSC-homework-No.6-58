/*
 * ============================================================================
 * MPI Ring Pattern Demo - Distributed Ring Summation
 * ============================================================================
 * 
 * Course:       Parallel Programming and Supercomputer Collaborative Design
 * Student Name: Fan Zhanhong
 * Student ID:   58
 * 
 * Assignment Parameters (calculated from Student ID):
 *   - Pattern:   5 - Ring Exchange  (formula: (58/4) % 10 + 1 = 5)
 *   - Data Type: 3 - long long int  (formula: 58 % 4 + 1 = 3)
 * 
 * Program Description:
 *   This program demonstrates the MPI Ring Exchange pattern where processes
 *   are arranged in a logical ring topology. A token (accumulator) is passed
 *   from process 0 through all processes back to process 0, with each process
 *   adding its local value to the token.
 * 
 * Ring Topology Visualization:
 * 
 *       ┌──────────────────────────────────────┐
 *       │                                      │
 *       ▼                                      │
 *   [Rank 0] ──▶ [Rank 1] ──▶ [Rank 2] ──▶ ... ──▶ [Rank N-1]
 *       ▲                                              │
 *       └──────────────────────────────────────────────┘
 * 
 * Communication Flow:
 *   1. Rank 0 initializes token with its local value and sends to Rank 1
 *   2. Each subsequent rank receives, adds its value, and forwards
 *   3. Rank N-1 sends the final accumulated sum back to Rank 0
 *   4. Rank 0 verifies the result using arithmetic series formula
 * 
 * ============================================================================
 */

#include <mpi.h>      // MPI library for parallel communication
#include <iostream>   // Standard I/O for console output

/*
 * ============================================================================
 * Main Function - Entry Point of the MPI Program
 * ============================================================================
 * 
 * Parameters:
 *   argc - Number of command line arguments (passed to MPI_Init)
 *   argv - Array of command line argument strings (passed to MPI_Init)
 * 
 * Returns:
 *   0 on successful execution
 * ============================================================================
 */
int main(int argc, char** argv) {
    
    /* ========================================================================
     * PHASE 1: MPI Environment Initialization
     * ========================================================================
     * MPI_Init must be called before any other MPI functions.
     * It initializes the MPI execution environment and sets up communication.
     */
    MPI_Init(&argc, &argv);

    /* ========================================================================
     * PHASE 2: Retrieve Process Information
     * ========================================================================
     * Get the total number of processes and the rank (ID) of this process.
     * - world_size: Total number of processes in MPI_COMM_WORLD
     * - world_rank: Unique identifier for this process (0 to world_size-1)
     */
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    /* ========================================================================
     * PHASE 3: Initialize Local Data
     * ========================================================================
     * Each process computes its own local value based on its rank.
     * Formula: local_value = rank * 1000
     * 
     * Example with 4 processes:
     *   Rank 0: local_value = 0 * 1000 = 0
     *   Rank 1: local_value = 1 * 1000 = 1000
     *   Rank 2: local_value = 2 * 1000 = 2000
     *   Rank 3: local_value = 3 * 1000 = 3000
     * 
     * Data type: long long int (64-bit) to prevent overflow with large sums
     */
    long long local_value = (long long)world_rank * 1000;
    
    // Token variable to hold the accumulated sum during ring transmission
    long long token;

    /* ========================================================================
     * PHASE 4: Determine Ring Neighbors
     * ========================================================================
     * In a ring topology, each process has exactly two neighbors:
     *   - next_rank: The process to send data TO
     *   - prev_rank: The process to receive data FROM
     * 
     * Using modulo arithmetic ensures the ring wraps around:
     *   - Process N-1 sends to Process 0 (completing the ring)
     *   - Process 0 receives from Process N-1
     */
    int next_rank = (world_rank + 1) % world_size;
    int prev_rank = (world_rank - 1 + world_size) % world_size;
    
    // Message tag for MPI communication (used to match send/receive pairs)
    int tag = 0;

    /* ========================================================================
     * PHASE 5: Ring Communication Logic
     * ========================================================================
     * The communication is split into two roles:
     *   - Master (Rank 0): Initiates and concludes the ring communication
     *   - Workers (Rank 1 to N-1): Relay and accumulate the token
     */
    
    if (world_rank == 0) {
        /* ====================================================================
         * MASTER PROCESS (Rank 0)
         * ====================================================================
         * Responsibilities:
         *   1. Print program information and topology details
         *   2. Initialize the token with its local value
         *   3. Send the token to start the ring
         *   4. Wait for the token to complete the full circle
         *   5. Verify the final result
         */
        
        // Display program header and configuration
        std::cout << "[Process 0] MPI Ring Pattern Demo by Fan Zhanhong (ID: 58)" << std::endl;
        std::cout << "[Process 0] Topology: Ring size = " << world_size 
                  << ". Data Type: long long int." << std::endl;
        
        // Special case: Handle single-process execution (no communication needed)
        if (world_size == 1) {
            std::cout << "[Process 0] Only 1 process. Result is local value: " 
                      << local_value << std::endl;
            std::cout << "[Process 0] Verification: SUCCESS" << std::endl;
        } else {
            /* ================================================================
             * Multi-process Ring Communication
             * ================================================================ */
            
            // Step 1: Initialize token with local value (Rank 0's contribution)
            token = local_value;
            std::cout << "[Process 0] Starting token with value: " << token << std::endl;
            
            /* ----------------------------------------------------------------
             * Step 2: Send token to the next process (Rank 1)
             * ----------------------------------------------------------------
             * MPI_Send Parameters:
             *   &token      - Pointer to data buffer to send
             *   1           - Number of elements to send
             *   MPI_LONG_LONG - MPI data type (matches long long int)
             *   next_rank   - Destination process rank
             *   tag         - Message tag for matching
             *   MPI_COMM_WORLD - Communicator (group of all processes)
             */
            MPI_Send(&token, 1, MPI_LONG_LONG, next_rank, tag, MPI_COMM_WORLD);
            
            /* ----------------------------------------------------------------
             * Step 3: Wait to receive the final result from Rank N-1
             * ----------------------------------------------------------------
             * MPI_Recv Parameters:
             *   &token      - Pointer to buffer for received data
             *   1           - Maximum number of elements to receive
             *   MPI_LONG_LONG - MPI data type
             *   prev_rank   - Source process rank (Rank N-1)
             *   tag         - Message tag for matching
             *   MPI_COMM_WORLD - Communicator
             *   MPI_STATUS_IGNORE - We don't need status information
             */
            MPI_Recv(&token, 1, MPI_LONG_LONG, prev_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Display the received final result
            std::cout << "[Process 0] Final Result received from Process " 
                      << prev_rank << ": " << token << std::endl;

            /* ----------------------------------------------------------------
             * Step 4: Verify the result using mathematical formula
             * ----------------------------------------------------------------
             * The sum of all local values should equal:
             *   sum = 1000 * (0 + 1 + 2 + ... + (N-1))
             *       = 1000 * (N-1) * N / 2   (Arithmetic series formula)
             * 
             * Using 1000LL ensures the calculation is done in 64-bit to
             * prevent integer overflow for large process counts.
             */
            long long expected_sum = 1000LL * (long long)(world_size - 1) * (long long)world_size / 2;
            
            // Compare computed result with expected value
            if (token == expected_sum) {
                std::cout << "[Process 0] Verification: SUCCESS (Expected: " 
                          << expected_sum << ")" << std::endl;
            } else {
                std::cout << "[Process 0] Verification: FAILURE (Expected: " 
                          << expected_sum << ", Got: " << token << ")" << std::endl;
            }
        }
    } else {
        /* ====================================================================
         * WORKER PROCESSES (Rank 1 to N-1)
         * ====================================================================
         * Responsibilities:
         *   1. Receive token from the previous process
         *   2. Add local value to the token
         *   3. Send updated token to the next process
         * 
         * This creates a pipeline where data flows through all workers
         * before returning to the master process.
         */
        
        /* --------------------------------------------------------------------
         * Step 1: Receive token from previous process
         * --------------------------------------------------------------------
         * This is a BLOCKING receive - the process will wait here until
         * data arrives from the previous process in the ring.
         */
        MPI_Recv(&token, 1, MPI_LONG_LONG, prev_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // Log the receive, computation, and send operation
        std::cout << "[Process " << world_rank << "] Received token " << token 
                  << ", added " << local_value 
                  << ", sending " << (token + local_value) 
                  << " to Process " << next_rank << std::endl;
        
        /* --------------------------------------------------------------------
         * Step 2: Accumulate - Add this process's contribution to the token
         * --------------------------------------------------------------------
         */
        token += local_value;
        
        /* --------------------------------------------------------------------
         * Step 3: Forward - Send the updated token to the next process
         * --------------------------------------------------------------------
         * This is a BLOCKING send - the process will wait until the
         * message is safely buffered or received by the next process.
         */
        MPI_Send(&token, 1, MPI_LONG_LONG, next_rank, tag, MPI_COMM_WORLD);
    }

    /* ========================================================================
     * PHASE 6: MPI Environment Cleanup
     * ========================================================================
     * MPI_Finalize must be called by all processes before program exit.
     * It cleans up MPI state and releases resources.
     * No MPI functions can be called after MPI_Finalize.
     */
    MPI_Finalize();
    
    return 0;
}
