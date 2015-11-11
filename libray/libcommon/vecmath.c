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
#include "common.h"

/*
 * Normalize a vector, return original length.
 */
Float
VecNormalize(a)
register Vector *a;
{
	Float d;

	d = sqrt(a->x*a->x + a->y*a->y + a->z*a->z);
	if (equal(d, 0.))
		return 0.;
	a->x /= d;
	a->y /= d;
	a->z /= d;
#ifdef CRAY
	/*
	 * The Cray Research Inc. math functional units don't work in the IEEE
	 * standard way, so when we get here, we just might have an x,y or z
	 * that is not in the range  -1.0 <= J <= 1.0 Yes, I know that that
	 * can't happen, but it does. So since we know we just normalized this
	 * vector, we'll just force x,y and z into the range -1.0 to 1.0 O.K?
	 */
	if (a->x >= 1.0) a->x = 1.0;
	else if (a->x <= -1.0) a->x = -1.0;
	if (a->y >= 1.0) a->y = 1.0;
	else if (a->y <= -1.0) a->y = -1.0;
	if (a->z >= 1.0) a->z = 1.0;
	else if (a->z <= -1.0) a->z = -1.0;
#endif /* CRAY */

	return d;
}

/*
 * Compute cross-product of a and b, place normalized result in o.  Returns
 * length of result before normalization.
 */
Float
VecNormCross(a, b, r)
Vector *a, *b, *r;
{
	VecCross(a, b, r);
	return VecNormalize(r);
}

/*
 * Compute cross-product of a and b, place result in o.
 */
void
VecCross(a, b, r)
Vector *a, *b, *r;
{
	r->x = (a->y * b->z) - (a->z * b->y);
	r->y = (a->z * b->x) - (a->x * b->z);
	r->z = (a->x * b->y) - (a->y * b->x);
}

/*
 * Calculate direction of refracted ray using Heckbert's formula.  Returns TRUE
 * if a total internal reflection occurs.
 */
int
Refract(dir, from_index, to_index, I, N, cos1)
Float from_index, to_index, cos1;
Vector *dir, *I, *N;
{
	double kn, cos2, k;
	Vector nrm;

	if (cos1 < 0.) {
		/*
		 * Hit the 'backside' of a surface -- flip the normal.
		 */
		nrm.x = -N->x;
		nrm.y = -N->y;
		nrm.z = -N->z;
		cos1 = -cos1;
	} else
		nrm = *N;

	kn = from_index / to_index;
	cos2 = 1. - kn*kn*(1. - cos1*cos1);
	if (cos2 < 0.)
		return TRUE;		/* Total internal reflection. */
	k = kn * cos1 - sqrt((double)cos2);
	VecComb(kn, *I, k, nrm, dir);
	return FALSE;
}

/*
 * Given a vector, find two additional vectors such that all three
 * are mutually perpendicular and uaxis X vaxis = vector.  The given
 * vector need not be normalized. uaxis and vaxis are normalized.
 */
void
VecCoordSys(vector, uaxis, vaxis)
Vector *vector, *uaxis, *vaxis;
{
	uaxis->x = -vector->y;
	uaxis->y = vector->x;
	uaxis->z = 0.;
	if (VecNormalize(uaxis) == 0.) {
		uaxis->x = vector->z;
		uaxis->y = 0.;
		uaxis->z = -vector->x;
		if (VecNormalize(uaxis) == 0.)
			RLerror(RL_WARN,
				"VecCoordSys passed degenerate vector.\n");
	}
	(void)VecNormCross(vector, uaxis, vaxis);
}

/*
 * Modify given normal by "bumping" it.
 */
void
MakeBump(norm, dpdu, dpdv, fu, fv)
Vector *norm, *dpdu, *dpdv;	/* normal, surface derivatives */
Float fu, fv;			/* bump function partial derivatives in uv */
{
	Vector tmp1, tmp2;

	VecCross(norm, dpdv, &tmp1);
	VecScale(fu, tmp1, &tmp1);
	VecCross(norm, dpdu, &tmp2);
	VecScale(fv, tmp2, &tmp2);
	VecSub(tmp1, tmp2, &tmp1);
	VecAdd(*norm, tmp1, norm);
	(void)VecNormalize(norm);
}
