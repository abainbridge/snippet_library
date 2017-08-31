#pragma once


// Standard headers
#include <stdlib.h>
#include <stdint.h>


// hash_table_add() needs some temporary space to store items while inserting. It uses the stack
// because it is faster than the heap. The downside is, we need to define a fixed size of the
// temporary buffer.
#define MAX_ITEM_SIZE 200


typedef struct
{
    // Table is an array of structs - NOT pointers to structs.
    // The struct definition can be anything you want except that the first
    // 32-bits must be the key for the item. And, the key value 0 is reserved
    // and is used to mark slots in the table as empty.
    void *table;
    
    uint32_t size_mask; // Equal to size/2 - 1. Size is a power of 2.
						// It also happens to be approximately the maximum number
						// of items the table can store due to the fact that the
						// max load factor for the table is ~50%.

    // sizeof the items stored in table. Not size_t to save space on 64-bit targets.
    uint32_t sizeof_item;
} HashTable;


// Num items will be rounded up the nearest power of 2. The maximum number of
// items that the table will actually be able to store will be approximately
// 50% of that figure.
HashTable hash_table_create(uint32_t num_items, size_t sizeof_item);

// Allocates a slot in the table and returns a pointer to it.
// Returns NULL if the item is already present.
// If the table is full, the program will print an error message and terminate.
void *hash_table_add(HashTable ht, void *item);

void *hash_table_lookup(HashTable ht, uint32_t key);
