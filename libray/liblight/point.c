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
#include "point.h"

static LightMethods *iPointMethods = NULL;

Pointlight *
PointCreate(pos)
Vector *pos;
{
	Pointlight *p;

	p = (Pointlight *)share_malloc(sizeof(Pointlight));
	p->pos = *pos;
	return p;
}

LightMethods *
PointMethods()
{
	if (iPointMethods == (LightMethods *)NULL) {
		iPointMethods = LightMethodsCreate();
		iPointMethods->intens = PointIntens;
		iPointMethods->dir = PointDirection;
	}
	return iPointMethods;
}

int
PointIntens(lp, lcolor, cache, ray, dist, noshadow, color)
Pointlight *lp;
Color *lcolor, *color;
ShadowCache *cache;
Ray *ray;
Float dist;
int noshadow;
{
	return !Shadowed(color, lcolor, cache, ray, dist, noshadow);
}

void
PointDirection(lp, pos, dir, dist)
Pointlight *lp;
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

PointMethodRegister(meth)
UserMethodType meth;
{
	if (iPointMethods)
		iPointMethods->user = meth;
}
