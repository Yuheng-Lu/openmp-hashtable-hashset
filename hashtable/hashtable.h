#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Define the key type
#ifndef KEY_T
#define KEY_T uint32_t
#endif

typedef KEY_T hash_key_t;

// Define the value type
#ifndef VALUE_T
#define VALUE_T uint32_t
#endif

typedef VALUE_T value_t;

// Define the empty key marker
#ifndef K_EMPTY
#define K_EMPTY ((hash_key_t)(-1))
#endif

// KeyValue structure
typedef struct {
    hash_key_t key;
    value_t value;
} KeyValue;

// Initialize the hash table with specified capacity
KeyValue* initialize_hashtable(size_t capacity);

// Insert a key-value pair into the hash table
void hashtable_insert(KeyValue* hashtable, size_t capacity, hash_key_t key, value_t value);

// Lookup a key in the hash table
value_t hashtable_lookup(KeyValue* hashtable, size_t capacity, hash_key_t key);

// Delete a key from the hash table
void hashtable_delete(KeyValue* hashtable, size_t capacity, hash_key_t key);

// Batch insert key-value pairs
void hashtable_insert_batch(KeyValue* hashtable, size_t capacity, KeyValue* kvs, unsigned int numkvs);

// Batch lookup keys
void hashtable_lookup_batch(KeyValue* hashtable, size_t capacity, KeyValue* kvs, unsigned int numkvs, value_t* results);

// Batch delete keys
void hashtable_delete_batch(KeyValue* hashtable, size_t capacity, KeyValue* kvs, unsigned int numkvs);

// Generate random key-value pairs
KeyValue* generate_kv_pairs(unsigned int numkvs, size_t capacity);

// Compute the next power of two
size_t next_power_of_two(size_t x);

#endif // HASHTABLE_H
