#include "pch.h"
#include "common.h"
#include "algo_selection_sort.h"

algo_errno_t selection_sort(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *out_counter) {
    uint8_t *arr_ = arr;

    for (size_t i = 0; i < count; i++) {
        size_t min = i;
        for (size_t j = i + 1; j < count; j++) {
            COUNTER_INC_AND_CHECK(*out_counter, threshold);
            if (comp(arr_ + j * size, arr_ + min * size) < 0) {
                min = j;
            }
        }
        swap_bytes(arr_ + i * size, arr_ + min * size, size);
    }

    return OK;
}
