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
#include "texture.h"
#include "sky.h"

Sky *
SkyCreate(scale, h, lambda, octaves, cthresh, lthresh)
Float h, lambda, scale, cthresh, lthresh;
int octaves;
{
	Sky *sky;

	sky = (Sky *)Malloc(sizeof(Sky));
	sky->beta = 1. + 2 * h;
	sky->omega = pow(lambda, -0.5 * sky->beta);
	sky->lambda = lambda;
	sky->scale = scale;
	sky->cthresh = cthresh;
	sky->lthresh = lthresh;
	sky->octaves = octaves;
	return sky;
}

void
SkyApply(sky, prim, ray, pos, norm, gnorm, surf)
Sky *sky;
Geom *prim;
Ray *ray;
Vector *pos, *norm, *gnorm;
Surface *surf;
{
	Float It, maxval;

	It = fBm(pos, sky->omega, sky->lambda, sky->octaves);
	maxval = 1. / (1. - sky->omega);
	/*
	 * Map from [-maxval,maxval] to [0,1]
	 */
	It = (maxval +  It) * 0.5/maxval;

	It = (It - sky->lthresh) / (sky->cthresh - sky->lthresh);
	if (It < 0.)
		It = 0;
	else if (It > 1.)
		It = 1;

	if (sky->scale != 0.)
		It = pow(It, sky->scale);

	surf->transp = 1. - It;

	ColorScale(It, surf->diff, &surf->diff);
	ColorScale(It, surf->amb, &surf->amb);
}
