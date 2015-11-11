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
#include "cylinder.h"

static Methods *iCylinderMethods = NULL;
static char cylName[] = "cylinder";

unsigned long CylTests, CylHits;

Cylinder *
CylinderCreate(r, bot, top)
Float r;
Vector *bot, *top;
{
	Cylinder *cyl;
	Float len;
	Vector axis;

	if (r <= 0.) {
		RLerror(RL_WARN, "Invalid cylinder radius.\n");
		return (Cylinder*)NULL;
	}

	VecSub(*top, *bot, &axis);

	len = VecNormalize(&axis);

	if (len < EPSILON) {
		RLerror(RL_WARN, "Degenerate cylinder.\n");
		return (Cylinder *)NULL;
	}

	cyl = (Cylinder *)share_malloc(sizeof(Cylinder));
	CoordSysTransform(bot, &axis, r, len, &cyl->trans);
	return cyl;
}

Methods *
CylinderMethods()
{
	if (iCylinderMethods == (Methods *)NULL) {
		iCylinderMethods = MethodsCreate();
		iCylinderMethods->name = CylinderName;
		iCylinderMethods->create = (GeomCreateFunc *)CylinderCreate;
		iCylinderMethods->methods = CylinderMethods;
		iCylinderMethods->intersect = CylinderIntersect;
		iCylinderMethods->normal = CylinderNormal;
		iCylinderMethods->uv = CylinderUV;
		iCylinderMethods->bounds = CylinderBounds;
		iCylinderMethods->stats = CylinderStats;
		iCylinderMethods->checkbounds = TRUE;
		iCylinderMethods->closed = FALSE;
	}
	return iCylinderMethods;
}

/*
 * Ray-cylinder intersection test.
 */
int
CylinderIntersect(cyl, ray, mindist, maxdist)
Cylinder *cyl;
Ray *ray;
Float mindist, *maxdist;
{
	Float t1, t2, a, b, c, zpos1, zpos2, disc;
	Float distfact;
	Ray newray;
	Vector nray, npos;
	Float nmin;

	CylTests++;

	/*
	 * Transform ray into canonical cylinder space.
	 */
	newray = *ray;
	distfact = RayTransform(&newray, &cyl->trans.itrans);
	nray = newray.dir;
	npos = newray.pos;
	nmin = mindist * distfact;

	a = nray.x * nray.x + nray.y * nray.y;
	if (a < EPSILON*EPSILON)
		/* |nray.z| == 1. */
		return FALSE;

	b = nray.x * npos.x + nray.y * npos.y;
	c = npos.x*npos.x + npos.y*npos.y - 1;
	disc = b*b - a*c;
	if(disc < 0.)
		return FALSE;
	disc = sqrt(disc);
	t1 = (-b + disc) / a;
	t2 = (-b - disc) / a;
	if (t1 < nmin && t2 < nmin)
		return FALSE;
	zpos1 = npos.z + t1 * nray.z;
	zpos2 = npos.z + t2 * nray.z;

	if (t1 < nmin || zpos1 < 0. || zpos1 > 1.) {
		if (t2 < nmin || zpos2 < 0. || zpos2 > 1.)
			return FALSE;
		else
			t1 = t2 / distfact;

	} else {
		if (t2 < nmin || zpos2 < 0. || zpos2 > 1.)
			t1 /= distfact;
		else {
			t1 = min(t1, t2) / distfact;
		}
	}
	
	if (t1 < *maxdist) {
		*maxdist = t1;
		CylHits++;
		return TRUE;
	}
	return FALSE;
}

int
CylinderNormal(cyl, pos, nrm, gnrm)
Cylinder *cyl;
Vector *pos, *nrm, *gnrm;
{
	/*
	 * Transform position into cylinder space.
	 */
	*nrm = *pos;
	PointTransform(nrm, &cyl->trans.itrans);
	/*
	 * The normal is equal to the point of intersection in cylinder
	 * space, but with Z = 0.;
	 */
	nrm->z = 0.;

	/*
	 * Tranform normal back to world space.
	 */
	NormalTransform(nrm, &cyl->trans.itrans);
	*gnrm = *nrm;
	return FALSE;
}

void
CylinderUV(cyl, pos, norm, uv, dpdu, dpdv)
Cylinder *cyl;
Vector *pos, *norm, *dpdu, *dpdv;
Vec2d *uv;
{
	Vector npos;

	npos = *pos;
	PointTransform(&npos, &cyl->trans.itrans);

	uv->v = npos.z;
	/*
	 * Due to roundoff error, |npos.x| may be > 1.
	 */
	if (npos.x > 1.)
		uv->u = 0.;
	else if (npos.x < -1.)
		uv->u = 0.5;
	else
		uv->u = acos(npos.x) / TWOPI;
	if (npos.y < 0.)
		uv->u = 1. - uv->u;

	if (dpdu) {
		dpdv->x = dpdv->y = 0.;
		dpdv->z = 1.;
		dpdu->x = -npos.y;
		dpdu->y = npos.x;
		dpdu->z = 0.;
		VecTransform(dpdu, &cyl->trans.trans);
		VecTransform(dpdv, &cyl->trans.trans);
		(void)VecNormalize(dpdu);
		(void)VecNormalize(dpdv);
	}
}

void
CylinderBounds(cyl, bounds)
Cylinder *cyl;
Float bounds[2][3];
{
	bounds[LOW][X] = bounds[LOW][Y] = -1;
	bounds[HIGH][X] = bounds[HIGH][Y] = 1;
	bounds[LOW][Z] = 0.;
	bounds[HIGH][Z] = 1;
	/*
	 * Transform bounding box to world space.
	 */
	BoundsTransform(&cyl->trans.trans, bounds);
}

char *
CylinderName()
{
	return cylName;
}

void
CylinderStats(tests, hits)
unsigned long *tests, *hits;
{
	*tests = CylTests;
	*hits = CylHits;
}

void
CylinderMethodRegister(meth)
UserMethodType meth;
{
	if (iCylinderMethods)
		iCylinderMethods->user = meth;
}
