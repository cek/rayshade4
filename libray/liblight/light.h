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
#ifndef LIGHT_H
#define LIGHT_H

#include "libobj/geom.h"

#define SHADOW_NONE	001
#define SHADOW_TRANSP	002
#define SHADOW_CSG	004
#define SHADOW_CACHE	010
#define SHADOW_BLUR	020

#define NOSHADOWS(f)	((f) & SHADOW_NONE)
#define SHADOWTRANSP(f)	((f) & SHADOW_TRANSP)
#define SHADOWCSG(f)	((f) & SHADOW_CSG)
#define SHADOWCACHE(f)	((f) & SHADOW_CACHE)
#define SHADOWBLUR(f)	((f) & SHADOW_BLUR)

#define SHADOW_EPSILON	(4. * EPSILON)

typedef char * LightRef;

typedef struct {
	struct Geom *obj;	/* Pointer to cached object */
	RSMatrix trans;	/* World-to-object transformation */
	char dotrans;		/* TRUE if above trans is non-identity */
} ShadowCache;

typedef struct {
	int	(*intens)();	/* intensity method */
	void	(*dir)(),	/* direction method */
		(*user)();	/* user-defined method */
} LightMethods;

typedef struct Light {
	Color color;		/* Light source color & intensity */
	int shadow;		/* Does light source cast shadows? */
	LightRef light;		/* Pointer to light information */
	LightMethods *methods;	/* Light source methods */
	ShadowCache *cache;	/* Shadow cache, if any */
	struct Light *next;	/* Next light in list */
} Light;

extern LightMethods	*LightMethodsCreate();
extern Light	*LightCreate();
extern void	LightAllocateCache(), LightAddToDefined();
extern int	LightIntens(), LightDirection();
extern void	ShadowSetOptions(), ShadowStats();

#endif /* LIGHT_H */
