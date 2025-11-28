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

## 📁 Project Structure

```
PPSC-homework-No.6-58/
├── README.md          # Project documentation (this file)
├── technology.md      # Technical design document
├── Makefile           # Build automation script
└── ring_sum.cpp       # Main source code
```

---

## 🐳 Docker Support

If running in a Docker container as root:

```bash
# Option 1: Add command line flag
mpirun --oversubscribe --allow-run-as-root -np 4 ./ring_sum

# Option 2: Set environment variables
export OMPI_ALLOW_RUN_AS_ROOT=1
export OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1
mpirun --oversubscribe -np 4 ./ring_sum
```

---

## 📝 License

This project is for educational purposes as part of the PPSC course assignment.
