#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <complex.h>
#include <assert.h>

#include "debug.h"
#include "normalizer.h"

#define LUT_SIZE	0x10000
//#define IS_BIG_ENDIAN

struct _normalizer_t {
	float complex lut[LUT_SIZE];
};


normalizer_t *
normalizer_create ()
{
	unsigned int i;

	normalizer_t *self = (normalizer_t *) malloc (sizeof (normalizer_t));
	assert(self);

	for(i = 0; i < LUT_SIZE; i++)
	{
#ifdef IS_BIG_ENDIAN
		self->lut[i] = (((float)(i >> 8) - 127.4f) * (1.0f/128.0f)) +
				_Complex_I * (((float)(i & 0xff) - 127.4f) * (1.0f/128.0f));
#else
		self->lut[i] = (((float)(i & 0xff) - 127.4f) * (1.0f/128.0f)) +
				_Complex_I * (((float)(i >> 8) - 127.4f) * (1.0f/128.0f));

		//debug("   lut[%4u] = %12.4e + j*%12.4e;\n", i+1, crealf(self->lut[i]), cimagf(self->lut[i]));
#endif
	}

	return self;
}

complex float
normalizer_normalize(normalizer_t *self, uint16_t index)
{
	return self->lut[index];

}

void
normalizer_destroy (normalizer_t **self_p)
{
	assert (self_p);
	if (*self_p) {
		normalizer_t *self = *self_p;

		//  Free object itself
		free (self);
		*self_p = NULL;
	}
}
