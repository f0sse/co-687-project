/*----------------------------------------------------------------------------*\
 *
 * program1.c
 * lucas@pamorana.net
 *
 * Implements \p main, using the contrived double-and-add algorithm to
 * perform a scalar multiplication of an elliptic curve point. The curve
 * used is secp256k1.
 *
\*----------------------------------------------------------------------------*/

#include <stdlib.h>

#include "dbl_and_add.h"

#define P \
	"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"

#define Q \
	"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141"

#define Gx \
	"79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"

#define Gy \
	"483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"

#define ELEN(A) (sizeof(A[0]))
#define ALEN(A) (sizeof(A)/sizeof(A[0]))

int main(int argc, char *argv[])
{
	EC C;
	PT R;

	mpz_t k;

	unsigned char kbuf[256/8];

	point_init(&R);
	curve_init(&C);
	mpz_init(k);

	// secp256k1
	mpz_set_str(C.p, P, 16);
	mpz_set_str(C.q, Q, 16);
	mpz_set_ui (C.a, 0);
	mpz_set_ui (C.b, 7);
	mpz_set_str(C.g.x, Gx, 16);
	mpz_set_str(C.g.y, Gy, 16);

	do
	{
		arc4random_buf(kbuf, sizeof(kbuf));
		mpz_import(k, ALEN(kbuf), 1, ELEN(kbuf), 1, 0, kbuf);
	}
	while (mpz_cmp(C.q, k) <= 0);

	dbl_and_add(&C, k, &C.g, &R);

	gmp_printf("truth: %Zd\n", k);

	mpz_clear(k);
	curve_free(&C);
	point_free(&R);

	return EXIT_SUCCESS;
}
