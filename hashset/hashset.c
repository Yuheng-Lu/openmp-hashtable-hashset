#include "hashset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <limits.h>

hash_key_t* initialize_hashset(size_t capacity) {
    hash_key_t* hashset = (hash_key_t*)malloc(sizeof(hash_key_t) * capacity);
    if (!hashset) {
        perror("Failed to allocate hash set");
        exit(EXIT_FAILURE);
    }
    // Initialize all slots to empty in parallel
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < capacity; ++i) {
        hashset[i] = K_EMPTY_SET;
    }
    return hashset;
}

// Simple hash function using Knuth's multiplicative method
static inline size_t hash_key_set(hash_key_t key) {
    return ((size_t)key) * 2654435761u;
}

// Atomic compare and swap using GCC built-ins
static bool atomic_compare_and_swap_set(hash_key_t* ptr, hash_key_t expected, hash_key_t desired) {
    return __atomic_compare_exchange_n(ptr, &expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

// Insert a key into the hash set
void hashset_insert(hash_key_t* hashset, size_t capacity, hash_key_t key) {
    size_t slot = hash_key_set(key) & (capacity - 1);

    while (1) {
        hash_key_t prev = hashset[slot];
        if (prev == K_EMPTY_SET) {
            // Attempt to insert the key atomically
            if (atomic_compare_and_swap_set(&hashset[slot], K_EMPTY_SET, key)) {
                break;
            }
        } else if (prev == key) {
            // Key already exists; nothing to do
            break;
        }
        // Linear probing with wrap-around
        slot = (slot + 1) & (capacity - 1);
    }
}

// Check if a key exists in the hash set
bool hashset_contains(hash_key_t* hashset, size_t capacity, hash_key_t key) {
    size_t slot = hash_key_set(key) & (capacity - 1);

    while (1) {
        hash_key_t current_key = __atomic_load_n(&hashset[slot], __ATOMIC_SEQ_CST);
        if (current_key == key) {
            return true;
        }
        if (current_key == K_EMPTY_SET) {
            return false;
        }
        // Linear probing with wrap-around
        slot = (slot + 1) & (capacity - 1);
    }
}

// Delete a key from the hash set
void hashset_delete(hash_key_t* hashset, size_t capacity, hash_key_t key) {
    size_t slot = hash_key_set(key) & (capacity - 1);

    while (1) {
        hash_key_t current_key = __atomic_load_n(&hashset[slot], __ATOMIC_SEQ_CST);
        if (current_key == key) {
            // Atomically set the slot to empty
            __atomic_store_n(&hashset[slot], K_EMPTY_SET, __ATOMIC_SEQ_CST);
            return;
        }
        if (current_key == K_EMPTY_SET) {
            return;
        }
        // Linear probing with wrap-around
        slot = (slot + 1) & (capacity - 1);
    }
}

// Batch insert keys into the hash set
void hashset_insert_batch(hash_key_t* hashset, size_t capacity, hash_key_t* keys, unsigned int num_keys) {
    #pragma omp parallel for schedule(static)
    for (unsigned int i = 0; i < num_keys; ++i) {
        hashset_insert(hashset, capacity, keys[i]);
    }
}

// Batch check keys in the hash set
void hashset_contains_batch(hash_key_t* hashset, size_t capacity, hash_key_t* keys, unsigned int num_keys, bool* results) {
    #pragma omp parallel for schedule(static)
    for (unsigned int i = 0; i < num_keys; ++i) {
        results[i] = hashset_contains(hashset, capacity, keys[i]);
    }
}

// Batch delete keys from the hash set
void hashset_delete_batch(hash_key_t* hashset, size_t capacity, hash_key_t* keys, unsigned int num_keys) {
    #pragma omp parallel for schedule(static)
    for (unsigned int i = 0; i < num_keys; ++i) {
        hashset_delete(hashset, capacity, keys[i]);
    }
}

// Generate random keys with potential duplicates
hash_key_t* generate_keys(unsigned int num_keys, size_t capacity) {
    hash_key_t* keys = (hash_key_t*)malloc(sizeof(hash_key_t) * num_keys);
    if (!keys) {
        perror("Failed to allocate keys");
        exit(EXIT_FAILURE);
    }
    srand((unsigned int)time(NULL));
    for (unsigned int i = 0; i < num_keys; ++i) {
        keys[i] = (hash_key_t)(rand() % (capacity / 2)); // Intentional duplicates
        if (keys[i] == K_EMPTY_SET) {
            keys[i] = (hash_key_t)(keys[i] + 1); // Avoid K_EMPTY_SET
        }
    }
    return keys;
}

size_t next_power_of_two(size_t x) {
    if (x == 0) {
        return 1;
    }
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    #if SIZE_MAX > UINT32_MAX
    x |= x >> 32;
    #endif
    return x + 1;
}
