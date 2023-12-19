#include "pch.h"
#include "mtwister.h"
#include "../algo/algo.h"

#define COLOR_ANSI_RED "\033[1;31m"
#define COLOR_ANSI_GREEN "\033[1;32m"
#define COLOR_ANSI_YELLOW "\033[1;33m"
#define COLOR_ANSI_CYAN "\033[1;36m"
#define COLOR_ANSI_WHITE "\033[1;37m"
#define COLOR_ANSI_RESET "\033[1;0m"

#define ARR_LEN_STATIC(a_) (sizeof(a_) / sizeof((a_)[0]))

#define EXPECT(e_, a_) do { if (!((e_) == (a_))) { printf("[!] " __FUNCTION__ "():%d: Assertion (" #a_ " == " #e_ ") failed", __LINE__); abort(); } } while (0)
#define EXPECT_NOT(e_, a_) do { if (!((e_) != (a_))) { printf("[!] " __FUNCTION__ "():%d: Assertion (" #a_ " != " #e_ ") failed", __LINE__); abort(); } } while (0)
#define PRINT_INFO(fmt_, ...) do { printf(COLOR_ANSI_CYAN "[*]" COLOR_ANSI_RESET " " __FUNCTION__ "():%d: " fmt_, __LINE__, ##__VA_ARGS__); } while (0)
#define PRINT_GOOD(fmt_, ...) do { printf(COLOR_ANSI_GREEN "[+]" COLOR_ANSI_RESET " " __FUNCTION__ "():%d: " fmt_, __LINE__, ##__VA_ARGS__); } while (0)
#define PRINT_BAD(fmt_, ...) do { printf(COLOR_ANSI_RED "[-]" COLOR_ANSI_RESET " " __FUNCTION__ "():%d: " fmt_, __LINE__, ##__VA_ARGS__); } while (0)

static const uint64_t BENCH_ARR_SCALES[] = {1000, 10000, 100000, 1000000, 10000000};

static int cmp_uint32(const void *a, const void *b) {
    const uint32_t a_ = *(const uint32_t *)a;
    const uint32_t b_ = *(const uint32_t *)b;

    if (a_ > b_) {
        return 1;
    } else if (a_ < b_) {
        return -1;
    } else {
        return 0;
    }
}

static void check(MTRand_t *rng, const named_algo_t *al, size_t n_elem, uint64_t threshold) {
    const algo_t sorter = al->algo;
    const char *sorter_name = al->name;

    uint32_t *arr_1 = (uint32_t *)malloc(sizeof(uint32_t) * n_elem);
    uint32_t *arr_2 = (uint32_t *)malloc(sizeof(uint32_t) * n_elem);

    EXPECT_NOT(NULL, arr_1);
    EXPECT_NOT(NULL, arr_2);

    for (size_t i = 0; i < n_elem; i++) {
        uint32_t x = mt_gen_rand_long(rng);
        arr_1[i] = x;
        arr_2[i] = x;
    }

    uint64_t cmp_counter = 0;
    algo_errno_t result = sorter(arr_1, n_elem, sizeof(uint32_t), cmp_uint32, threshold, &cmp_counter);
    EXPECT(OK, result);
    qsort(arr_2, n_elem, sizeof(uint32_t), cmp_uint32);

    for (size_t i = 0; i < n_elem; i++) {
        EXPECT(arr_2[i], arr_1[i]);
    }

    PRINT_GOOD("algo \"%s\": produces identical result as stdlib qsort\n", sorter_name);

    free(arr_1);
    free(arr_2);
}

static void bench_rand(MTRand_t *rng, const named_algo_t *al, size_t n_elem, uint64_t threshold) {
    const algo_t sorter = al->algo;
    const char *sorter_name = al->name;

    uint32_t *arr_1 = (uint32_t *)malloc(sizeof(uint32_t) * n_elem);

    EXPECT_NOT(NULL, arr_1);

    for (size_t i = 0; i < n_elem; i++) {
        arr_1[i] = mt_gen_rand_long(rng);
    }

    uint64_t cmp_counter = 0;
    algo_errno_t result = sorter(arr_1, n_elem, sizeof(uint32_t), cmp_uint32, threshold, &cmp_counter);
    if (result == OK) {
        PRINT_GOOD("algo \"%s\": done - %" PRIu64 " comps\n", sorter_name, cmp_counter);
    } else {
        PRINT_BAD("algo \"%s\": failed - %" PRIu64 " comps limit exceeded\n", sorter_name, threshold);
    }

    free(arr_1);
}

int main(void) {
    MTRand_t rng;

    mt_seed_rand(&rng, (unsigned long)time(NULL));

    PRINT_INFO("Checking correctness of algorithms\n");

    for (const named_algo_t *al = ALGOS; al->algo != NULL; al++) {
        check(&rng, al, 10, (uint64_t)(UINT32_MAX >> 8));
    }

    for (size_t i = 0; i < ARR_LEN_STATIC(BENCH_ARR_SCALES); i++) {
        uint64_t scale = BENCH_ARR_SCALES[i];

        PRINT_INFO("Running benchmarks on scale %" PRIu64 "\n", scale);
        for (const named_algo_t *al = ALGOS; al->algo != NULL; al++) {
            bench_rand(&rng, al, scale, (uint64_t)(UINT32_MAX >> 4));
        }
    }

    return 0;
}
