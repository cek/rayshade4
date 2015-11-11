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
#include "geom.h"
#include "plane.h"

static Methods *iPlaneMethods = NULL;
static char planeName[] = "plane";

unsigned long PlaneTests, PlaneHits;

/*
 * create plane primitive
 */
Plane *
PlaneCreate(pos, norm)
Vector *pos, *norm;
{
	Plane *plane;
	Vector tmpnrm;

	tmpnrm = *norm;
	if (VecNormalize(&tmpnrm) == 0.) {
		RLerror(RL_WARN, "Degenerate plane normal.\n");
		return (Plane *)NULL;
	}
	plane = (Plane *)share_malloc(sizeof(Plane));
	plane->norm = tmpnrm;
	plane->pos = *pos;
	plane->d = dotp(&plane->norm, pos);

	return plane;
}

Methods *
PlaneMethods()
{
	if (iPlaneMethods == (Methods *)NULL) {
		iPlaneMethods = MethodsCreate();
		iPlaneMethods->name = PlaneName;
		iPlaneMethods->create = (GeomCreateFunc *)PlaneCreate;
		iPlaneMethods->methods = PlaneMethods;
		iPlaneMethods->intersect = PlaneIntersect;
		iPlaneMethods->normal = PlaneNormal;
		iPlaneMethods->uv = PlaneUV;
		iPlaneMethods->bounds = PlaneBounds;
		iPlaneMethods->stats = PlaneStats;
		iPlaneMethods->checkbounds = FALSE;
		iPlaneMethods->closed = FALSE;
	}
	return iPlaneMethods;
}

int
PlaneIntersect(plane, ray, mindist, maxdist)
Plane *plane;
Ray *ray;
Float mindist, *maxdist;
{
	Float d;

	PlaneTests++;

	d = dotp(&plane->norm, &ray->dir);
	if (fabs(d) < EPSILON)
		return FALSE;
	d = (plane->d - dotp(&plane->norm, &ray->pos)) / d;

	if (d > mindist && d < *maxdist) {
		*maxdist = d;
		PlaneHits++;
		return TRUE;
	}
	return FALSE;
}

/*ARGSUSED*/
int
PlaneNormal(plane, pos, nrm, gnrm)
Plane *plane;
Vector *pos, *nrm, *gnrm;
{
	*gnrm = *nrm = plane->norm;
	return FALSE;
}

void
PlaneUV(plane, pos, norm, uv, dpdu, dpdv)
Plane *plane;
Vector *pos, *norm, *dpdu, *dpdv;
Vec2d *uv;
{
	Vector vec, du, dv;

	VecCoordSys(norm, &du, &dv);
	VecSub(*pos, plane->pos, &vec);

	uv->u = dotp(&vec, &du);
	uv->v = dotp(&vec, &dv);

	if (dpdu)
		*dpdu = du;
	if (dpdv)
		*dpdv = dv;
}
	
/*ARGSUSED*/
void
PlaneBounds(plane, bounds)
Plane *plane;
Float bounds[2][3];
{
	/*
	 * Planes are unbounded by nature.  minx > maxx signifies
	 * this.
	 */
	bounds[LOW][X] = 1.0;
	bounds[HIGH][X] = -1.0;
}

char *
PlaneName()
{
	return planeName;
}

void
PlaneStats(tests, hits)
unsigned long *tests, *hits;
{
	*tests = PlaneTests;
	*hits = PlaneHits;
}

void
PlaneMethodRegister(meth)
UserMethodType meth;
{
	if (iPlaneMethods)
		iPlaneMethods->user = meth;
}
