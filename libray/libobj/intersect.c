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

static void AddToHitList();
/*
 * Number of bounding volume tests.
 * External modules have read access via IntersectStats().
 */
static unsigned long BVTests;

/*
 * Intersect object & ray.  Return distance from "pos" along "ray" to
 * intersection point.  Return value <= 0 indicates no intersection.
 */
int
intersect(obj, ray, hitlist, mindist, maxdist)
Geom *obj;				/* Geom to be tested. */
Ray *ray;				/* Ray origin, direction. */
HitList *hitlist;			/* Intersection path */
Float mindist, *maxdist;
{
	Ray newray;
	Vector vtmp;
	Trans *curtrans;	
	Float distfact, nmindist, nmaxdist;

	/*
	 * Check ray/bounding volume intersection, if required.
	 */
	if (obj->methods->checkbounds) {
		VecAddScaled(ray->pos, mindist, ray->dir, &vtmp);
		if (OutOfBounds(&vtmp, obj->bounds)) {
			nmaxdist = *maxdist;
			BVTests++;
			if (!BoundsIntersect(ray, obj->bounds, mindist,
							&nmaxdist))
				return FALSE;
		}
	}

	newray = *ray;
	nmindist = mindist;
	nmaxdist = *maxdist;

	/*
	 * Transform the ray if necessary.
	 */
	if (obj->trans != (Trans *)0) {
		/*
		 * If object's idea of the current time is wrong,
		 * update the transformations.
		 */
		if (obj->animtrans && !equal(obj->timenow, ray->time)) {
			TransResolveAssoc(obj->trans);
		}
				
		/*
		 * Transforming the ray can change the distance between
		 * the ray origin and the point of intersection.
		 * We save the amount the ray is "stretched" and later
		 * divide the computed distance by this amount.
		 */
		distfact = 1.;
		for (curtrans = obj->transtail; curtrans; 
		     curtrans = curtrans->prev)
			distfact *= RayTransform(&newray, &curtrans->itrans);
		nmindist *= distfact;
		nmaxdist *= distfact;
	}
	/*
	 * Geom has been updated to current time.
	 */
	obj->timenow = ray->time;

	/*
	 * Call correct intersection routine.
	 */
	if (IsAggregate(obj)) {
		/*
		 * Aggregate
		 */
		if (!(*obj->methods->intersect)
		     (obj->obj, &newray, hitlist, nmindist, &nmaxdist))
		return FALSE;
	} else {
		/*
		 * Primitive
		 */
		if (!(*obj->methods->intersect)
		      (obj->obj, &newray, nmindist, &nmaxdist))
			return FALSE;
		hitlist->nodes = 0;
	}

	/*
	 * Had a hit -- add ray, distance and object to tail of hitlist.
	 */
	AddToHitList(hitlist, &newray, nmindist, nmaxdist, obj);

	/*
	 * Set dist to distance to intersection point from the origin
	 * of the untransformed ray.
	 */
	if (obj->trans != (Trans *)0)
		*maxdist = nmaxdist / distfact;
	else
		*maxdist = nmaxdist;

	return TRUE;
}

static void
AddToHitList(hitlist, ray, mind, dist, obj)
HitList *hitlist;
Ray *ray;
Float mind, dist;
Geom *obj;
{
	HitNode *np;
	Trans *list;

	np = &hitlist->data[hitlist->nodes++];

	np->ray = *ray;
	np->obj = obj;
	np->mindist = mind;
	np->dist = dist;
	np->enter = 0;

	if (obj->trans) {
		/*
		 * Compute total transformation, forward and inverse,
		 * for this object, and store in hitlist for use later.
		 */
		TransCopy(obj->trans, &np->trans);
		for (list = obj->trans->next; list; list = list->next)
			TransCompose(&np->trans, list, &np->trans);
		np->dotrans = TRUE;
	} else
		np->dotrans = FALSE;
}

/*
 * Return intersection statistics.
 * Currently, this is limited to the # of bounding volume test performed.
 */
void
IntersectStats(bvtests)
unsigned long *bvtests;
{
	*bvtests = BVTests;
}
