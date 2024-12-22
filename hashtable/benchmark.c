#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

// Function to perform serial insertions (baseline)
void serial_insert(KeyValue* hashtable, size_t capacity, KeyValue* kvs, unsigned int numkvs) {
    for (unsigned int i = 0; i < numkvs; ++i) {
        hashtable_insert(hashtable, capacity, kvs[i].key, kvs[i].value);
    }
}

// Function to perform serial lookups (baseline)
void serial_lookup(KeyValue* hashtable, size_t capacity, KeyValue* kvs, unsigned int numkvs, value_t* results) {
    for (unsigned int i = 0; i < numkvs; ++i) {
        results[i] = hashtable_lookup(hashtable, capacity, kvs[i].key);
    }
}

// Function to perform serial deletions (baseline)
void serial_delete(KeyValue* hashtable, size_t capacity, KeyValue* kvs, unsigned int numkvs) {
    for (unsigned int i = 0; i < numkvs; ++i) {
        hashtable_delete(hashtable, capacity, kvs[i].key);
    }
}

int main(int argc, char* argv[]) {
    // Number of keys for benchmarking
    unsigned int numkvs = 10000000; 
    int num_threads = 4;  // Default to 4 threads

    if (argc > 1) {
        numkvs = atoi(argv[1]);
    }
    if (argc > 2) {
        num_threads = atoi(argv[2]);
    }

    omp_set_num_threads(num_threads);

    printf("Benchmarking Lock-Free Hash Table with OpenMP\n");
    printf("Number of Key-Value Pairs: %u\n\n", numkvs);
    printf("Number of Threads: %d\n\n", num_threads);

    // Compute capacity as next power of two greater than or equal to numkvs
    size_t capacity = next_power_of_two(numkvs);
    printf("Hash Table Capacity: %zu\n\n", capacity);

    // Generate test data
    KeyValue* kvs = generate_kv_pairs(numkvs, capacity);
    value_t* lookup_results = (value_t*)malloc(sizeof(value_t) * numkvs);
    if (!lookup_results) {
        perror("Failed to allocate lookup results");
        exit(EXIT_FAILURE);
    }

    // Initialize hash tables for parallel and serial implementations
    printf("Initializing hash tables...\n");
    KeyValue* hashtable_parallel = initialize_hashtable(capacity);
    KeyValue* hashtable_serial = initialize_hashtable(capacity); // Separate table for serial
    printf("Initialization complete.\n\n");

    // ------ Parallel Insert ------ //
    printf("Starting Parallel Insert...\n");
    double start = omp_get_wtime();
    hashtable_insert_batch(hashtable_parallel, capacity, kvs, numkvs);
    double end = omp_get_wtime();
    double parallel_insert_time = end - start;
    printf("Parallel Insert Time: %f seconds\n", parallel_insert_time);

    // ------ Serial Insert ------ //
    printf("Starting Serial Insert (Baseline)...\n");
    start = omp_get_wtime();
    serial_insert(hashtable_serial, capacity, kvs, numkvs);
    end = omp_get_wtime();
    double serial_insert_time = end - start;
    printf("Serial Insert Time: %f seconds\n\n", serial_insert_time);

    // ------ Parallel Lookup ------ //
    printf("Starting Parallel Lookup...\n");
    start = omp_get_wtime();
    hashtable_lookup_batch(hashtable_parallel, capacity, kvs, numkvs, lookup_results);
    end = omp_get_wtime();
    double parallel_lookup_time = end - start;
    printf("Parallel Lookup Time: %f seconds\n", parallel_lookup_time);

    // ------ Serial Lookup ------ //
    printf("Starting Serial Lookup (Baseline)...\n");
    start = omp_get_wtime();
    serial_lookup(hashtable_serial, capacity, kvs, numkvs, lookup_results);
    end = omp_get_wtime();
    double serial_lookup_time = end - start;
    printf("Serial Lookup Time: %f seconds\n\n", serial_lookup_time);

    // ------ Parallel Delete ------ //
    printf("Starting Parallel Delete...\n");
    start = omp_get_wtime();
    hashtable_delete_batch(hashtable_parallel, capacity, kvs, numkvs);
    end = omp_get_wtime();
    double parallel_delete_time = end - start;
    printf("Parallel Delete Time: %f seconds\n", parallel_delete_time);

    // ------ Serial Delete ------ //
    printf("Starting Serial Delete (Baseline)...\n");
    start = omp_get_wtime();
    serial_delete(hashtable_serial, capacity, kvs, numkvs);
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
    free(hashtable_parallel);
    free(hashtable_serial);
    free(kvs);
    free(lookup_results);

    return 0;
}
