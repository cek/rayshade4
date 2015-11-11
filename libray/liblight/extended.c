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
#include "light.h"
#include "libcommon/sampling.h"
#include "extended.h"

static LightMethods *iExtendedMethods = NULL;

Extended *
ExtendedCreate(r, pos)
Float r;
Vector *pos;
{
	Extended *e;

	e = (Extended *)share_malloc(sizeof(Extended));
	e->pos = *pos;
	e->radius = r;
	
	return e;
}

LightMethods *
ExtendedMethods()
{
	if (iExtendedMethods == (LightMethods *)NULL) {
		iExtendedMethods = LightMethodsCreate();
		iExtendedMethods->intens = ExtendedIntens;
		iExtendedMethods->dir = ExtendedDirection;
	}
	return iExtendedMethods;
}

/*
 * Compute intensity ('color') of extended light source 'lp' from 'pos'.
 */
static int
ExtendedIntens(lp, lcolor, cache, ray, dist, noshadow, color)
Extended *lp;
Color *lcolor, *color;
ShadowCache *cache;
Ray *ray;
Float dist;
int noshadow;
{
	int uSample, vSample, islit;
	Float jit, vbase, ubase, vpos, upos, lightdist;
	Color newcol;
	Ray newray;
	Vector Uaxis, Vaxis, ldir;

	if (noshadow) {
		*color = *lcolor;
		return TRUE;
	}

	newray = *ray;
	/*
	 * Determinte two orthoganal vectors that lay in the plane
	 * whose normal is defined by the vector from the center
	 * of the light source to the point of intersection and
	 * passes through the center of the light source.
 	 */
	VecSub(lp->pos, ray->pos, &ldir);
	VecCoordSys(&ldir, &Uaxis, &Vaxis);

	jit = 2. * lp->radius * Sampling.spacing;

	/*
	 * Sample a single point, determined by SampleNumber,
	 * on the extended source.
	 */
	vpos = -lp->radius + (ray->sample % Sampling.sidesamples)*jit;
	upos = -lp->radius + (ray->sample / Sampling.sidesamples)*jit;
	vpos += nrand() * jit;
	upos += nrand() * jit;
	VecComb(upos, Uaxis, vpos, Vaxis, &newray.dir);
	VecAdd(ldir, newray.dir, &newray.dir);
	lightdist = VecNormalize(&newray.dir);

	return !Shadowed(color, lcolor, cache, &newray,
		lightdist, noshadow);
}

void
ExtendedDirection(lp, pos, dir, dist)
Extended *lp;
Vector *pos, *dir;
Float *dist;
{
	/*
	 * Calculate dir from position to center of
	 * light source.
	 */
	VecSub(lp->pos, *pos, dir);
	*dist = VecNormalize(dir);
}

ExtendedMethodRegister(meth)
UserMethodType meth;
{
	if (iExtendedMethods)
		iExtendedMethods->user = meth;
}
