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
#include "surface.h"

#define blend(a, b, p, q)	(a * p + b * q)

Color	Black = {0., 0., 0.},
	White = {1., 1., 1.};

/*
 * Create and return pointer to surface with given properties.
 */
Surface *
SurfaceCreate()
{
	Surface *stmp;

	stmp = (Surface *)Malloc(sizeof(Surface));

	stmp->amb = stmp->diff = stmp->spec =
		stmp->translu = Black;

	stmp->body = White;

	stmp->srexp = stmp->stexp = DEFAULT_PHONGPOW;
	stmp->statten = 1.;	/* No attenuation by default */

	stmp->reflect = stmp->transp = 0.;

	stmp->noshadow = FALSE;

	stmp->index = DEFAULT_INDEX;

	stmp->name = (char *)NULL;
	stmp->next = (Surface *)NULL;

	return stmp;
}

Surface *
SurfaceCopy(surf)
Surface *surf;
{
	Surface *res;

	if (!surf)
		return (Surface *)NULL;

	res = SurfaceCreate();
	*res = *surf;
	res->next = (Surface *)NULL;
	res->name = (char *)NULL;
	return res;
}

/*
 * Compute combination of two surfaces. Resulting surface is copied into surf1.
 */
void
SurfaceBlend(surf1, surf2, p, q)
Surface *surf1, *surf2;
Float p, q;
{
	/*
 	 * P is weight of surf1.  q is weight of surf2.
	 * Result is placed in surf1.
	 */
	if (q < EPSILON)
		return;	/* keep surf1 as-is */

	ColorBlend(&surf1->amb, &surf2->amb, p, q);
	ColorBlend(&surf1->diff, &surf2->diff, p, q);
	ColorBlend(&surf1->spec, &surf2->spec, p, q);
	ColorBlend(&surf1->translu, &surf2->translu, p, q);
	ColorBlend(&surf1->body, &surf2->body, p, q);

	surf1->srexp = blend(surf1->srexp, surf2->srexp, p, q);
	surf1->stexp = blend(surf1->stexp, surf2->stexp, p, q);

	surf1->reflect = blend(surf1->reflect, surf2->reflect, p, q);
	surf1->transp  = blend(surf1->transp,  surf2->transp,  p, q);
	surf1->translucency = blend(surf1->translucency, surf2->translucency,
			p, q);
	/*
	 * Questionable...
	 */
	surf1->statten = blend(surf1->statten, surf2->statten, p, q);
	surf1->index = blend(surf1->index, surf2->index, p, q);

	if (p < EPSILON) {
		surf1->noshadow = surf2->noshadow;
	} else {
		/* else there's a blend of some kind... */
		surf1->noshadow = (surf1->noshadow && surf2->noshadow);
	}
}

/*
 * Blend two colors.  Result is placed in color1.
 */
void
ColorBlend(color1, color2, p, q)
Color *color1, *color2;
Float p, q;
{
	color1->r = blend(color1->r, color2->r, p, q);
	color1->g = blend(color1->g, color2->g, p, q);
	color1->b = blend(color1->b, color2->b, p, q);
}

SurfList *
SurfPop(list)
SurfList *list;
{
	SurfList *stmp = list->next;

	free((voidstar)list);
	return stmp;
}

SurfList *
SurfPush(surf, list)
Surface *surf;
SurfList *list;
{
	SurfList *stmp;

	stmp = (SurfList *)Malloc(sizeof(SurfList));
	stmp->surf = surf;
	stmp->next = list;
	return stmp;
}
