// Implements a simple cuckoo hashing scheme using 2 hash algorithms, indexing into a single table.

// Own header
#include "hash_table.h"

// Project headers
#include "error_util.h"

// Standard headers
#include <memory.h>
#include <stdlib.h>


HashTable hash_table_create(uint32_t num_items, size_t sizeof_item)
{
    if (sizeof_item < 4) {
        FATAL_ERROR("Hash table item not large enough to include key.");
    }
    if (sizeof_item > MAX_ITEM_SIZE) {
        FATAL_ERROR("Hash table item too large.");
    }

    // The table size must be a power of two (so that we can efficiently limit the output of the
    // hash functions to be a valid table index). We will round up the specified size to the
    // nearest power of 2. We don't actually need to store the size, just a bitmask that the
    // hash function will use. Actually, it is easy to calculate the mask without having to do the
    // intermediate step of rounding up to a power of 2.
    uint32_t mask = num_items;
    mask |= (mask >> 1); mask |= (mask >> 2);
    mask |= (mask >> 4); mask |= (mask >> 8);
    mask |= (mask >> 16);

    HashTable ht;
    ht.sizeof_item = (uint32_t)sizeof_item;
    ht.table = calloc(mask + 1, sizeof_item);
    ht.size_mask = mask / 2; // We only allocate a single table, but we split in two. The mask size is
                             // halved so to make it work for these two half-sized tables.
    if (!ht.table) {
        FATAL_ERROR("Out of memory allocating hash table");
    }

    return ht;
}


static inline void *find_slot_table1(HashTable ht, uint32_t key)
{
    uint32_t index = key & ht.size_mask;
    uint8_t *bytes = ht.table;
    return bytes + index * ht.sizeof_item;
}


static inline void *find_slot_table2(HashTable ht, uint32_t key)
{
    uint32_t index = (~key >> 8) & ht.size_mask;
    uint8_t *bytes = ht.table;
    return bytes + (index + ht.size_mask + 1) * ht.sizeof_item;
}


#define slot_key(slot) (*((uint32_t*)(slot)))


void *hash_table_lookup(HashTable ht, uint32_t key)
{
    void *value = find_slot_table1(ht, key);
    if (slot_key(value) == key) {
        return value;
    }

    value = find_slot_table2(ht, key);
    if (slot_key(value) == key) {
        return value;
    }

    return NULL;
}


// Insert the key in the first of the two tables.
// If that position was occupied, insert the old value into the second table.
// If that position was occupied, continue the loop.
void *hash_table_add(HashTable ht, void *item)
{
    void *rv = NULL;
    char old_val[MAX_ITEM_SIZE];
    char old_val_2[MAX_ITEM_SIZE];

    for (int i = 0; ; i++) {
        uint32_t key = slot_key(item);
        if (key == 0) {
            FATAL_ERROR("Bad key");
        }
        void *value = find_slot_table1(ht, key);
        if (slot_key(value) == key)
            return NULL;

        if (!rv) {
            rv = value;
        }

        if (slot_key(value) == 0) {
            // First slot is empty. Insert there and return.
            memcpy(value, item, ht.sizeof_item);
            break;
        }
        else {
            // First slot not empty. Copy existing to old_val. Then overwrite.
            memcpy(old_val, value, ht.sizeof_item);
            memcpy(value, item, ht.sizeof_item);

            value = find_slot_table2(ht, key);
            if (slot_key(value) == 0) {
                // Second slot is empty. Insert there and return.
                memcpy(value, old_val, ht.sizeof_item);
                break;
            }
            else {
                // Second slot not empty. Copy existing to old_val_2. Then overwrite.
                memcpy(old_val_2, value, ht.sizeof_item);
                memcpy(value, old_val, ht.sizeof_item);

                // Prepare to iterate.
                item = old_val_2;
            }
        }

        if (i > 100) {
            FATAL_ERROR("Too many collisions on hash table insert.");
        }
    }

    return rv;
}


void hash_table_remove(HashTable ht, uint32_t key)
{
    // If the second hash function finds the specified ue_ip_address, then we simply clear that
    // slot. If not, then we try with the first hash function. If that finds it, we need to copy
    // the data out of the slot the second hash function gave and clear the second.

    void *slot2 = find_slot_table2(ht, key);
    if (slot_key(slot2) != key) {
        void *slot1 = find_slot_table1(ht, key);
        if (slot_key(slot1) != key) {   
            FATAL_ERROR("Key 0x%x not found when attempting to remove from hash table.", key);
        }

        memcpy(slot1, slot2, ht.sizeof_item);
    }

    slot_key(slot2) = 0;
}
