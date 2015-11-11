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
#ifndef SURFACE_H
#define SURFACE_H

#define DEFAULT_INDEX		1.0	/* Default index of refraction */
#define DEFAULT_PHONGPOW	15.0	/* Default specular highlight exp */

/*
 * Surface definition.
 */
typedef struct Surface {
	char	*name;			/* Name */
	struct Color	amb,		/* Ambient 'curve' */
			diff,		/* Diffuse reflection 'curve' */
			spec,		/* Specular reflection 'curve' */
			translu,	/* Diffuse transmission 'curve' */
			body;		/* Specular transmission 'curve' */
	Float	srexp,			/* Specular reflection exponent */
		stexp,			/* Specular transmission exponent */
		statten,		/* Specular transmission attenuation */
		index,			/* Index of refraction */
		reflect,		/* Specular reflectivity */
		transp,			/* Specular transmittance */
		translucency;		/* Diffuse transmittance */ 
	char	noshadow;		/* No shadowing? */
	struct Surface *next;		/* Next surface in list (if any) */
} Surface;

/*
 * Linked list of (linked list of) surfaces.
 */
typedef struct SurfList {
	Surface *surf;
	struct SurfList *next;
} SurfList;

extern Surface	*SurfaceCreate(),
		*GetShadingSurf(),	/* application-provided */
		*SurfaceCopy();

extern SurfList	*SurfPop(), *SurfPush();

extern void	SurfaceBlend(), ColorBlend();

extern int	ComputeSurfProps();

#endif /* SURFACE_H */
