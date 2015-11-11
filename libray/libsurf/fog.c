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
#include "fog.h"

Fog *
FogCreate(color, trans)
Color *color, *trans;
{
	Fog *fog;
	static void ComputeFog();

	fog = (Fog *)Malloc(sizeof(Fog));

	if (color == (Color *)NULL)
		fog->color.r = fog->color.g = fog->color.b = 0.;
	else
		fog->color = *color;
	if (trans == (Color *)NULL)
		fog->trans.r = fog->trans.g = fog->trans.b = FAR_AWAY;
	else {
		fog->trans = *trans;
	}
	return fog;
}

/*
 * Add fog to the given color.
 */
void
FogApply(fog, ray, pos, dist, color)
Fog *fog;
Ray *ray;
Vector *pos;
Float dist;
Color *color;
{
	Float atten;
	extern Float ExpAtten();

	atten = ExpAtten(dist, fog->trans.r);
	if (fog->trans.r == fog->trans.g && fog->trans.r == fog->trans.b) {
		ColorBlend(color, &fog->color, atten, 1. - atten);
		return;
	}
	color->r = atten*color->r + (1. - atten) * fog->color.r;

	atten = ExpAtten(dist, fog->trans.g);
	color->g = atten*color->g + (1. - atten) * fog->color.g;
	atten = ExpAtten(dist, fog->trans.b);
	color->b = atten*color->b + (1. - atten) * fog->color.b;
}
