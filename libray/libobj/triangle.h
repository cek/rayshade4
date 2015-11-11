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
#ifndef TRIANGLE_H
#define TRIANGLE_H

#define FLATTRI		0
#define PHONGTRI	1

#define GeomTriangleCreate(t,a,b,c,d,e,f,g,h,i,s)  GeomCreate( \
		(GeomRef)TriangleCreate(t,a,b,c,d,e,f,g,h,i,s), TriangleMethods())

/*
 * Triangle
 */
typedef struct {
	Vector	nrm,		/* triangle normal */
		p[3],		/* vertices */
		e[3],		/* "edge" vectors (scaled) */
		*vnorm,		/* Array of vertex normals */
		*dpdu, *dpdv;	/* U and V direction vectors */
	Float	d,		/* plane constant  */
		b[3];		/* Array of barycentric coordinates */
	Vec2d	*uv;		/* Array of UV coordinates of vertices */
	char	index,		/* Flag used for shading/intersection test. */
		type;		/* type (to detect if phong or flat) */
} Triangle;

extern Triangle	*TriangleCreate();
extern int	TriangleIntersect(), TriangleNormal();
extern void	TriangleBounds(), TriangleUV(),
		TriangleStats();
extern Methods	*TriangleMethods();
char		*TriangleName();
#endif /* TRIANGLE_H */
