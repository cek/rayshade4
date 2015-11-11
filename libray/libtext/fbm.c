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
#include "fbm.h"

FBm *
FBmCreate(offset, scale, h, lambda, octaves, thresh, mapname)
Float h, lambda, scale, offset, thresh;
int octaves;
char *mapname;
{
	FBm *fbm;

	fbm = (FBm *)Malloc(sizeof(FBm));

	fbm->beta = 1. + 2*h;
	fbm->omega = pow(lambda, -0.5*fbm->beta);
	fbm->lambda = lambda;
	fbm->scale = scale;
	fbm->offset = offset;
	fbm->thresh = thresh;
	fbm->octaves = octaves;
	if (mapname != (char *)NULL)
		fbm->colormap = ColormapRead(mapname);
	else
		fbm->colormap = (Color *)NULL;
	return fbm;
}

void
FBmApply(fbm, prim, ray, pos, norm, gnorm, surf)
FBm *fbm;
Geom *prim;
Ray *ray;
Vector *pos, *norm, *gnorm;
Surface *surf;
{
	Float val;
	int index;

	val = fBm(pos, fbm->omega, fbm->lambda, fbm->octaves);
	if (val < fbm->thresh)
		val = fbm->offset;
	else
		val = fbm->offset + fbm->scale*(val - fbm->thresh);
	if (fbm->colormap) {
		index = 255. * val;
		if (index > 255) index = 255;
		if (index < 0) index = 0;
		surf->diff.r *= fbm->colormap[index].r;
		surf->diff.g *= fbm->colormap[index].g;
		surf->diff.b *= fbm->colormap[index].b;
		surf->amb.r *= fbm->colormap[index].r;
		surf->amb.g *= fbm->colormap[index].g;
		surf->amb.b *= fbm->colormap[index].b;
	} else {
		ColorScale(val, surf->diff, &surf->diff);
		ColorScale(val, surf->amb, &surf->amb);
	}
}
