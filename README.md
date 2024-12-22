# OpenMP Hash Sets and Hash Tables

This project contains implementations of hash sets and hash tables for multicore environments.

## Overview

This folder contains two implementations for data structures: hashset and hashtable. Each subfolder includes the necessary source files and a Makefile to compile and benchmark the implementation.

## Structure

```
.
├── hashset
│   ├── Makefile
│   ├── benchmark.c
│   ├── hashset.c
│   ├── hashset.h
│   └── runexp.sh
└── hashtable
    ├── Makefile
    ├── benchmark.c
    ├── hashtable.c
    ├── hashtable.h
    └── runexp.sh
```

## Instructions

### hashset

1. **Compile and Run** (default `uint32_t` key type, 10,000,000 keys, and 4 threads):

   ```bash
   make
   ./benchmark
   ```

2. **Compile with `char` key type**:

   ```bash
   make clean
   make KEY_T=char
   ./benchmark
   ```

3. **Specify number of keys**:
   ```bash
   ./benchmark <number_of_keys>
   ```
   The hash set capacity adjusts to the next power of two greater than or equal to the specified number of keys.

### hashtable

1. **Compile and Run** (default `uint32_t` key type, 10,000,000 key-value pairs, and 4 threads):

   ```bash
   make
   ./benchmark
   ```

2. **Compile with `char` key type**:

   ```bash
   make clean
   make KEY_T=char
   ./benchmark
   ```

3. **Specify number of key-value pairs and number of threads**:
   ```bash
   ./benchmark <number_of_pairs> <number_of_threads>
   ```
   The hash table capacity adjusts to the next power of two greater than or equal to the specified number of pairs. OpenMP will launch the number of threads specified as the second argument.

## Benchmark Script

Each folder contains a script named `runexp.sh` to automate benchmarking. The script executes the `./benchmark` program 5 times with user-provided `KEY_T` and size, computes average times for operations, and calculates speedups.

### Usage

1. Navigate to the respective folder (`hashset` or `hashtable`).

2. Make the script executable:

   ```bash
   chmod +x runexp.sh
   ```

3. Run the script with your desired `KEY_T`, size, and number_of_threads:

   ```bash
   ./runexp.sh <KEY_T> <size> <number_of_threads>
   ```

   **Example:**

   ```bash
   ./runexp.sh char 10000000 16
   ```

   The script will display the average times and speedups for Insert, Lookup, and Delete operations.

## Notes

- Ensure `make` is installed on your system.
- Run `make clean` before recompiling with a different key type.
- The `runexp.sh` script parses results directly from the `./benchmark` output.
