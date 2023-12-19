#include "pch.h"
#include "common.h"

void swap_bytes(void *restrict a, void *restrict b, size_t size) {
    uint8_t *a_ = a;
    uint8_t *b_ = b;
    for (size_t i = 0; i < size; i++) {
        uint8_t tmp = a_[i];
        a_[i] = b_[i];
        b_[i] = tmp;
    }
}
