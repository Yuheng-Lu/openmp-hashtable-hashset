#include "hashset.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>
#include <stdbool.h>

void serial_insert_set(hash_key_t* hashset, size_t capacity, hash_key_t* keys, unsigned int num_keys) {
    for (unsigned int i = 0; i < num_keys; ++i) {
        hashset_insert(hashset, capacity, keys[i]);
    }
}

void serial_contains_set(hash_key_t* hashset, size_t capacity, hash_key_t* keys, unsigned int num_keys, bool* results) {
    for (unsigned int i = 0; i < num_keys; ++i) {
        results[i] = hashset_contains(hashset, capacity, keys[i]);
    }
}

void serial_delete_set(hash_key_t* hashset, size_t capacity, hash_key_t* keys, unsigned int num_keys) {
    for (unsigned int i = 0; i < num_keys; ++i) {
        hashset_delete(hashset, capacity, keys[i]);
    }
}

int main(int argc, char* argv[]) {
    // Number of keys for benchmarking
    unsigned int num_keys = 10000000; 
    int num_threads = 4;  // Default to 4 threads

    if (argc > 1) {
        num_keys = atoi(argv[1]);
    }
    if (argc > 2) {
        num_threads = atoi(argv[2]);
    }

    omp_set_num_threads(num_threads);

    printf("Benchmarking Lock-Free Hash Set with OpenMP\n");
    printf("Number of Keys: %u\n\n", num_keys);
    printf("Number of Threads: %d\n\n", num_threads);

    // Compute capacity as next power of two greater than or equal to num_keys
    size_t capacity = next_power_of_two(num_keys);
    printf("Hash Set Capacity: %zu\n\n", capacity);

    // Generate test data
    hash_key_t* keys = generate_keys(num_keys, capacity);
    bool* lookup_results_parallel = (bool*)malloc(sizeof(bool) * num_keys);
    bool* lookup_results_serial = (bool*)malloc(sizeof(bool) * num_keys);
    if (!lookup_results_parallel || !lookup_results_serial) {
        perror("Failed to allocate lookup results");
        exit(EXIT_FAILURE);
    }

    // Initialize hash sets for parallel and serial implementations
    printf("Initializing hash sets...\n");
    hash_key_t* hashset_parallel = initialize_hashset(capacity);
    hash_key_t* hashset_serial = initialize_hashset(capacity); // Separate set for serial
    printf("Initialization complete.\n\n");

    // ------ Parallel Insert ------ //
    printf("Starting Parallel Insert...\n");
    double start = omp_get_wtime();
    hashset_insert_batch(hashset_parallel, capacity, keys, num_keys);
    double end = omp_get_wtime();
    double parallel_insert_time = end - start;
    printf("Parallel Insert Time: %f seconds\n", parallel_insert_time);

    // ------ Serial Insert ------ //
    printf("Starting Serial Insert (Baseline)...\n");
    start = omp_get_wtime();
    serial_insert_set(hashset_serial, capacity, keys, num_keys);
    end = omp_get_wtime();
    double serial_insert_time = end - start;
    printf("Serial Insert Time: %f seconds\n\n", serial_insert_time);

    // ------ Parallel Lookup ------ //
    printf("Starting Parallel Lookup...\n");
    start = omp_get_wtime();
    hashset_contains_batch(hashset_parallel, capacity, keys, num_keys, lookup_results_parallel);
    end = omp_get_wtime();
    double parallel_lookup_time = end - start;
    printf("Parallel Lookup Time: %f seconds\n", parallel_lookup_time);

    // ------ Serial Lookup ------ //
    printf("Starting Serial Lookup (Baseline)...\n");
    start = omp_get_wtime();
    serial_contains_set(hashset_serial, capacity, keys, num_keys, lookup_results_serial);
    end = omp_get_wtime();
    double serial_lookup_time = end - start;
    printf("Serial Lookup Time: %f seconds\n\n", serial_lookup_time);

    // ------ Parallel Delete ------ //
    printf("Starting Parallel Delete...\n");
    start = omp_get_wtime();
    hashset_delete_batch(hashset_parallel, capacity, keys, num_keys);
    end = omp_get_wtime();
    double parallel_delete_time = end - start;
    printf("Parallel Delete Time: %f seconds\n", parallel_delete_time);

    // ------ Serial Delete ------ //
    printf("Starting Serial Delete (Baseline)...\n");
    start = omp_get_wtime();
    serial_delete_set(hashset_serial, capacity, keys, num_keys);
    end = omp_get_wtime();
    double serial_delete_time = end - start;
    printf("Serial Delete Time: %f seconds\n\n", serial_delete_time);

    // ------ Performance Summary ------ //
    printf("Performance Summary:\n");
    printf("---------------------\n");
    printf("Insert - Parallel: %f s | Serial: %f s | Speedup: %.2fx\n", 
           parallel_insert_time, serial_insert_time, serial_insert_time / parallel_insert_time);
    printf("Lookup - Parallel: %f s | Serial: %f s | Speedup: %.2fx\n", 
           parallel_lookup_time, serial_lookup_time, serial_lookup_time / parallel_lookup_time);
    printf("Delete - Parallel: %f s | Serial: %f s | Speedup: %.2fx\n", 
           parallel_delete_time, serial_delete_time, serial_delete_time / parallel_delete_time);

    // Cleanup
    free(hashset_parallel);
    free(hashset_serial);
    free(keys);
    free(lookup_results_parallel);
    free(lookup_results_serial);

    return 0;
}
