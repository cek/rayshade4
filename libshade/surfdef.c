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
#include "libsurf/surface.h"

static Surface *Surfaces;	/* Named surfaces */

Surface DefaultSurface = {
		"DeFault",		/* name */
		{0.1, 0.1, 0.1},	/* ambient */
		{0.6, 0.6, 0.6},	/* diffuse */
		{0.5, 0.5, 0.5},	/* specular */
		{0.0, 0.0, 0.0},	/* Diffuse transmission 'curve' */
		{1.0, 1.0, 1.0},	/* Specular transmission 'curve' */
		12.,			/* reflected Phong coef */
		12.,			/* transmitted Phong coef */
		1.,			/* spec. transmitted attenuation */
		DEFAULT_INDEX,		/* index of refr */
		0.,			/* reflectivity */
		0.,			/* transparency */
		0.,			/* translucency */
		FALSE,			/* noshadow */
		NULL,			/* next */
};

Surface *SurfaceGetNamed(), *SurfaceFetchNamed();

/*
 * Add surf to the list of defined surfaces.
 */
void
SurfaceAddToDefined(surf)
Surface *surf;
{
	/*
	 * Make sure index of refraction isn't bogus.
	 */
	if (surf->transp > EPSILON && surf->index <= 0.)
		RLerror(RL_PANIC,
			"Index of refraction must be positive.\n");

	if (surf->name == (char *)NULL || *surf->name == (char)NULL)
		RLerror(RL_PANIC, "Surface with NULL name defined.\n");

	if (SurfaceFetchNamed(surf->name) != (Surface *)NULL)
		RLerror(RL_WARN,
			"Redefinition of \"%s\" surface.", surf->name);

	surf->next = Surfaces;
	Surfaces = surf;
}

/*
 * Search for surface with given name.  If not found, complain and exit.
 */
Surface *
SurfaceGetNamed(name)
char *name;
{
	Surface *stmp;

	stmp = SurfaceFetchNamed(name);
	if (stmp == (Surface *)NULL)
		RLerror(RL_PANIC, "Undefined surface \"%s\".", name);

	return stmp;
}

/*
 * Return pointer to surface with given name, NULL if no such surface.
 */
Surface *
SurfaceFetchNamed(name)
char *name;
{
	Surface *stmp;

	for (stmp = Surfaces; stmp ; stmp = stmp->next)
		if(strcmp(name, stmp->name) == 0)
			return stmp;
	/*
	 * No surface named "name".
	 */
	return (Surface *)NULL;
}

/*
 * Traverse the given hitlist to find the "bottom-most" surface.
 * If no surface is found, use the default.
 */
Surface *
GetShadingSurf(hitlist)
HitList *hitlist;
{
	int i;

	/*
	 * -1 here because the World always has a NULL surface
	 * (DefaultSurf is used instead)
	 */
	for (i = 0; i < hitlist->nodes -1; i++) {
		if (hitlist->data[i].obj->surf)
			return hitlist->data[i].obj->surf;
	}
	/*
	 * No suface found -- use the default.
	 */
	return &DefaultSurface;
}
