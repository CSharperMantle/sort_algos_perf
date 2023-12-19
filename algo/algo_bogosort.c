#include "pch.h"
#include "common.h"
#include "algo_bogosort.h"

algo_errno_t bogosort_determ(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *out_counter) {
    uint8_t *arr_ = arr;

    *out_counter = 0;

    size_t *aux = (size_t *)malloc((count + 1) * sizeof(size_t));
    PTR_ASSERT_NONNULL(aux);

    for (size_t i = 0; i < count + 1; i++) {
        aux[i] = i;
    }

    size_t i = 1;
    while (i < count) {
        size_t j = 0;
        aux[i]--;
        if (i % 2 == 1) {
            j = aux[i];
        }
        swap_bytes(arr_ + i * size, arr_ + j * size, size);

        bool is_same = true;
        for (size_t k = 0; k < count - 1; k++) {
            COUNTER_INC_AND_CHECK(*out_counter, threshold);
            if (comp(arr_ + k * size, arr_ + (k + 1) * size) > 0) {
                is_same = false;
                break;
            }
        }
        if (is_same) {
            break;
        }

        i = 1;
        while (aux[i] == 0) {
            aux[i] = i;
            i++;
        }
    }

    free(aux);

    return OK;
}
