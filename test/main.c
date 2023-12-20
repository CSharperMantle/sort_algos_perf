#include "pch.h"
#include "mtwister.h"
#include "../algo/algo.h"

#define CONST_CHECK_N_ELEMS 10
#define CONST_BENCH_N_ROUNDS 3
#define CONST_ALMOST_ORDERED_N_SWAPS 16
#define CONST_CHECK_SHIFT_DIST 2
#define CONST_BENCH_SHIFT_DIST 2

#define COLOR_ANSI_RED "\033[1;31m"
#define COLOR_ANSI_GREEN "\033[1;32m"
#define COLOR_ANSI_YELLOW "\033[1;33m"
#define COLOR_ANSI_CYAN "\033[1;36m"
#define COLOR_ANSI_WHITE "\033[1;37m"
#define COLOR_ANSI_RESET "\033[1;0m"

#define ARR_LEN_STATIC(a_) (sizeof(a_) / sizeof((a_)[0]))

#define EXPECT(e_, a_) do { if (!((e_) == (a_))) { fprintf(stderr, COLOR_ANSI_RED "[!]" COLOR_ANSI_RESET " " __FUNCTION__ "():%d: Assertion (" #a_ " == " #e_ ") failed", __LINE__); abort(); } } while (0)
#define EXPECT_NOT(e_, a_) do { if (!((e_) != (a_))) { fprintf(stderr, COLOR_ANSI_RED "[!]" COLOR_ANSI_RESET " " __FUNCTION__ "():%d: Assertion (" #a_ " != " #e_ ") failed", __LINE__); abort(); } } while (0)
#define PRINT_INFO(fmt_, ...) do { fprintf(stderr, COLOR_ANSI_CYAN "[*]" COLOR_ANSI_RESET " " __FUNCTION__ "():%d: " fmt_, __LINE__, ##__VA_ARGS__); } while (0)
#define PRINT_GOOD(fmt_, ...) do { fprintf(stderr, COLOR_ANSI_GREEN "[+]" COLOR_ANSI_RESET " " __FUNCTION__ "():%d: " fmt_, __LINE__, ##__VA_ARGS__); } while (0)
#define PRINT_BAD(fmt_, ...) do { fprintf(stderr, COLOR_ANSI_YELLOW "[-]" COLOR_ANSI_RESET " " __FUNCTION__ "():%d: " fmt_, __LINE__, ##__VA_ARGS__); } while (0)

typedef struct timespec timespec_t;

typedef struct bench_result_ {
    algo_errno_t retval;
    timespec_t diff;
    uint64_t comp_count;
} bench_result_t;

static int cmp_uint32(const void *a, const void *b) {
    const uint32_t a_ = *(const uint32_t *)a;
    const uint32_t b_ = *(const uint32_t *)b;

    return (a_ > b_) - (a_ < b_);
}

typedef void (*gen_data_t)(MTRand_t *restrict, size_t, uint32_t *restrict);

static void gen_rand_data(MTRand_t *restrict rng, size_t n_elem, uint32_t *restrict out_arr) {
    for (size_t i = 0; i < n_elem; i++) {
        out_arr[i] = mt_gen_rand_long(rng);
    }
}

static void gen_rising_data(MTRand_t *restrict rng, size_t n_elem, uint32_t *restrict out_arr) {
    gen_rand_data(rng, n_elem, out_arr);
    qsort(out_arr, n_elem, sizeof(uint32_t), cmp_uint32);
}

static void gen_almost_rising_data(MTRand_t *restrict rng, size_t n_elem, uint32_t *restrict out_arr) {
    gen_rising_data(rng, n_elem, out_arr);

    for (int i = 0; i < CONST_ALMOST_ORDERED_N_SWAPS; i++) {
        size_t low = (size_t)(mt_gen_rand(rng) * (double)n_elem);
        size_t high = (size_t)(mt_gen_rand(rng) * (double)n_elem);
        uint32_t tmp = out_arr[low];
        out_arr[low] = out_arr[high];
        out_arr[high] = tmp;
    }
}

/*
 * Calculate time difference.
 * Adapted from https://www.gnu.org/software/libc/manual/html_node/Calculating-Elapsed-Time.html.
 */
