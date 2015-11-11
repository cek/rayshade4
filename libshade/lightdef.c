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
#include "rayshade.h"
#include "options.h"
#include "liblight/light.h"
#include "liblight/infinite.h"	/* to create default infinite light */
#include "liblight/jittered.h"	/* to create jittered light sources */

Light *Lights = NULL;		/* Linked list of defined lights */

void
LightAddToDefined(light)
Light *light;
{
	if (light) {
		light->next = Lights;
		Lights = light;
	}
}

void
LightSetup()
{
	long shadowopts;
	Light *ltmp;

	/*
	 * Set shadowing options.
	 */
	shadowopts = 0;
	if (Options.no_shadows)
		shadowopts |= SHADOW_NONE;
	if (Options.shadowtransp)
		shadowopts |= SHADOW_TRANSP;
	if (Options.csg)
		shadowopts |= SHADOW_CSG;
	if (Options.cache)
		shadowopts |= SHADOW_CACHE;
	if (Options.shutterspeed > 0.)
		shadowopts |= SHADOW_BLUR;
	ShadowSetOptions(shadowopts);

	/*
	 * If no light sources were defined, add a default.
	 */
	if (Lights == (Light *)NULL) {
		Color ctmp;
		Vector vtmp;
		vtmp.x = vtmp.z = 1.;
		vtmp.y = -1;
		ctmp.r = ctmp.g = ctmp.b = 1.;

		LightAddToDefined(LightInfiniteCreate(&ctmp, &vtmp));
	}
	/*
	 * Now that we've parsed the input file, we know what
	 * maxlevel is, and we can allocate the correct amount of
	 * space for each light source's cache.
	 */
	for (ltmp = Lights; ltmp; ltmp = ltmp->next) {
		ltmp->cache = (ShadowCache *)Calloc(
			(unsigned)Options.maxdepth + 1, sizeof(ShadowCache));
	}
}

void
AreaLightCreate(color, corner, u, usamp, v, vsamp, shadow)
Color *color;
Vector *corner, *u, *v;
int usamp, vsamp, shadow;
{
	Vector vpos, curpos;
	int i, j, numlight;
	Float ulen, vlen;
	Color intens;
	Light *ltmp;

	if (usamp < 1 || vsamp < 1)
		RLerror(RL_ABORT, "Invalid area light specification.\n");

	numlight = usamp * vsamp;	/* Total number of jittered sources */

	/*
	 * Sum of all intensities is equal to specified intensity.
	 */
	intens.r = color->r / (Float)numlight;
	intens.g = color->g / (Float)numlight;
	intens.b = color->b / (Float)numlight;

	VecSub(*u, *corner, u);
	VecSub(*v, *corner, v);
	/*
	 * Make sure that u and v are not degenerate.
	 */
	ulen = VecNormalize(u);
	vlen = VecNormalize(v);
	if (ulen < EPSILON || vlen < EPSILON)
		RLerror(RL_ABORT, "Degenerate area light source.\n");
	/*
	 * Scale u and v such that they define the area covered by a
	 * single sample.
	 */
	VecScale(ulen / (Float)usamp, *u, u); 
	VecScale(vlen / (Float)vsamp, *v, v);
	/*
	 * For each sample...
	 */
	vpos = *corner;
	for (i = 0; i < vsamp; i++) {
		curpos = vpos;
		for (j = 0; j < usamp; j++) {
			/*
			 * current pos is the "corner" of a new light
			 * source.  A jittered position based on
			 * the "corner" and the two edge vectors
			 * is used as the position for the
			 * light source in lighting calculations.
			 */
			ltmp = LightJitteredCreate(&intens, &curpos, u, v);
			ltmp->shadow = shadow;
			LightAddToDefined(ltmp);
			VecAdd(curpos, *u, &curpos);
		}
		VecAdd(vpos, *v, &vpos);
	}
}
