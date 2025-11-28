# MPI Ring Pattern Demo

**Course**: Parallel Programming and Supercomputer Collaborative Design (PPSC)  
**Student**: Fan Zhanhong  
**Student ID**: 58

---

## 📋 Project Overview

This project implements an **MPI Ring Exchange Pattern** for distributed parallel computing. The program demonstrates inter-process communication in a ring topology, where data (a token) is passed sequentially through all processes in a circular manner.

### Assignment Parameters (Based on Student ID: 58)

| Parameter | Formula | Calculation | Result |
|-----------|---------|-------------|--------|
| **Pattern** | `(58/4) % 10 + 1` | `14 % 10 + 1` | **5 - Ring Exchange** |
| **Data Type** | `58 % 4 + 1` | `2 + 1` | **3 - long long int** |

---

## 🎯 Problem Description

The program performs a **Distributed Ring Summation**:

1. Each process holds a local value: `local_value = rank × 1000`
2. A token starts at Process 0 and travels around the ring
3. Each process adds its local value to the token
4. The final sum is verified using the arithmetic series formula

### Why This Task?

This task was chosen for the following reasons:

1. **Perfect Match for Ring Pattern**: The accumulative summation requires a "token" to travel through all processes in sequence, which is the classic textbook application of the Ring Exchange pattern.

2. **Demonstrates Point-to-Point Communication**: The task uses only `MPI_Send` and `MPI_Recv` without relying on collective operations (like `MPI_Reduce`), clearly showing mastery of the ring communication logic.

3. **Justifies 64-bit Data Type**: When the number of processes is large or each process holds a big value, the sum may overflow a 32-bit integer. Using `long long int` is both reasonable and necessary.

4. **Easy to Verify**: The result can be mathematically verified using the arithmetic series formula: $\text{Sum} = 1000 \times \frac{(N-1) \times N}{2}$, ensuring correctness.

5. **Scalable**: The same logic works for any number of processes, from 2 to thousands.

### Ring Topology

```
    ┌─────────────────────────────────────┐
    │                                     │
    ▼                                     │
┌───────┐     ┌───────┐     ┌───────┐     │
│ Rank 0│────▶│ Rank 1│────▶│ Rank 2│────▶│ ... ────▶ Rank N-1
└───────┘     └───────┘     └───────┘              │
    ▲                                              │
    └──────────────────────────────────────────────┘
```

---

## 🔧 Technical Details

### Algorithm Design

1. **Initialization Phase**
   - Each process calculates its local value
   - Define communication neighbors: `next = (rank + 1) % size`, `prev = (rank - 1 + size) % size`

2. **Communication Phase**
   - **Rank 0 (Master)**: Sends initial token, waits for final result
   - **Rank 1 to N-1 (Workers)**: Receive → Add → Send

3. **Verification Phase**
   - Compare result with expected sum: `1000 × (N-1) × N / 2`

### MPI Functions Used

| Function | Purpose |
|----------|---------|
| `MPI_Init` | Initialize MPI environment |
| `MPI_Finalize` | Terminate MPI environment |
| `MPI_Comm_rank` | Get process rank |
| `MPI_Comm_size` | Get total number of processes |
| `MPI_Send` | Blocking send to neighbor |
| `MPI_Recv` | Blocking receive from neighbor |

### Data Type

- **C++ Type**: `long long int` (64-bit signed integer)
- **MPI Type**: `MPI_LONG_LONG`
- **Reason**: Prevents overflow when summing large values across many processes

---

## 🚀 Quick Start

### Prerequisites

- Linux operating system (Ubuntu, CentOS, etc.)
- OpenMPI or MPICH installed
- C++ compiler (g++)

### Installation (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y openmpi-bin libopenmpi-dev
```

### Build

```bash
make
```

### Run

```bash
# Run with 4 processes
make run

# Or manually:
mpirun --oversubscribe --allow-run-as-root -np 4 ./ring_sum

