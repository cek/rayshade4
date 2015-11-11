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
#ifndef GRID_H
#define GRID_H

#define GeomGridCreate(x,y,z)	GeomCreate((GeomRef)GridCreate(x,y,z), \
					GridMethods())
/*
 * Convert from voxel number along X/Y/Z to corresponding coordinate.
 */
#define voxel2x(g,x)		((x) * g->voxsize[0] + g->bounds[0][0])
#define voxel2y(g,y)		((y) * g->voxsize[1] + g->bounds[0][1])
#define voxel2z(g,z)		((z) * g->voxsize[2] + g->bounds[0][2])
/*
 * And vice-versa.
 */
#define x2voxel(g,x)		(((x) - g->bounds[0][0]) / g->voxsize[0])
#define y2voxel(g,y)		(((y) - g->bounds[0][1]) / g->voxsize[1])
#define z2voxel(g,z)		(((z) - g->bounds[0][2]) / g->voxsize[2])

/*
 * Grid object
 */
typedef struct {
	short	xsize, ysize, zsize;	/* # of voxels along each axis */
	Float	bounds[2][3];		/* bounding box */
	Float	voxsize[3];		/* size of a voxel */
	struct	Geom	*unbounded,	/* unbounded objects */
			*objects;	/* all bounded objects */
	struct	GeomList	****cells;	/* Voxels */
} Grid;

extern char	*GridName();
extern void	*GirdBounds();
extern int	GridIntersect(), GridConvert();
extern Grid	*GridCreate();
extern Methods	*GridMethods();

#endif /* GRID_H */
