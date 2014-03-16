/*  =========================================================================
    Based on Dimitri Stolnikovs GNU Radio rtl-sdr block

    -------------------------------------------------------------------------
    Copyright (c) 2013 Mariusz Ryndzionek - mryndzionek@gmail.com

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTA-
    BILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see http://www.gnu.org/licenses/.
    =========================================================================
 */

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
