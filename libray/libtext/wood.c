/*
 * Copyright (C) 1989-2015, Craig E. Kolb, Robert F. Skinner
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
#include "wood.h"

Wood *
WoodCreate()
{
	return (Wood *)NULL;	/* No data associated with wood texture */
}

/*ARGSUSED*/
void
WoodApply(wood, prim, ray, pos, norm, gnorm, surf)
Wood *wood;
Geom *prim;
Ray *ray;
Vector *pos, *norm, *gnorm;
Surface *surf;
{
	Float red, grn, blu;
	Float chaos, brownLayer, greenLayer;
	Float perturb, brownPerturb, greenPerturb, grnPerturb;
	Float t;

	chaos = Chaos(pos, 7);
	t = sin(sin(8.*chaos + 7*pos->x +3.*pos->y));

	greenLayer = brownLayer = fabs(t);

	perturb = sin(40.*chaos + 50.*pos->z);
	perturb = fabs(perturb);

	brownPerturb = .6*perturb + 0.3;
	greenPerturb = .2*perturb + 0.8;
	grnPerturb = .15*perturb + 0.85;
	grn = 0.5 * pow(fabs(brownLayer), 0.3);
	brownLayer = pow(0.5 * (brownLayer+1.0), 0.6) * brownPerturb;
	greenLayer = pow(0.5 * (greenLayer+1.0), 0.6) * greenPerturb;

	red = (0.5*brownLayer + 0.35*greenLayer)*2.*grn;
	blu = (0.25*brownLayer + 0.35*greenLayer)*2.0*grn;
	grn *= max(brownLayer, greenLayer) * grnPerturb;

	surf->diff.r *= red;
	surf->diff.g *= grn;
	surf->diff.b *= blu;
	surf->amb.r *= red;
	surf->amb.g *= grn;
	surf->amb.b *= blu;
}
