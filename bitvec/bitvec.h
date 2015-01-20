/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * C99 BitVec implementation.
 *
 * Bits are indexed as an array, i.e.,
 *   0    1          i              n
 *   +----+----+     +--------+     +----+
 *   |BYTE|BYTE| ... |01101100| ... |BYTE| ...
 *   +----+----+     +--------+     +----+
 *                    ^      ^
 *                   {bit 0, bit 7} of byte i
 *
 *  Provides O(1) random access get & set, O(1) pop, amortized O(1) push, and
 *  O(n) vector push.
 **/

#ifndef __BITVEC_H__

#define __BITVEC_H__


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#define BITVEC_BITS 8192 /* 1KB */


typedef struct bitvec_ bitvec;

/**
 * TODO:
 *  - maybe express size in bytes and use bit & byte offsets?
 **/
struct bitvec_ {
    uint8_t *vec;
    size_t   nbits;
    size_t   off;
};


bitvec *  bv_new();
bitvec *  bv_init(bool v);
bool      bv_get(const bitvec *b, size_t bit);
void      bv_set(bitvec *b, size_t bit, bool v);
void      bv_push(bitvec *b, bool v);
bool      bv_pop(bitvec *b);
void      bv_push_bits(bitvec *b, const uint8_t *vec, size_t nbits);
void      bv_push_bytes(bitvec *b, const uint8_t *vec, size_t nbytes);
uint8_t * bv_to_array(const bitvec *b, size_t *nbits);
void      bv_destroy();


#endif /* __BITVEC_H__ */
