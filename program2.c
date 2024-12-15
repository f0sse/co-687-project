/*----------------------------------------------------------------------------*\
 *
 * program2.c
 * lucas@pamorana.net
 *
 * Implements \p main, using OpenSSL 1.0.2a to perform a scalar multiplication
 * of an elliptic curve point. The curve used is secp256k1.
 *
\*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/objects.h>

int main(int argc, char *argv[])
{
	char *kdec;

	EC_GROUP *curve = NULL;
	EC_POINT *point = NULL, *result = NULL;

	BIGNUM *k   = NULL, *q = NULL;
	BN_CTX *ctx = NULL;

	ctx    = BN_CTX_new();
	curve  = EC_GROUP_new_by_curve_name(NID_secp256k1);
	point  = EC_POINT_new(curve);
	result = EC_POINT_new(curve);

	EC_POINT_copy(point, EC_GROUP_get0_generator(curve));

	k = BN_new();
	q = BN_new();

	EC_GROUP_get_order(curve, q, ctx);

	// assumes k != 0 (is likely)
	BN_rand_range(k, q);

	// scalar multiply
	EC_POINT_mul(curve, result, NULL, point, k, ctx);

	kdec = BN_bn2dec(k);
	printf("truth: %s\n", kdec);
	free(kdec);

	BN_free(k);
	BN_free(q);
	EC_POINT_free(result);
	EC_POINT_free(point);
	EC_GROUP_free(curve);
	BN_CTX_free(ctx);

	return EXIT_SUCCESS;
}
