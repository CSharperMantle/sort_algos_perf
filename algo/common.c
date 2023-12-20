#include "pch.h"
#include "common.h"

#define DEF_SWAP_BYTES_NB_(n_)  \
static void swap_bytes_##n_##b(void *a, void *b) {  \
    uint8_t *a_ = a;  \
    uint8_t *b_ = b;  \
    uint8_t tmp[n_];  \
    memcpy_s(tmp, sizeof(tmp), a_, sizeof(tmp));  \
    memcpy_s(a_, sizeof(tmp), b_, sizeof(tmp));  \
    memcpy_s(b_, sizeof(tmp), tmp, sizeof(tmp));  \
}

DEF_SWAP_BYTES_NB_(1)
DEF_SWAP_BYTES_NB_(2)
DEF_SWAP_BYTES_NB_(4)
DEF_SWAP_BYTES_NB_(8)
DEF_SWAP_BYTES_NB_(16)
DEF_SWAP_BYTES_NB_(32)
DEF_SWAP_BYTES_NB_(64)

#undef DEF_SWAP_BYTES_NB_

#define DEF_CASE_SWAP_BYTES_NB_(n_, a_, b_)  \
case n_:  \
    swap_bytes_##n_##b(a_, b_);  \
    break

void swap_bytes(void *a, void *b, size_t size) {
    uint8_t *a_ = a;
    uint8_t *b_ = b;

    switch (size) {
        DEF_CASE_SWAP_BYTES_NB_(1, a, b);
        DEF_CASE_SWAP_BYTES_NB_(2, a, b);
        DEF_CASE_SWAP_BYTES_NB_(4, a, b);
        DEF_CASE_SWAP_BYTES_NB_(8, a, b);
        DEF_CASE_SWAP_BYTES_NB_(16, a, b);
        DEF_CASE_SWAP_BYTES_NB_(32, a, b);
        DEF_CASE_SWAP_BYTES_NB_(64, a, b);
    default:
        for (size_t i = 0; i < size; i++) {
            uint8_t tmp = a_[i];
            a_[i] = b_[i];
            b_[i] = tmp;
        }
        break;
    }
}

#undef DEF_CASE_SWAP_BYTES_NB_
