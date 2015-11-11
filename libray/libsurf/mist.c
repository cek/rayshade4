/*
 * Copyright (C) 1989-2015, Craig E. Kolb
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "atmosphere.h"
#include "mist.h"

Mist *
MistCreate(color, trans, zero, scale)
Color *color, *trans;
Float zero, scale;
{
	Mist *mist;

	mist = (Mist *)Malloc(sizeof(Mist));
	mist->color = *color;
	mist->trans = *trans;
	mist->zero = zero;
	mist->scale = 1. / scale;
	return mist;
}

/*
 * Add low-altitude mist to the given color.
 */
void
MistApply(mist, ray, pos, dist, color)
Mist *mist;
Ray *ray;
Vector *pos;
Float dist;
Color *color;
{
	Float deltaZ, d, atten;
	extern Float ExpAtten();

	deltaZ = mist->scale * (pos->z - ray->pos.z);
	if (fabs(deltaZ) > EPSILON)
		d = (exp(-ray->pos.z*mist->scale + mist->zero) -
			    exp(-pos->z*mist->scale + mist->zero)) / deltaZ;
	else
		d = exp(-pos->z*mist->scale + mist->zero);

	dist *= d;

	atten = ExpAtten(dist, mist->trans.r);
	color->r = atten*color->r + (1. - atten)*mist->color.r;

	atten = ExpAtten(dist, mist->trans.g);
	color->g = atten*color->g + (1. - atten)*mist->color.g;

	atten = ExpAtten(dist, mist->trans.b);
	color->b = atten*color->b + (1. - atten)*mist->color.b;
}
