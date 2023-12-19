#include "pch.h"
#include "common.h"
#include "algo_bubblesort.h"

algo_errno_t bubblesort(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *out_counter) {
    uint8_t *arr_ = arr;

    for (size_t i = 0; i < count; i++) {
        for (size_t j = 0; j < count - i - 1; j++) {
            COUNTER_INC_AND_CHECK(*out_counter, threshold);
            if (comp(arr_ + j * size, arr_ + (j + 1) * size) > 0) {
                swap_bytes(arr_ + j * size, arr_ + (j + 1) * size, size);
            }
        }
    }

    return OK;
}
