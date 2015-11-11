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
#include "mount.h"

/*
 * Create and return a reference to a "mount" texture.
 */
Mount *
MountCreate(cmap, turb, slope)
char *cmap;
Float turb, slope;
{
	Mount *mount;

	mount = (Mount *)Malloc(sizeof(Mount));
	mount->turb = turb;
	mount->slope = slope;
	mount->cmap = ColormapRead(cmap);
	return mount;
}

/*
 * Apply a "mount" texture.
 */
void
MountApply(mount, prim, ray, pos, norm, gnorm, surf)
Mount *mount;
Geom *prim;
Ray *ray;
Vector *pos, *norm, *gnorm;
Surface *surf;
{
	int index;
	Float t;
	Color c;

	t = Chaos(pos, 7);
	index = (pos->z + mount->turb*t - mount->slope*(1.-norm->z))*256;
	if (index < 0)
		index = 0;
	if (index > 255)
		index = 255;

	ColorMultiply(surf->amb, mount->cmap[index], &surf->amb);
	ColorMultiply(surf->diff, mount->cmap[index], &surf->diff);
	ColorMultiply(surf->spec, mount->cmap[index], &surf->spec);
}