# Run with custom number of processes
make run-np NP=8
```

### Clean

```bash
make clean
```

---

## 📊 Example Output

```
[Process 0] MPI Ring Pattern Demo by Fan Zhanhong (ID: 58)
[Process 0] Topology: Ring size = 4. Data Type: long long int.
[Process 0] Starting token with value: 0
[Process 1] Received token 0, added 1000, sending 1000 to Process 2
[Process 2] Received token 1000, added 2000, sending 3000 to Process 3
[Process 3] Received token 3000, added 3000, sending 6000 to Process 0
[Process 0] Final Result received from Process 3: 6000
[Process 0] Verification: SUCCESS (Expected: 6000)
```

### Calculation Verification (4 Processes)

| Process | Local Value | Token Before | Token After |
|---------|-------------|--------------|-------------|
| 0 | 0 | - | 0 |
| 1 | 1000 | 0 | 1000 |
| 2 | 2000 | 1000 | 3000 |
| 3 | 3000 | 3000 | 6000 |

Expected Sum = `1000 × (0 + 1 + 2 + 3)` = `1000 × 6` = **6000** ✓

---

## 💡 How This Project Demonstrates Parallel Computing

### 1. Distributed Computation with Independent Processes

```cpp
long long local_value = (long long)world_rank * 1000;
```

Each process computes its own `local_value` **independently and simultaneously**. With 8 processes, all 8 values are computed at the same time, not sequentially. This is the essence of parallelism: **work is divided among multiple workers**.

### 2. Message Passing Instead of Shared Memory

```cpp
MPI_Send(&token, 1, MPI_LONG_LONG, next_rank, tag, MPI_COMM_WORLD);
MPI_Recv(&token, 1, MPI_LONG_LONG, prev_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
```

Processes do **not** share memory. They communicate explicitly through `MPI_Send` and `MPI_Recv`. This model:
- Scales to **thousands of machines** across networks
- Avoids race conditions and data corruption
- Is the foundation of **supercomputer programming**

### 3. Ring Topology: Structured Parallel Communication

```cpp
int next_rank = (world_rank + 1) % world_size;
int prev_rank = (world_rank - 1 + world_size) % world_size;
```

The ring topology demonstrates:
- **Neighbor communication**: Each process only talks to its neighbors
- **Scalability**: Adding more processes doesn't increase communication complexity per process
- **Pipeline parallelism**: Data flows through processes like an assembly line

### 4. Parallel Reduction Pattern

The token accumulation (`token += local_value`) implements a **parallel reduction**:

| Step | What Happens | Parallel Benefit |
|------|--------------|------------------|
| Process 0 | Initializes token = 0 | Master coordinates |
| Process 1-7 | Each adds its value | **Simultaneous local computation** |
| Final | Sum = 28000 | Distributed workload |

In real applications, this pattern handles:
- Summing billions of numbers across a cluster
- Aggregating sensor data from thousands of IoT devices
- Computing global statistics in distributed databases

### 5. Verification: Ensuring Correctness in Parallel Systems

```cpp
long long expected_sum = 1000LL * (long long)(world_size - 1) * (long long)world_size / 2;
if (token == expected_sum) { /* SUCCESS */ }
```

Parallel programs are harder to debug. This verification step shows:
- **Deterministic validation**: Mathematical formula guarantees expected result
- **Fault detection**: Any communication error would cause mismatch

### Summary: From Simple Demo to Real-World Impact

| This Project | Real Parallel Systems |
|--------------|----------------------|
| 8 processes on 1 machine | 100,000+ processes across data centers |
| Token = sum of ranks | Token = partial results of complex simulations |
| Ring of 8 nodes | Ring of GPU clusters for deep learning |
| Verify with formula | Checksums for petabyte-scale data integrity |

**This project is a microcosm of how supercomputers solve problems**: divide work, communicate results, combine answers.

---

## 📁 Project Structure

```
PPSC-homework-No.6-58/
├── README.md          # Project documentation (this file)
├── Makefile           # Build automation script
└── ring_sum.cpp       # Main source code
```
