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
#include "jittered.h"

static LightMethods *iJitteredMethods = NULL;

Jittered *
JitteredCreate(pos, e1, e2)
Vector *pos, *e1, *e2;
{
	Jittered *j;

	j = (Jittered *)share_malloc(sizeof(Jittered));

	j->pos = *pos;
	j->e1 = *e1;
	j->e2 = *e2;

	return j;
}

LightMethods *
JitteredMethods()
{
	if (iJitteredMethods == (LightMethods *)NULL) {
		iJitteredMethods = LightMethodsCreate();
		iJitteredMethods->intens = JitteredIntens;
		iJitteredMethods->dir = JitteredDirection;
	}
	return iJitteredMethods;
}

int
JitteredIntens(jit, lcolor, cache, ray, dist, noshadow, color)
Jittered *jit;
Color *lcolor, *color;
ShadowCache *cache;
Ray *ray;
Float dist;
int noshadow;
{
	return !Shadowed(color, lcolor, cache, ray, dist, noshadow);
}

void
JitteredDirection(lp, pos, dir, dist)
Jittered *lp;
Vector *pos, *dir;
Float *dist;
{
	/*
	 * Choose a location with the area define by corner, e1
	 * and e2 at which this sample will be taken.
	 */
	VecAddScaled(lp->pos, nrand(), lp->e1, &lp->curpos);
	VecAddScaled(lp->curpos, nrand(), lp->e2, &lp->curpos);
	VecSub(lp->curpos, *pos, dir);
	*dist = VecNormalize(dir);
}

JitteredMethodRegister(meth)
UserMethodType meth;
{
	if (iJitteredMethods)
		iJitteredMethods->user = meth;
}
