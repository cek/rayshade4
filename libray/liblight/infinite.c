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
#include "infinite.h"

static LightMethods *iInfMethods = NULL;

Infinite *
InfiniteCreate(dir)
Vector *dir;
{
	Infinite *inf;

	inf = (Infinite *)share_malloc(sizeof(Infinite));
	inf->dir = *dir;
	if (VecNormalize(&inf->dir) == 0.) {
		RLerror(RL_ABORT, "Invalid directional light.\n");
		return (Infinite *)NULL;
	}
	return inf;
}

LightMethods *
InfiniteMethods()
{
	if (iInfMethods == (LightMethods *)NULL) {
		iInfMethods = LightMethodsCreate();
		iInfMethods->intens = InfiniteIntens;
		iInfMethods->dir = InfiniteDirection;
	}
	return iInfMethods;
}

int
InfiniteIntens(inf, lcolor, cache, ray, dist, noshadow, color)
Infinite *inf;
Color *lcolor, *color;
ShadowCache *cache;
Ray *ray;
Float dist;
int noshadow;
{
	return !Shadowed(color, lcolor, cache, ray, dist, noshadow);
}

void
InfiniteDirection(lp, pos, dir, dist)
Infinite *lp;
Vector *pos, *dir;
Float *dist;
{
	*dir = lp->dir;
	*dist = FAR_AWAY;
}

InfiniteMethodRegister(meth)
UserMethodType meth;
{
	if (iInfMethods)
		iInfMethods->user = meth;
}
