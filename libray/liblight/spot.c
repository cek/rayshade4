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
#include "spot.h"

static LightMethods *iSpotMethods = NULL;

Spotlight *
SpotCreate(from, to, coef, in, out)
Vector *from, *to;
Float coef, in, out;
{
	Spotlight *spot;

	spot = (Spotlight *)share_malloc(sizeof(Spotlight));
	spot->pos = *from;
	VecSub(*to, *from, &spot->dir);
	if (VecNormalize(&spot->dir) == 0. || in > out) {
		RLerror(RL_ABORT,"Invalid spotlight specification.\n");
		return (Spotlight *)NULL;
	}
	spot->coef = coef;
	spot->radius = cos(deg2rad(in));
	spot->falloff = cos(deg2rad(out));

	return spot;
}

LightMethods *
SpotMethods()
{
	if (iSpotMethods == (LightMethods *)NULL) {
		iSpotMethods = LightMethodsCreate();
		iSpotMethods->intens = SpotIntens;
		iSpotMethods->dir = SpotDirection;
	}
	return iSpotMethods;
}

/*
 * Calculate intensity ('color') of light reaching 'pos' from light 'lp'.
 * The spotlight is 'dist' units from 'pos' along 'dir'.
 *
 * Returns TRUE if non-zero illumination, FALSE otherwise.
 */
int
SpotIntens(spot, lcolor, cache, ray, dist, noshadow, color)
Spotlight *spot;
ShadowCache *cache;
Ray *ray;
Color *lcolor, *color;
int noshadow;
Float dist;
{
	Float atten;
	extern Float SpotAtten();

	/*
	 * Compute spotlight color
	 */
	atten = SpotAtten(spot, &ray->dir);
	/*
	 * If outside of spot, return FALSE.
	 */
	if (atten == 0.)
		return FALSE;
	if (Shadowed(color, lcolor, cache, ray, dist, noshadow))
		return FALSE;
	ColorScale(atten, *color, color);
	return TRUE;
}

/*
 * Compute intensity of spotlight along 'dir'.
 */
Float
SpotAtten(lp, dir)
Spotlight *lp;
Vector *dir;
{
	Float costheta, atten;
	extern Float rampup();

	costheta = -dotp(dir, &lp->dir);
	/*
	 * Behind spotlight.
	 */
	if (costheta <= 0.)
		return 0.;
	/*
	 * Intensity is the product of costheta raised to lp->coef and
	 * a function that smoothly interpolates from 0 at
	 * costheta=lp->falloff to 1 at costheta=lp->radius.
	 */
	atten = pow(costheta, lp->coef);
	if (lp->radius > 0.)
		atten *= rampup(lp->falloff, lp->radius, costheta);
	return atten;
}

/*
 * Cubic interpolation between 0 at left and 1 at right, sampled at 'at'
 * It is assumed that right >= left.
 */
Float
rampup(left, right, at)
Float left, right, at;
{
	if (at < left)
		return 0.;
	else if (at > right)
		return 1.;

	if (right == left)
		return 0.;

	at = (at - left) / (right - left);
	return (3 - 2*at)*at*at;
}

void
SpotDirection(lp, pos, dir, dist)
Spotlight *lp;
Vector *pos, *dir;
Float *dist;
{
	/*
	 * Calculate dir from position to center of light source.
	 */
	VecSub(lp->pos, *pos, dir);
	*dist = VecNormalize(dir);
}

SpotMethodRegister(meth)
UserMethodType meth;
{
	if (iSpotMethods)
		iSpotMethods->user = meth;
}
