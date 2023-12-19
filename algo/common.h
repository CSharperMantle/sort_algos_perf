#pragma once

#ifndef COMMON_H_INCLUDED_
#define COMMON_H_INCLUDED_

#include "pch.h"

typedef enum algo_errno_ {
	OK,
	INTERNAL_ERR,
	TIMED_OUT,
} algo_errno_t;

typedef int (*comp_t)(const void *, const void *);
typedef algo_errno_t (*algo_t)(void *, size_t, size_t, comp_t, uint64_t, uint64_t *);

void swap_bytes(void *restrict a, void *restrict b, size_t size);

#define PTR_ASSERT_NONNULL(p_) do { if ((p_) == NULL) { return INTERNAL_ERR; } } while (0)
#define COUNTER_CHECK(c_, t_) do { if ((c_) > (t_)) { return TIMED_OUT; } } while (0)
#define COUNTER_INC_AND_CHECK(c_, t_) do { (c_)++; if ((c_) > (t_)) { return TIMED_OUT; } } while (0)
#define COUNTER_INC_AND_CHECK_FREE(c_, t_, p_) do { (c_)++; if ((c_) > (t_)) { free(p_); return TIMED_OUT; } } while (0)
#define ALGO_ERRNO_UNWRAP(exp_) do { algo_errno_t res_ = (exp_); if (res_ != OK) { return res_; } } while (0)

#endif // COMMON_H_INCLUDED_
