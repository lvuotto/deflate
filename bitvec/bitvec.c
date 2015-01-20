/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 **/

#include "bitvec.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


/* =========================================================================
 * Helper functions declaration.
 * ========================================================================= */


size_t next_power_of_2(size_t x);


/* =========================================================================
 * Public API.
 * ========================================================================= */


bitvec * bv_new()
{
    bitvec *b = (bitvec *) malloc(sizeof(bitvec));
    b->vec = (uint8_t *) calloc(sizeof(uint8_t), BITVEC_BITS / 8);
    assert(b->vec != NULL);
    b->nbits = BITVEC_BITS;
    b->off = 0;

    return b;
}


bitvec * bv_init(bool v)
{
    bitvec *b = bv_new();
    if (v)
        memset(b->vec, 0xff, b->nbits / 8);
    return b;
}


bool bv_get(const bitvec *b, size_t bit)
{
    assert(bit < b->off);
    return (b->vec[bit / 8] & (0x80 >> bit)) != 0;
}


void bv_set(bitvec *b, size_t bit, bool v)
{
    assert(bit < b->off);
    size_t mask = 0x80 >> bit;
    if (v)
        b->vec[bit / 8] |= mask;
    else
        b->vec[bit / 8] &= ~mask;
}


void bv_push(bitvec *b, bool v)
{
    if (b->nbits == b->off) {
        /**
         * TODO:
         *  - boundary check for size_t?
         **/
        b->nbits *= 2;
        b->vec = (uint8_t *) realloc(b->vec, b->nbits / 8);
        assert(b->vec != NULL);
    }

    if (v)
        b->vec[b->off / 8] |= 0x80 >> (b->off % 8);
    b->off++;
}


bool bv_pop(bitvec *b)
{
    assert(b->off > 0);
    uint8_t mask = 0x80 >> (b->off % 8);
    bool r = (b->vec[b->off / 8] & mask) != 0;
    b->vec[b->off / 8] &= ~mask;
    b->off--;

    return r;
}


/**
 * Push nbits bits from vec at the back of b.
 **/
void bv_push_bits(bitvec *b, const uint8_t *vec, size_t nbits)
{
    size_t nbytes = nbits / 8;
    if (nbits % 8 != 0)
        nbytes++;
    bv_push_bytes(b, vec, nbytes);
    b->off = b->off - 8*nbytes + nbits; /* offset correction */
}


/**
 * Push nbytes bytes from vec at the back of b.
 *
 * TODO:
 *  - maybe optimize?
 **/
void bv_push_bytes(bitvec *b, const uint8_t *vec, size_t nbytes)
{
    if (b->nbits < b->off + 8*nbytes + 1) {
        size_t nbits = b->off + 8*nbytes + 1;
        if (nbits % 8 != 0) 
            nbits += 8 - nbits % 8;
        b->nbits = next_power_of_2(nbits);
        b->vec = (uint8_t *) realloc(b->vec, b->nbits / 8);
    }

    size_t base = b->off / 8;
    size_t sbits = b->off % 8;
    if (sbits == 0) {
        memcpy(b->vec + base, vec, nbytes);
    } else {
        memcpy(b->vec + base + 1, vec, nbytes);
        for (size_t i = 0; i < nbytes; i++) {
            b->vec[base + i] |= b->vec[base + i + 1] >> sbits;
            b->vec[base + i + 1] <<= 8 - sbits;
        }
    }
    b->off += 8*nbytes;
}


/**
 * Creates an array with the bits of b, padding with 0's to complete the last
 * byte. The array must be free'd by the user. nbits holds the number of valid
 * bits of the array, i.e., the number of bits that were actually set in the
 * bitvector.
 **/
uint8_t * bv_to_array(const bitvec *b, size_t *nbits)
{
    size_t nbytes = b->off / 8;
    if (b->off % 8 != 0)
        nbytes++;
    uint8_t *r = (uint8_t *) malloc(nbytes);
    assert(r != NULL);
    memcpy(r, b->vec, nbytes);
    *nbits = b->off;

    return r;
}


void bv_destroy(bitvec *b)
{
    free(b->vec);
    b->vec = NULL;
    free(b);
}


/* =========================================================================
 * Helper functions.
 * ========================================================================= */


/**
 * Returns the first power of 2 that is greater or equal than x, or BITVEC_BITS
 * if x < BITVEC_BITS
 **/
size_t next_power_of_2(size_t x)
{
    size_t r = BITVEC_BITS;
    while (r < x)
        r <<= 1;
    return r;
}
