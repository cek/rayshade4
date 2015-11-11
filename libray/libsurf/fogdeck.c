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
#include "fogdeck.h"

Fogdeck *
FogdeckCreate(alt, offset, scale, chaoscale, octaves, color, trans)
Float alt, offset, chaoscale;
Vector *scale;
int octaves;
Color *color, *trans;
{
	Fogdeck *fogdeck;
	static void ComputeFogdeck();

	fogdeck = (Fogdeck *)Malloc(sizeof(Fogdeck));

	fogdeck->alt = alt;
	fogdeck->octaves = octaves;
	fogdeck->scale = *scale;
	fogdeck->chaoscale = chaoscale;
	fogdeck->offset = offset;

	if (color == (Color *)NULL)
		fogdeck->color.r = fogdeck->color.g = fogdeck->color.b = 0.;
	else
		fogdeck->color = *color;
	if (trans == (Color *)NULL)
		fogdeck->trans.r = fogdeck->trans.g = fogdeck->trans.b =
			FAR_AWAY;
	else {
		fogdeck->trans = *trans;
	}
	return fogdeck;
}

/*
 * Add fogdeck to the given color.
 */
void
FogdeckApply(fogdeck, ray, pos, dist, color)
Fogdeck *fogdeck;
Ray *ray;
Vector *pos;
Float dist;
Color *color;
{
	Float atten, hitdist, density;
	Color trans;
	Vector endp;
	extern Float ExpAtten(), PAChaos();

	/*
	 * Find distance from origin at which ray strikes
	 * z = fogdeck->alt plane
	 */
	if (abs(ray->dir.z) < EPSILON)
		return;
	hitdist = (fogdeck->alt - ray->pos.z) / ray->dir.z;
	if (hitdist < EPSILON || hitdist > dist)
		return;
	/*
	 * Compute ray endpoint
	 */
	VecAddScaled(ray->pos, hitdist, ray->dir, &endp);

	/*
	 * Modify transmissivity based on point of
	 * intersection.
	 */
	endp.x *= fogdeck->scale.x;
	endp.y *= fogdeck->scale.y;
	endp.z *= fogdeck->scale.z;

	density = fogdeck->offset +
			fogdeck->chaoscale * PAChaos(&endp, fogdeck->octaves);
	if (density < EPSILON)
		density = HUGE;
	else
		density = 1. / density;

	trans = fogdeck->trans;
	ColorScale(density, trans, &trans);

	dist -= hitdist;
	
	atten = ExpAtten(dist, trans.r);

	if (trans.r == trans.g &&
	    trans.r == trans.b) {
		ColorBlend(color, &fogdeck->color, atten, 1. - atten);
		return;
	}
	color->r = atten*color->r + (1. - atten) * fogdeck->color.r;

	atten = ExpAtten(dist, trans.g);
	color->g = atten*color->g + (1. - atten) * fogdeck->color.g;
	atten = ExpAtten(dist, trans.b);
	color->b = atten*color->b + (1. - atten) * fogdeck->color.b;
}
