// This is a lock-free FIFO, for single consumer, single producer.

// Only tested on Linux, GCC, x86_64. Probably needs some compiler fences adding.
// Definitely needs to memory fences adding for use on ARM.


#pragma once

#include <stdlib.h>


#define ALIGN64 __attribute__((aligned(64)))


// If read_idx == write_idx, queue is empty.
typedef struct lf_fifo_t {
    char *buf;
    int item_size;
    int ALIGN64 max_items_minus_1;      // Must be a power of 2 minus 1.
    volatile ALIGN64 int read_idx;      // Index of next item that consumer will read from.
    volatile ALIGN64 int write_idx;     // Index of next item that producer will write to.
                                        // ALIGN64 keeps each variable in a seperate cache line,
                                        // which reduces the cache-synchronisation needed when
                                        // the consumer and producer are on different cores.
} lf_fifo_t;


void lf_fifo_init(int item_size, int max_items, lf_fifo_t *f);
void lf_fifo_destroy(lf_fifo_t *f);


// Returns the number of items that can be added before the FIFO is full.
static inline int lf_fifo_calc_amount_space(lf_fifo_t *f) {
    int amt_readable = (f->write_idx - f->read_idx) & f->max_items_minus_1;
    int amt_space = f->max_items_minus_1 - amt_readable;
    return amt_space;
}


// Returns NULL if no space.
static inline void *lf_fifo_get_space(lf_fifo_t *f) {
    int amt_space = lf_fifo_calc_amount_space(f);
    if (amt_space < 1) {
        return NULL;
    }

    return f->buf + f->write_idx * f->item_size;
}


// A dangerous version of lf_fifo_get_space(). The intention is to allow
// increased performance when writing a batch of items to the FIFO. You can
// call lf_fifo_calc_amount_space() once up front, and then know you have
// enough space to call this function multiple times.
static inline void *lf_fifo_get_space_no_check(lf_fifo_t *f) {
    return f->buf + f->write_idx * f->item_size;
}


static inline void lf_fifo_commit(lf_fifo_t *f) {
    f->write_idx = (f->write_idx + 1) & f->max_items_minus_1;
}


// Returns NULL if no data.
static inline void *lf_fifo_get_data(lf_fifo_t *f) {
    int amt_readable = (f->write_idx - f->read_idx) & f->max_items_minus_1;
    if (amt_readable == 0) {
        return NULL;
    }

    return f->buf + f->read_idx * f->item_size;
}


static inline void lf_fifo_consume(lf_fifo_t *f) {
    f->read_idx = (f->read_idx + 1) & f->max_items_minus_1;
}
