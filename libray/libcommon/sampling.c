/*
 * Copyright (C) 1989-2015, Craig E. Kolb, Rod G. Bogart
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
#include "common.h"
#include "sampling.h"

SampleInfo	Sampling;	/* sampling information */

/*
 * Set sampling options.
 */
void
SamplingSetOptions(n, gaussian, width)
int n, gaussian;
Float width;
{
	Float norm, u, v;
	int x, y;

	Sampling.sidesamples = n;
	Sampling.totsamples = n*n;
	Sampling.weight = 1. / (Float)Sampling.totsamples;
	Sampling.spacing = 1. / (Float)Sampling.sidesamples;
	Sampling.filterwidth = width;
	Sampling.filterdelta = Sampling.filterwidth * Sampling.spacing;
	Sampling.gaussian = gaussian;

	Sampling.filter = (Float **)Malloc(Sampling.sidesamples
		*sizeof(Float *));
	for (y = 0; y < Sampling.sidesamples; y++) {
		Sampling.filter[y] = (Float *)Malloc(Sampling.sidesamples *
			sizeof(Float));
	}
	if (Sampling.gaussian) {
		norm = 0.;
		u = -0.5*Sampling.filterwidth + 
		     0.5*Sampling.filterwidth*Sampling.spacing;
		for (x = 0; x < Sampling.sidesamples; x++) {
			v = -0.5*Sampling.filterwidth +
			     0.5*Sampling.filterwidth*Sampling.spacing;
			for (y = 0; y < Sampling.sidesamples; y++) {
				Sampling.filter[x][y] = exp(-0.5*(u*u+v*v));
				norm += Sampling.filter[x][y];
				v += Sampling.spacing *
					Sampling.filterwidth;
			}
			u += Sampling.spacing * Sampling.filterwidth;
		}
		
		for (x = 0; x < Sampling.sidesamples; x++)
			for (y = 0; y < Sampling.sidesamples; y++)
				Sampling.filter[x][y] /= norm;
	} else {
		/* Box filter.  Yawn. */
		for (x = 0; x < Sampling.sidesamples; x++)
			for (y = 0; y < Sampling.sidesamples; y++)
				Sampling.filter[x][y] = Sampling.weight;
	}
}

/*
 * Set start time and duration of frame.
 */
void
SamplingSetTime(starttime, shutter, frame)
Float starttime, shutter;
int frame;
{
	Sampling.starttime = starttime;
	Sampling.shutter = shutter;
	Sampling.framenum = frame;
	TimeSet(Sampling.starttime);
	FrameSet((Float)frame);
}

/*
 * Find a point on a unit circle that is separated from other random
 * points by some jitter spacing.
 *
 * It should do the above, but the temporary hack below just finds a
 * jittered point in a unit square.
 */
void
UnitCirclePoint(pnt, sample)
Vector *pnt;
{
	/*
	 * This picks a random point on a -1 to 1 square.  The jitter stuff
	 * is correct enough to avoid excessive noise.  An extremely blurry
	 * bright highlight will look squarish, not roundish.  Sorry.
	 */
	Float jit;

	if (sample >= 0) {
		jit = 2. * Sampling.spacing;

		pnt->x = nrand()*jit - 1.0 +
			(sample % Sampling.sidesamples) * jit;
		pnt->y = nrand()*jit - 1.0 +
			(sample / Sampling.sidesamples) * jit;
		pnt->z = 0.0;
	} else {
		pnt->x = nrand() * 2.0 - 1.0;
		pnt->y = nrand() * 2.0 - 1.0;
		pnt->z = 0.0;
	}
}
