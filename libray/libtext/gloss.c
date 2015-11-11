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
#include "gloss.h"

Gloss *
GlossCreate(glossiness)
Float glossiness;
{
	Gloss *gloss;

	gloss = (Gloss *)Malloc(sizeof(Gloss));
	gloss->glossy = 1. - glossiness;
	return gloss;
}

void
GlossApply(gloss, prim, ray, pos, norm, gnorm, surf)
Gloss *gloss;
Geom *prim;
Ray *ray;
Vector *pos, *norm, *gnorm;
Surface *surf;
{
	Vector uaxis, vaxis, point, norminc;
	extern void UnitCirclePoint();

	/*
	 * Find coordinate system with norm as the Z axis.
	 */
	VecCoordSys(norm, &uaxis, &vaxis);
	/*
	 * Find point on unit circle based on sample #.
	 */
	UnitCirclePoint(&point, ray->sample);
	/*
	 * Perturb normal appropriately.
	 */
	VecComb(gloss->glossy * point.x, uaxis,
		gloss->glossy * point.y, vaxis,
		&norminc);
	VecAdd(*norm, norminc, norm);
	/*
	 * Renormalize.
	 */
	(void)VecNormalize(norm);
}
