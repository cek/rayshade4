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
#include "texture.h"
#include "marble.h"

MarbleText *
MarbleCreate(mapname)
char *mapname;
{
	MarbleText *marble;

	marble = (MarbleText *)Malloc(sizeof(MarbleText));
	if (mapname)
		marble->colormap = ColormapRead(mapname);
	else
		marble->colormap = (Color *)NULL;
	return marble;
}

void
MarbleApply(marble, prim, ray, pos, norm, gnorm, surf)
MarbleText *marble;
Geom *prim;
Ray *ray;
Vector *pos, *norm, *gnorm;
Surface *surf;
{
	Float val;
	int index;

	val = Marble(pos);
	if (marble->colormap) {
		index = (int)(255. * val);
		surf->diff.r *= marble->colormap[index].r;
		surf->diff.g *= marble->colormap[index].g;
		surf->diff.b *= marble->colormap[index].b;
		surf->amb.r *= marble->colormap[index].r;
		surf->amb.g *= marble->colormap[index].g;
		surf->amb.b *= marble->colormap[index].b;
	} else {
		ColorScale(val, surf->amb, &surf->amb);
		ColorScale(val, surf->diff, &surf->diff);
	}
}
