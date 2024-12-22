#ifndef HASHSET_H
#define HASHSET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Define the key type
#ifndef KEY_T
#define KEY_T uint32_t
#endif

typedef KEY_T hash_key_t;

// Define the empty key marker
#ifndef K_EMPTY_SET
#define K_EMPTY_SET ((hash_key_t)(-1))
#endif

// Initialize the hash set with specified capacity
hash_key_t* initialize_hashset(size_t capacity);

// Insert a key into the hash set
void hashset_insert(hash_key_t* hashset, size_t capacity, hash_key_t key);

// Check if a key exists in the hash set
bool hashset_contains(hash_key_t* hashset, size_t capacity, hash_key_t key);

// Delete a key from the hash set
void hashset_delete(hash_key_t* hashset, size_t capacity, hash_key_t key);

// Batch insert keys into the hash set
void hashset_insert_batch(hash_key_t* hashset, size_t capacity, hash_key_t* keys, unsigned int num_keys);

// Batch check keys in the hash set
void hashset_contains_batch(hash_key_t* hashset, size_t capacity, hash_key_t* keys, unsigned int num_keys, bool* results);

// Batch delete keys from the hash set
void hashset_delete_batch(hash_key_t* hashset, size_t capacity, hash_key_t* keys, unsigned int num_keys);

// Generate random keys with potential duplicates
hash_key_t* generate_keys(unsigned int num_keys, size_t capacity);

// Compute the next power of two
size_t next_power_of_two(size_t x);

#endif // HASHSET_H
