#include "pch.h"
#include "common.h"
#include "algo_eradix_sort.h"

#define NYBBLES_IN_BYTES_(b_) ((b_) * (CHAR_BIT / 4))
#define GET_NYBBLE_LOC_NTH_BYTE_(n_nybble_) ((n_nybble_) / NYBBLES_IN_BYTES_(1))
#define GET_NYBBLE_LOC_OFFSET_(n_nybble_) ((n_nybble) % NYBBLES_IN_BYTES_(1))
#define GET_NTH_NYBBLE_(a_, n_nybble_) (((a_)[GET_NYBBLE_LOC_NTH_BYTE_(n_nybble_)] >> (GET_NYBBLE_LOC_OFFSET_(n_nybble_) * (CHAR_BIT / NYBBLES_IN_BYTES_(1)))) & 0xFu)

static algo_errno_t count_sort(void *arr, size_t count, size_t size, size_t n_nybble) {
    uint8_t *arr_ = arr;
    size_t cnt_nybble[0x10u] = {0};

    uint8_t *output = (uint8_t *)malloc(count * size * sizeof(uint8_t));
    PTR_ASSERT_NONNULL(output);

    for (size_t i = 0; i < count; i++) {
        uint8_t nybble = GET_NTH_NYBBLE_(arr_ + i * size, n_nybble);
        cnt_nybble[nybble]++;
    }

    for (size_t i = 0x1u; i <= 0xFu; i++) {
        cnt_nybble[i] += cnt_nybble[i - 1];
    }

    for (size_t i = count - 1; i != SIZE_MAX; i--) {
        uint8_t nybble = GET_NTH_NYBBLE_(arr_ + i * size, n_nybble);
        uint8_t *elem_pos = output + (cnt_nybble[nybble] - 1) * size;
        memcpy(elem_pos, arr_ + i * size, size * sizeof(uint8_t));
        cnt_nybble[nybble]--;
    }

    memcpy(arr_, output, count * size * sizeof(uint8_t));

    free(output);
    return OK;
}

algo_errno_t eradix_sort(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *out_counter) {
    *out_counter = 0;

    size_t nybbles = NYBBLES_IN_BYTES_(size);

    for (size_t n_nybble = 0; n_nybble < nybbles; n_nybble++) {
        ALGO_ERRNO_UNWRAP(count_sort(arr, count, size, n_nybble));
    }

    return OK;
}
