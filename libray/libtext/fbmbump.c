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
#include "fbmbump.h"

FBm *
FBmBumpCreate(offset, scale, h, lambda, octaves)
Float offset, h, lambda, scale;
int octaves;
{
	FBm *fbm;
	fbm = FBmCreate(offset, scale, h, lambda, octaves, 0., (char *)NULL);
	return fbm;
}

/*ARGSUSED*/
void
FBmBumpApply(fbm, prim, ray, pos, norm, gnorm, surf)
FBm *fbm;
Geom *prim;
Ray *ray;
Vector *pos, *norm, *gnorm;
Surface *surf;
{
	Vector disp;

	VfBm(pos, fbm->omega, fbm->lambda, fbm->octaves, &disp);
	norm->x += fbm->offset + disp.x * fbm->scale;
	norm->y += fbm->offset + disp.y * fbm->scale;
	norm->z += fbm->offset + disp.z * fbm->scale;
}
