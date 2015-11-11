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
#ifndef VECTOR_H
#define VECTOR_H
/*
 * Constants used in projecting onto planes
 */
#define XNORMAL		(char)0
#define YNORMAL		(char)1
#define ZNORMAL		(char)2

/*
 * Maximum length
 */
#define FAR_AWAY		1.0E+14

typedef struct {
	Float u, v;			/* 2D point */
} Vec2d;

typedef struct Vector {
	Float x, y, z;			/* 3D point */
} Vector;

/*
 * Linked list of points
 */
typedef struct PointList {
	Vector	vec;			/* Vector data */
	struct	PointList *next;	/* Next in list */
} PointList;

/*
 * Project a point in 3-space to the plane whose normal is indicated by "i."
 */
#define VecProject(r, p, i)	{switch(i) { \
				case XNORMAL: \
					r.u = (p).y; \
					r.v = (p).z; \
					break; \
				case YNORMAL: \
					r.u = (p).x; \
					r.v = (p).z; \
					break; \
				case ZNORMAL: \
					r.u = (p).x; \
					r.v = (p).y; \
					break; \
  				} }

#define dotp(a, b)	(((a)->x*(b)->x)+((a)->y*(b)->y)+((a)->z*(b)->z))
#define VecSub(a,b,r) (r)->x=(a).x-(b).x,(r)->y=(a).y-(b).y,(r)->z=(a).z-(b).z
#define VecAdd(a,b,r) (r)->x=(a).x+(b).x,(r)->y=(a).y+(b).y,(r)->z=(a).z+(b).z
#define VecScale(s,a,r)  (r)->x=(s)*(a).x,(r)->y=(s)*(a).y,(r)->z=(s)*(a).z
#define VecComb(s1,v1,s2,v2,r)	(r)->x = (s1)*(v1).x + (s2)*(v2).x, \
				 (r)->y = (s1)*(v1).y + (s2)*(v2).y, \
				 (r)->z = (s1)*(v1).z + (s2)*(v2).z
#define VecAddScaled(v1,s,v2,r)	(r)->x = (v1).x + (s)*(v2).x, \
				 (r)->y = (v1).y + (s)*(v2).y, \
				 (r)->z = (v1).z + (s)*(v2).z

extern void	VecCross(), VecCoordSys(), MakeBump();
extern Float	VecNormCross(), VecNormalize();
extern int	Refract();

#endif /* VECTOR_H */
