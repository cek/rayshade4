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
#include "disc.h"

static Methods *iDiscMethods = NULL;
static char discName[] = "disc";

unsigned long DiscTests, DiscHits;

Disc *
DiscCreate(r, pos, norm)
Float r;
Vector *pos, *norm;
{
	Disc *disc;		/* Pointer to new disc. */

	if (r < EPSILON) {
		RLerror(RL_WARN, "Degenerate disc.\n");
	        /*
	         * Don't create this primitive.
	         */
	        return (Disc *)NULL;
	}

	if (VecNormalize(norm) == 0.) {
		RLerror(RL_WARN, "Degenerate disc normal.\n");
		return (Disc *)NULL;
	}
	/*
	 * Allocate new Disc.
	 */
	disc = (Disc *)share_malloc(sizeof(Disc));
	/*
	 * Initialize new disc.
	 * We store the square of the radius to save us a sqrt().
	 */
	disc->radius = r*r;
	disc->pos = *pos;
	disc->norm = *norm;
	/*
	 * Compute plane constant.
	 */
	disc->d = dotp(pos, norm);
	/*
	 * Allocate new primitive
	 */
	return disc;
}

Methods *
DiscMethods()
{
	if (iDiscMethods == (Methods *)NULL) {
		iDiscMethods = MethodsCreate();
		iDiscMethods->name = DiscName;
		iDiscMethods->create = (GeomCreateFunc *)DiscCreate;
		iDiscMethods->methods = DiscMethods;
		iDiscMethods->intersect = DiscIntersect;
		iDiscMethods->normal = DiscNormal;
		iDiscMethods->uv = DiscUV;
		iDiscMethods->bounds = DiscBounds;
		iDiscMethods->stats = DiscStats;
		iDiscMethods->checkbounds = FALSE;
		iDiscMethods->closed = FALSE;
	}
	return iDiscMethods;
}

int
DiscIntersect(disc, ray, mindist, maxdist)
Disc *disc;
Ray *ray;
Float mindist, *maxdist;
{
	Vector hit;
	Float denom, dist;

	DiscTests++;

	denom = dotp(&disc->norm, &ray->dir);
	if (fabs(denom) < EPSILON)
		/* Edge-on intersection */
		 return FALSE;

	dist = (disc->d - dotp(&disc->norm, &ray->pos)) / denom;
	if (dist < mindist || dist > *maxdist)
		/* Too close or too far */
		return FALSE;
	/*
	 *  Find difference between point of intersection and center of disc.
	 */
	VecAddScaled(ray->pos, dist, ray->dir, &hit);
	VecSub(hit, disc->pos, &hit);
	/*
	 * If hit point is <= disc->radius from center, we've hit the disc.
	 */
	if (dotp(&hit, &hit) <= disc->radius) {
		*maxdist = dist;
		DiscHits++;
		return TRUE;
	}
	return FALSE;
}

int
DiscNormal(disc, pos, nrm, gnrm)
Disc *disc;
Vector *pos, *nrm, *gnrm;
{
	*gnrm = *nrm = disc->norm;
	return FALSE;
}

void
DiscUV(disc, pos, norm, uv, dpdu, dpdv)
Disc *disc;
Vector *pos, *norm, *dpdu, *dpdv;
Vec2d *uv;
{
	Float dist, val;

	dist =	(pos->x - disc->pos.x) * (pos->x - disc->pos.x) +
		(pos->y - disc->pos.y) * (pos->y - disc->pos.y) +
		(pos->z - disc->pos.z) * (pos->z - disc->pos.z);

	if (dist < EPSILON) {
		uv->u = uv->v = 0.;
		return;
	}

	dist = sqrt(dist);
	uv->v = dist / sqrt(disc->radius); /* should store r and r*r */

	val = pos->x / dist;

	if (fabs(val) > 1.)
		uv->u = 0.5;
	else {
		uv->u = acos(val) / TWOPI; 
		if (pos->y < 0.)
			uv->u = 1. - uv->u;
	}

	if (dpdu) {
		VecSub(*pos, disc->pos, dpdv);
		/* dpdu = dpdv X norm */
		VecCross(dpdv, norm, dpdu);
	}
}

void
DiscBounds(disc, bounds)
Disc *disc;
Float bounds[2][3];
{
	Float extent, rad;

	rad = sqrt(disc->radius);
	/*
	 * Project disc along each of X, Y and Z axes.
	 */
	extent = rad * sqrt(1. - disc->norm.x * disc->norm.x);
	bounds[LOW][X] = disc->pos.x - extent;
	bounds[HIGH][X] = disc->pos.x + extent;
	extent = rad * sqrt(1. - disc->norm.y * disc->norm.y);
	bounds[LOW][Y] = disc->pos.y - extent;
	bounds[HIGH][Y] = disc->pos.y + extent;
	extent = rad * sqrt(1. - disc->norm.z * disc->norm.z);
	bounds[LOW][Z] = disc->pos.z - extent;
	bounds[HIGH][Z] = disc->pos.z + extent;
}

char *
DiscName()
{
	return discName;
}

void
DiscStats(tests, hits)
unsigned long *tests, *hits;
{
	*tests = DiscTests;
	*hits = DiscHits;
}

void
DiscMethodRegister(meth)
UserMethodType meth;
{
	if (iDiscMethods)
		iDiscMethods->user = meth;
}
