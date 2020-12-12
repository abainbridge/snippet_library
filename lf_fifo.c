#include "lf_fifo.h"

// Project headers
#include "util.h"

// Standard headers
#include <memory.h>
#include <stdlib.h>



void lf_fifo_init(int item_size, int max_items, lf_fifo_t *f) {
    // Round up item_size to the nearest multiple of 64 (to make sure two items
    // don't end up in the same cache line, which would cause contention
    // between the reader and writer cores).
    item_size = ((item_size - 1) / 64) * 64 + 64;

    if ((max_items - 1) & max_items) {
        FATAL_ERROR("lf_fifo_init() - max_items must be a power of 2");
    }

    f->buf = aligned_alloc(64, item_size * max_items);
    f->item_size = item_size;
    f->max_items_minus_1 = max_items - 1;
    f->read_idx = 0;
    f->write_idx = 0;
}


void lf_fifo_destroy(lf_fifo_t *f) {
    free(f->buf);
}


// #include <pthread.h>
// #include <stdio.h>
// #include <unistd.h>
//
// typedef struct item_t {
//     unsigned data[16];
// } item_t;
//
// enum { NUM_OPS = 1000 * 1000 };
//
// void *producer_proc(void *arg) {
//     lf_fifo_t *fifo = (lf_fifo_t *)arg;
//
//     item_t item;
//     item.data[0] = 0;
//     while (item.data[0] < NUM_OPS) {
//         if (lf_fifo_push(fifo, &item)) {
//             item.data[0]++;
//         }
//
//         if (rand() % 1000 == 0) {
//             printf("Producer waiting. read_idx=%i write_idx=%i\n",
//                 fifo->read_idx, fifo->write_idx);
//             usleep(rand() % 500);
//         }
//     }
//
//     puts("Finished producing");
//     return NULL;
// }
//
//
// void *consumer_proc(void *arg) {
//     lf_fifo_t *fifo = (lf_fifo_t *)arg;
//
//     unsigned next_val = 0;
//     while (next_val < NUM_OPS) {
//         item_t item;
//         if (lf_fifo_pop(fifo, &item)) {
//             if (item.data[0] != next_val) {
//                 printf("Expected %u but got %u", next_val, item.data[0]);
//                 exit(-1);
//             }
//
//             next_val++;
//         }
//
//         if (rand() % 1000 == 0) {
//             printf("Consumer waiting. read_idx=%i write_idx=%i\n",
//                 fifo->read_idx, fifo->write_idx);
//             usleep(rand() % 500);
//         }
//     }
//
//     puts("Finished consuming");
//     return NULL;
// }
//
//
// int main() {
//     lf_fifo_t fifo;
//     lf_fifo_init(sizeof(item_t), &fifo);
//
//     pthread_t t1, t2;
//     pthread_create(&t1, NULL, producer_proc, &fifo);
//     pthread_create(&t2, NULL, consumer_proc, &fifo);
//
//     pthread_join(t1, NULL);
//     pthread_join(t2, NULL);
//
//     puts("Test passed");
// }
