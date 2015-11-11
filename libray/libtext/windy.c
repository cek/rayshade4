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
#include "windy.h"

/*
 * Create and return a reference to a "windy" texture.
 */
WindyText *
WindyCreate(scale, wscale, cscale, bscale, octaves, tscale, hscale, offset)
Float scale, wscale, cscale, bscale, tscale, hscale, offset;
int octaves;
{
	WindyText *windy;

	windy = (WindyText *)Malloc(sizeof(WindyText));
	windy->scale = scale;
	windy->windscale = wscale;
	windy->chaoscale = cscale;
	windy->bumpscale = bscale;
	windy->tscale = tscale;
	windy->hscale = hscale;
	windy->offset = offset;
	windy->octaves = octaves;
	return windy;
}

/*
 * Apply a "windy" texture.
 */
void
WindyApply(windy, prim, ray, pos, norm, gnorm, surf)
WindyText *windy;
Geom *prim;
Ray *ray;
Vector *pos, *norm, *gnorm;
Surface *surf;
{
	Vector bump;

	Windy(pos, windy->windscale, windy->chaoscale, windy->bumpscale,
	      windy->octaves, windy->tscale, windy->hscale, windy->offset,
	      &bump);

	norm->x += windy->scale * bump.x;
	norm->y += windy->scale * bump.y;
	norm->z += windy->scale * bump.z;
	VecNormalize(norm);
}
