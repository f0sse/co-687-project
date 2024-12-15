/*----------------------------------------------------------------------------*\
 *
 * dbl_and_add.c
 * lucas@pamorana.net
 *
 * A contrived implementation of "Double & Add"
 * for a project in CO 687, UW, Fall 2024.
 *
\*----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "dbl_and_add.h"
#include "gmp.h"

const PT IDENTITY = PT_RO_INITIALIZER;

static void point_set(PT *dst, const PT *src)
{
	mpz_set(dst->x, src->x);
	mpz_set(dst->y, src->y);
}

int point_cmp(const PT *A, const PT *B)
{
	int r;

	if ((r = mpz_cmp(A->x, B->x)))
		return r;

	return mpz_cmp(A->y, B->y);
}

void lambda_slp(const EC *C, const PT *P, const PT *Q, mpz_t val)
{
	mpz_t a, b; mpz_inits(a, b, NULL);

	mpz_sub   (a,   Q->y, P->y); // a = Qy - Py
	mpz_sub   (b,   Q->x, P->x); // b = Qx - Px
	mpz_invert(b,   b,    C->p); // b = b^-1 % p
	mpz_mul   (val, a,       b); // r = a * b
	mpz_mod   (val, val,  C->p); // r = r % p

	mpz_clears(a, b, NULL);
}

void lambda_tan(const EC *C, const PT *P, const PT *Q, mpz_t val)
{
	mpz_t a, b; mpz_inits(a, b, NULL);

	mpz_mul   (a,   P->x, P->x); // a = Px^2
	mpz_mul_ui(a,   a,       3); //   * 3
	mpz_add   (a,   a,    C->a); //   + a
	mpz_mul_ui(b,   Q->y,    2); // b = 2 Qy
	mpz_invert(b,   b,    C->p); // b = b^-1 % p
	mpz_mul   (val, a,       b); // r = a * b
	mpz_mod   (val, val,  C->p); // r = r % p

	mpz_clears(a, b, NULL);
}

double timedelta(struct timespec *start, struct timespec *end)
{
	struct timespec tmp;

	double d;

	if ((end->tv_nsec - start->tv_nsec) < 0)
	{
		tmp.tv_sec  = end->tv_sec - start->tv_sec - 1;
		tmp.tv_nsec = 1000000000 + end->tv_nsec - start->tv_nsec;
	}
	else
	{
		tmp.tv_sec  = end->tv_sec  - start->tv_sec;
		tmp.tv_nsec = end->tv_nsec - start->tv_nsec;
	}

	return (double) tmp.tv_sec + (double) tmp.tv_nsec / 1e9;
}

/* group operation (obs: val must be able to overlap with P and/or Q) */
void operator(const EC *C, const PT *P, const PT *Q, PT *val)
{
	if (point_cmp(P, &IDENTITY) == 0) point_set(val, Q); else
	if (point_cmp(Q, &IDENTITY) == 0) point_set(val, P); else
	{
		mpz_t tst; mpz_init(tst);

		mpz_add(tst, P->y, Q->y); // is 0 if P->y = -Q->y <=> P + (-P) = I

		if (mpz_cmp(P->x, Q->x) == 0 && mpz_cmp_ui(tst, 0) == 0)
			point_set(val, &IDENTITY);
		else
		{
			void (*lambda)(const EC*, const PT*, const PT*, mpz_t) = \
				point_cmp(P, Q) == 0
					? lambda_tan
					: lambda_slp
					;

			mpz_t a, b, l; mpz_inits(a, b, l, NULL);

			lambda(C, P, Q, l);

			mpz_mul(a, l,    l); // Rx = lambda^2
			mpz_sub(a, a, P->x); //    - Px
			mpz_sub(a, a, Q->x); //    - Qx
			mpz_sub(b, P->x, a); // Ry = (Px - Rx)
			mpz_mul(b, l,    b); //    * lambda
			mpz_sub(b, b, P->y); //    - Py
			mpz_mod(a, a, C->p); // Rx mod p
			mpz_mod(b, b, C->p); // Ry mod p

			mpz_set(val->x, a);
			mpz_set(val->y, b);

			mpz_clears(a, b, NULL);
		}

		mpz_clear(tst);
	}
}


double find_boundary(const EC *C)
{
	struct timespec beg, end;

	double dbl = 0, add = 0.;

	PT Q; point_init(&Q);

	point_set(&Q, &C->g);

	// just a doubling takes how long?
	for (int i=0; i < 1e3l; i++)
	{
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &beg);
		operator(C, &Q, &Q, &Q);
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
		dbl += timedelta(&beg, &end);
	}
	dbl /= 1e3;

	point_set(&Q, &C->g);

	// double and add?
	for (int i=0; i < 1e3l; i++)
	{
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &beg);
		operator(C, &Q,    &Q, &Q);
		operator(C, &C->g, &Q, &Q);
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
		add += timedelta(&beg, &end);
	}
	add /= 1e3;

	point_free(&Q);

	return (add + dbl) / 2.;
}

void dbl_and_add(const EC *C, const mpz_t k, const PT *P, PT *val)
{
	PT Q; mpz_inits(Q.x, Q.y, NULL);

	mp_bitcnt_t n = mpz_sizeinbase(k, 2);
	mp_bitcnt_t i = n - 1;

	struct timespec beg, end;

	mpz_t guess; mpz_init(guess);

	double *samples = calloc((size_t) n, sizeof(double));
	double  bound   = find_boundary(C);
	size_t  sample  = 0;

	point_set(&Q, P);

	while (i--)
	{
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &beg);
		/*---*/
		operator(C, &Q, &Q, &Q);
		if (mpz_tstbit(k, i))
			operator(C, P, &Q, &Q);
		/*---*/
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
		samples[sample++] = timedelta(&beg, &end);
	}

	point_set(val, &Q);

	mpz_clears(Q.x, Q.y, NULL);

	mpz_set_ui(guess, 1); // MSB is always 1

	for (size_t j=0; j < sample; j++)
	{
		mpz_mul_ui(guess, guess, 2);
		if (samples[j] > bound)
			mpz_add_ui(guess, guess, 1);
	}

	if (getenv("PRINT_COORDS"))
	{
		printf("BEGIN COORDINATES\n");
		for (size_t j=0; j < sample; j++)
			printf("%zd,%0.9f\n", j+1, samples[j]);
		printf("END COORDINATES\n");
	}

	printf("bound: %e\n", bound);
	gmp_printf("guess: %Zd\n", k);

	mpz_clear(guess);
	free(samples);
}

void point_init(PT *p){ mpz_inits (p->x, p->y, NULL); }
void point_free(PT *p){ mpz_clears(p->x, p->y, NULL); }
void curve_init(EC *c){ mpz_inits (c->a,c->b,c->p,c->q,c->g.x,c->g.y,NULL); }
void curve_free(EC *c){ mpz_clears(c->a,c->b,c->p,c->q,c->g.x,c->g.y,NULL); }
