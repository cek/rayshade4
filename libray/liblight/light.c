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

Light *
LightCreate(light, meth, color)
LightRef light;
LightMethods *meth;
Color *color;
{
	Light *ltmp;

	if (light == (LightRef)NULL || meth == (LightMethods *)NULL)
		return (Light *)NULL;

	ltmp = (Light *)share_malloc(sizeof(Light));
	ltmp->light = light;
	ltmp->methods = meth;
	ltmp->color = *color;
	ltmp->next = (Light *)NULL;
	ltmp->cache = (ShadowCache *)NULL;
	ltmp->shadow = TRUE;
	return ltmp;
}

LightMethods *
LightMethodsCreate()
{
	return (LightMethods *)share_calloc(1, sizeof(LightMethods));
}

/*
 * Compute light color.  Returns FALSE if in full shadow, TRUE otherwise.
 * Computed light color is stored in 'color'.
 */
int
LightIntens(lp, ray, dist, noshadow, color)
Light *lp;
Ray *ray;
Float dist;
int noshadow;
Color *color;
{
	if (lp->methods->intens)
		return (*lp->methods->intens)(lp->light, &lp->color,
			lp->cache, ray, dist, noshadow || !lp->shadow, color);
	RLerror(RL_ABORT, "Cannot compute light intensity!\n");
	return FALSE;
}

/*
 * Calculate ray and distance from position to light.
 */
int
LightDirection(lp, objpos, lray, dist)
Light *lp;
Vector *objpos, *lray;
Float *dist;
{
	if (lp->methods->dir) {
		(*lp->methods->dir)(lp->light, objpos, lray, dist);
		return TRUE;
	} else {
		RLerror(RL_ABORT, "Cannot compute light direction!\n");
		return FALSE;
	}
}