static int timespec_subtract(timespec_t *x, timespec_t *y, timespec_t *result) {
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_nsec < y->tv_nsec) {
        int nsec = (y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
        y->tv_nsec -= 1000000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_nsec - y->tv_nsec > 100000000) {
        int nsec = (x->tv_nsec - y->tv_nsec) / 100000000;
        y->tv_nsec += 100000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_nsec = x->tv_nsec - y->tv_nsec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

static void check(MTRand_t *restrict rng, const named_algo_t *restrict al, size_t n_elem, comp_t cmp, uint64_t threshold, gen_data_t generator) {
    const algo_t sorter = al->algo;
    const char *sorter_name = al->name;

    uint32_t *arr_1 = (uint32_t *)malloc(sizeof(uint32_t) * n_elem);
    EXPECT_NOT(NULL, arr_1);
    uint32_t *arr_2 = (uint32_t *)malloc(sizeof(uint32_t) * n_elem);
    EXPECT_NOT(NULL, arr_2);

    generator(rng, n_elem, arr_1);
    memcpy_s(arr_2, sizeof(uint32_t) * n_elem, arr_1, sizeof(uint32_t) * n_elem);

    uint64_t cmp_counter = 0;
    algo_errno_t result = sorter(arr_1, n_elem, sizeof(uint32_t), cmp, threshold, &cmp_counter);
    EXPECT(OK, result);
    qsort(arr_2, n_elem, sizeof(uint32_t), cmp);

    for (size_t i = 0; i < n_elem; i++) {
        EXPECT(arr_2[i], arr_1[i]);
    }

    PRINT_GOOD("algo \"%s\": produces identical result as stdlib qsort\n", sorter_name);

    free(arr_1);
    free(arr_2);
}

static void bench_n_rounds(MTRand_t *restrict rng, const algo_t sorter, size_t n_elem, comp_t cmp, uint64_t threshold, size_t n_rounds, gen_data_t generator, bench_result_t *restrict out_result) {
    uint32_t *nums = (uint32_t *)malloc(sizeof(uint32_t) * n_elem);
    EXPECT_NOT(NULL, nums);

    for (size_t i = 0; i < n_rounds; i++) {
        generator(rng, n_elem, nums);

        uint64_t cmp_counter = 0;

        timespec_t ts_1;
        EXPECT_NOT(0, timespec_get(&ts_1, TIME_UTC));
        algo_errno_t result = sorter(nums, n_elem, sizeof(uint32_t), cmp, threshold, &cmp_counter);
        timespec_t ts_2;
        EXPECT_NOT(0, timespec_get(&ts_2, TIME_UTC));

        out_result[i].retval = result;
        out_result[i].comp_count = cmp_counter;
        timespec_subtract(&ts_2, &ts_1, &out_result[i].diff);
    }

    free(nums);
}

static void bench_rand(MTRand_t *restrict rng, const named_algo_t *restrict al, size_t n_elem, comp_t cmp, uint64_t threshold) {
    const algo_t sorter = al->algo;
    const char *sorter_name = al->name;

    bench_result_t result[CONST_BENCH_N_ROUNDS];
    bench_n_rounds(rng, sorter, n_elem, cmp, threshold, CONST_BENCH_N_ROUNDS, gen_rand_data, result);

    for (int i = 0; i < CONST_BENCH_N_ROUNDS; i++) {
        bench_result_t cur = result[i];
        if (cur.retval == OK) {
            PRINT_GOOD("algo \"%s\": random - round %" PRId32 ": done - %" PRIu64 " comps - %" PRIdMAX ".%09" PRId32 " sec\n", sorter_name, i + 1, cur.comp_count, (intmax_t)cur.diff.tv_sec, cur.diff.tv_nsec);
        } else {
            PRINT_BAD("algo \"%s\": random - round %" PRId32 ": error - %" PRIu64 " comps - %" PRIdMAX ".%09" PRId32 " sec\n", sorter_name, i + 1, cur.comp_count, (intmax_t)cur.diff.tv_sec, cur.diff.tv_nsec);
        }
        printf("%s,random,%" PRId32 ",%" PRId32 ",%" PRIu64 ",%" PRIdMAX ".%09" PRId32 "\n", sorter_name, cur.retval, i + 1, cur.comp_count, (intmax_t)cur.diff.tv_sec, cur.diff.tv_nsec);
    }
}

static void bench_rising(MTRand_t *restrict rng, const named_algo_t *restrict al, size_t n_elem, comp_t cmp, uint64_t threshold) {
    const algo_t sorter = al->algo;
    const char *sorter_name = al->name;

    bench_result_t result[CONST_BENCH_N_ROUNDS];
    bench_n_rounds(rng, sorter, n_elem, cmp, threshold, CONST_BENCH_N_ROUNDS, gen_rising_data, result);

    for (int i = 0; i < CONST_BENCH_N_ROUNDS; i++) {
        bench_result_t cur = result[i];
        if (cur.retval == OK) {
            PRINT_GOOD("algo \"%s\": rising - round %" PRId32 ": done - %" PRIu64 " comps - %" PRIdMAX ".%09" PRId32 " sec\n", sorter_name, i + 1, cur.comp_count, (intmax_t)cur.diff.tv_sec, cur.diff.tv_nsec);
        } else {
            PRINT_BAD("algo \"%s\": rising - round %" PRId32 ": error - %" PRIu64 " comps - %" PRIdMAX ".%09" PRId32 " sec\n", sorter_name, i + 1, cur.comp_count, (intmax_t)cur.diff.tv_sec, cur.diff.tv_nsec);
        }
        printf("%s,rising,%" PRId32 ",%" PRId32 ",%" PRIu64 ",%" PRIdMAX ".%09" PRId32 "\n", sorter_name, cur.retval, i + 1, cur.comp_count, (intmax_t)cur.diff.tv_sec, cur.diff.tv_nsec);
    }
}

static void bench_almost_rising(MTRand_t *restrict rng, const named_algo_t *restrict al, size_t n_elem, comp_t cmp, uint64_t threshold) {
    const algo_t sorter = al->algo;
    const char *sorter_name = al->name;

    bench_result_t result[CONST_BENCH_N_ROUNDS];
    bench_n_rounds(rng, sorter, n_elem, cmp, threshold, CONST_BENCH_N_ROUNDS, gen_almost_rising_data, result);

    for (int i = 0; i < CONST_BENCH_N_ROUNDS; i++) {
        bench_result_t cur = result[i];
        if (cur.retval == OK) {
            PRINT_GOOD("algo \"%s\": almost rising - round %" PRId32 ": done - %" PRIu64 " comps - %" PRIdMAX ".%09" PRId32 " sec\n", sorter_name, i + 1, cur.comp_count, (intmax_t)cur.diff.tv_sec, cur.diff.tv_nsec);
        } else {
            PRINT_BAD("algo \"%s\": almost rising - round %" PRId32 ": error - %" PRIu64 " comps - %" PRIdMAX ".%09" PRId32 " sec\n", sorter_name, i + 1, cur.comp_count, (intmax_t)cur.diff.tv_sec, cur.diff.tv_nsec);
        }
        printf("%s,almost rising,%" PRId32 ",%" PRId32 ",%" PRIu64 ",%" PRIdMAX ".%09" PRId32 "\n", sorter_name, cur.retval, i + 1, cur.comp_count, (intmax_t)cur.diff.tv_sec, cur.diff.tv_nsec);
    }
}

static const uint64_t BENCH_ARR_SCALES[] = {1000, 10000, 100000, 1000000, 10000000};

int main(void) {
    MTRand_t rng;

    mt_seed_rand(&rng, (unsigned long)time(NULL));

    printf("algo name,input dist,retval,round no,n comps,time sec\n");

    PRINT_INFO("Checking correctness of algorithms\n");
    for (const named_algo_t *al = ALGOS; al->algo != NULL; al++) {
        check(&rng, al, 10, cmp_uint32, (uint64_t)(UINT32_MAX >> CONST_CHECK_SHIFT_DIST), gen_rand_data);
    }

    for (size_t i = 0; i < ARR_LEN_STATIC(BENCH_ARR_SCALES); i++) {
        uint64_t scale = BENCH_ARR_SCALES[i];

        PRINT_INFO("Running benchmarks on scale %" PRIu64 "\n", scale);
        for (const named_algo_t *al = ALGOS; al->algo != NULL; al++) {
            bench_rand(&rng, al, scale, cmp_uint32, (uint64_t)(UINT32_MAX >> CONST_BENCH_SHIFT_DIST));
            bench_rising(&rng, al, scale, cmp_uint32, (uint64_t)(UINT32_MAX >> CONST_BENCH_SHIFT_DIST));
            bench_almost_rising(&rng, al, scale, cmp_uint32, (uint64_t)(UINT32_MAX >> CONST_BENCH_SHIFT_DIST));
        }
    }

    return 0;
}
