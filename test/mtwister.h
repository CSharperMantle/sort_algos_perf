/*
 * ESultanik/mtwister: A pure C implementation of the Mersenne twister is a pseudo-random number generation algorithm
 * Public Domain
 * Modified by Rong "Mantle" Bao <baorong2005@126.com>
 */

#pragma once

#ifndef MTWISTER_H_
#define MTWISTER_H_

#define STATE_VECTOR_LENGTH 624
#define STATE_VECTOR_M      397 /* changes to STATE_VECTOR_LENGTH also require changes to this */

typedef struct MTRand_ {
    unsigned long mt[STATE_VECTOR_LENGTH];
    int index;
} MTRand_t;

void mt_seed_rand(MTRand_t *rand, unsigned long seed);
unsigned long mt_gen_rand_long(MTRand_t *rand);
double mt_gen_rand(MTRand_t *rand);

#endif // MTWISTER_H_
