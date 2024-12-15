/*----------------------------------------------------------------------------*\
 *
 * dbl_and_add.h
 * lucas@pamorana.net
 *
 * A contrived implementation of "Double & Add"
 * for a project in CO 687, UW, Fall 2024.
 *
\*----------------------------------------------------------------------------*/

#ifndef _DBL_AND_ADD_H
#define _DBL_AND_ADD_H

#include <gmp.h>

typedef struct
{
	mpz_t x;
	mpz_t y;
}
PT;

#define PT_RO_INITIALIZER { MPZ_ROINIT_N(NULL, 0), MPZ_ROINIT_N(NULL, 0) }

typedef struct /* y^2 = x^3 + ax + b */
{
	mpz_t a, b; // coefficients
	mpz_t p, q; // prime modulus, order
	PT    g;    // generator
}
EC;

#define EC_RO_INITIALIZER \
{ \
	MPZ_ROINIT_N(NULL, 0), MPZ_ROINIT_N(NULL, 0), \
	MPZ_ROINIT_N(NULL, 0), MPZ_ROINIT_N(NULL, 0), \
	PT_RO_INITIALIZER \
}

extern const PT IDENTITY;

/*
 * point_cmp:
 *    0 <== \f$ A < B \f$
 *   -1 <== \f$ A = B \f$
 *    1 <== \f$ A > B \f$
 *
 *   The order of comparison is \p x, then \p y.
 */
int point_cmp(const PT *A, const PT *B);

/*
 * dbl_and_add:
 *   For a point \p P and a scalar nonce \p k, on the curve \p C,
 *   it computes and sets \p val to the point \f$ k P \f$.
 */
void dbl_and_add(const EC *C, const mpz_t k, const PT *P, PT *val);

/* memory management */
void point_init(PT *p);
void point_free(PT *p);
void curve_init(EC *c);
void curve_free(EC *c);

#endif /* _DBL_AND_ADD_H */
