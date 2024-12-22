#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <limits.h>

KeyValue* initialize_hashtable(size_t capacity) {
    KeyValue* hashtable = (KeyValue*)malloc(sizeof(KeyValue) * capacity);
    if (!hashtable) {
        perror("Failed to allocate hash table");
        exit(EXIT_FAILURE);
    }
    // Initialize all slots to empty in parallel
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < capacity; ++i) {
        hashtable[i].key = K_EMPTY;
        hashtable[i].value = 0; // Default value
    }
    return hashtable;
}

// Simple hash function using Knuth's multiplicative method
static inline size_t hash_key(hash_key_t key) {
    return ((size_t)key) * 2654435761u;
}

// Atomic compare and swap using GCC built-ins
static bool atomic_compare_and_swap_key(hash_key_t* ptr, hash_key_t expected, hash_key_t desired) {
    return __atomic_compare_exchange_n(ptr, &expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

// Insert a key-value pair into the hash table
void hashtable_insert(KeyValue* hashtable, size_t capacity, hash_key_t key, value_t value) {
    size_t slot = hash_key(key) & (capacity - 1);

    while (1) {
        hash_key_t prev_key = hashtable[slot].key;
        if (prev_key == K_EMPTY) {
            // Attempt to insert the key atomically
            if (atomic_compare_and_swap_key(&hashtable[slot].key, K_EMPTY, key)) {
                // Successfully inserted the key; now set the value
                __atomic_store_n(&hashtable[slot].value, value, __ATOMIC_SEQ_CST);
                break;
            }
        } else if (prev_key == key) {
            // Key already exists; update the value
            __atomic_store_n(&hashtable[slot].value, value, __ATOMIC_SEQ_CST);
            break;
        }
        // Linear probing with wrap-around
        slot = (slot + 1) & (capacity - 1);
    }
}

// Lookup a key in the hash table
value_t hashtable_lookup(KeyValue* hashtable, size_t capacity, hash_key_t key) {
    size_t slot = hash_key(key) & (capacity - 1);

    while (1) {
        hash_key_t current_key = __atomic_load_n(&hashtable[slot].key, __ATOMIC_SEQ_CST);
        if (current_key == key) {
            return __atomic_load_n(&hashtable[slot].value, __ATOMIC_SEQ_CST);
        }
        if (current_key == K_EMPTY) {
            return (value_t)0; // Default value indicating not found
        }
        // Linear probing with wrap-around
        slot = (slot + 1) & (capacity - 1);
    }
}

// Delete a key from the hash table
void hashtable_delete(KeyValue* hashtable, size_t capacity, hash_key_t key) {
    size_t slot = hash_key(key) & (capacity - 1);

    while (1) {
        hash_key_t current_key = __atomic_load_n(&hashtable[slot].key, __ATOMIC_SEQ_CST);
        if (current_key == key) {
            // Atomically set the key to K_EMPTY
            __atomic_store_n(&hashtable[slot].key, K_EMPTY, __ATOMIC_SEQ_CST);
            // Optionally set the value to default
            __atomic_store_n(&hashtable[slot].value, (value_t)0, __ATOMIC_SEQ_CST);
            return;
        }
        if (current_key == K_EMPTY) {
            return;
        }
        // Linear probing with wrap-around
        slot = (slot + 1) & (capacity - 1);
    }
}

// Batch insert key-value pairs
void hashtable_insert_batch(KeyValue* hashtable, size_t capacity, KeyValue* kvs, unsigned int numkvs) {
    #pragma omp parallel for schedule(static)
    for (unsigned int i = 0; i < numkvs; ++i) {
        hashtable_insert(hashtable, capacity, kvs[i].key, kvs[i].value);
    }
}

// Batch lookup keys
void hashtable_lookup_batch(KeyValue* hashtable, size_t capacity, KeyValue* kvs, unsigned int numkvs, value_t* results) {
    #pragma omp parallel for schedule(static)
    for (unsigned int i = 0; i < numkvs; ++i) {
        results[i] = hashtable_lookup(hashtable, capacity, kvs[i].key);
    }
}

// Batch delete keys
void hashtable_delete_batch(KeyValue* hashtable, size_t capacity, KeyValue* kvs, unsigned int numkvs) {
    #pragma omp parallel for schedule(static)
    for (unsigned int i = 0; i < numkvs; ++i) {
        hashtable_delete(hashtable, capacity, kvs[i].key);
    }
}

// Generate random key-value pairs with potential duplicates
KeyValue* generate_kv_pairs(unsigned int numkvs, size_t capacity) {
    KeyValue* kvs = (KeyValue*)malloc(sizeof(KeyValue) * numkvs);
    if (!kvs) {
        perror("Failed to allocate key-value pairs");
        exit(EXIT_FAILURE);
    }
    srand((unsigned int)time(NULL));
    for (unsigned int i = 0; i < numkvs; ++i) {
        kvs[i].key = (hash_key_t)(rand() % (capacity / 2)); // Intentional duplicates
        if (kvs[i].key == K_EMPTY) {
            kvs[i].key += 1; // Avoid K_EMPTY
        }
        kvs[i].value = (value_t)rand();
    }
    return kvs;
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
