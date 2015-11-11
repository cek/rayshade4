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
#include "libobj/geom.h"
#include "mapping.h"

void UVMapping(), SphereMapping(), CylinderMapping(), LinearMapping();

Mapping *
UVMappingCreate()
{
	Mapping *res;

	res = (Mapping *)Malloc(sizeof(Mapping));
	res->flags = PRIMSPACE;
	res->method = UVMapping;
	return res;
}

Mapping *
SphereMappingCreate(center, norm, uaxis)
Vector *center, *norm, *uaxis;
{
	Mapping *res;

	res = (Mapping *)Malloc(sizeof(Mapping));
	res->flags = OBJSPACE;
	res->method = SphereMapping;
	if (center)
		res->center = *center;
	else
		res->center.x = res->center.y = res->center.z = 0.;
	if (norm && uaxis) {
		res->norm = *norm;
		if (VecNormalize(&res->norm) == 0.) {
			RLerror(RL_ABORT, "Degenerate mapping vector.\n");
			return (Mapping *)NULL;
		}
		if (VecNormCross(norm, uaxis, &res->vaxis) == 0.) {
			RLerror(RL_ABORT, "Degenerate mapping vector.\n");
			return (Mapping *)NULL;
		}
		(void)VecNormCross(&res->vaxis, norm, &res->uaxis);
	} else {
		res->norm.x = res->norm.y = res->uaxis.y = res->uaxis.z =
			res->vaxis.x = res->vaxis.z = 0.;
		res->norm.z = res->uaxis.x = res->vaxis.y = 1.;
	}
	return res;
}

Mapping *
CylMappingCreate(center, norm, uaxis)
Vector *center, *norm, *uaxis;
{
	Mapping *res;

	res = (Mapping *)Malloc(sizeof(Mapping));
	res->flags = OBJSPACE;
	res->method = CylinderMapping;
	if (center)
		res->center = *center;
	else
		res->center.x = res->center.y = res->center.z = 0.;
	if (norm && uaxis) {
		res->norm = *norm;
		if (VecNormalize(&res->norm) == 0.) {
			RLerror(RL_ABORT, "Degenerate mapping vector.\n");
			return (Mapping *)NULL;
		}
		/*
		 * Here, uaxis indicates where theta (u) = 0.
		 */
		if (VecNormCross(norm, uaxis, &res->vaxis) == 0.) {
			RLerror(RL_ABORT, "Degenerate mapping vector.\n");
			return (Mapping *)NULL;
		}
		(void)VecNormCross(&res->vaxis, norm, &res->uaxis);
	} else {
		res->norm.x = res->norm.y = res->uaxis.y = res->uaxis.z =
			res->vaxis.x = res->vaxis.z = 0.;
		res->norm.z = res->uaxis.x = res->vaxis.y = 1.;
	}
	return res;
}

Mapping *
LinearMappingCreate(center, vaxis, uaxis)
Vector *center, *vaxis, *uaxis;
{
	Mapping *res;
	RSMatrix m;
	Vector n;

	res = (Mapping *)Malloc(sizeof(Mapping));
	res->flags = OBJSPACE;
	res->method= LinearMapping;

	if (center)
		res->center = *center;
	else
		res->center.x = res->center.y = res->center.z = 0.;

	if (uaxis && vaxis) {
		VecCross(uaxis, vaxis, &n);
		/* this is wrong, since uaxis and vaxis
		 * give U and V in world space, and we
		 * need the inverse.
		 */
		ArbitraryMatrix(
			uaxis->x, uaxis->y, uaxis->z,
			vaxis->x, vaxis->y, vaxis->z,
			n.x, n.y, n.z,
			res->center.x, res->center.y, res->center.z,
			&m);
		MatrixInvert(&m, &res->m);
		res->uaxis = *uaxis;
		res->vaxis = *vaxis;
		VecNormalize(&res->uaxis);
		VecNormalize(&res->vaxis);
	} else {
		VecScale(-1., res->center, &n);
		TranslationMatrix(n.x, n.y, n.z, &res->m);
		res->uaxis.x = res->vaxis.y = 1.;
		res->uaxis.y = res->uaxis.z = res->vaxis.x =
			res->vaxis.z = 0.;
	}
	return res;
}

void
UVMapping(map, obj, pos, norm, uv, dpdu, dpdv)
Mapping *map;
Geom *obj;
Vec2d *uv;
Vector *pos, *norm, *dpdu, *dpdv;
{
	PrimUV(obj, pos, norm, uv, dpdu, dpdv);
}

void
SphereMapping(map, obj, pos, norm, uv, dpdu, dpdv)
Mapping *map;
Geom *obj;
Vec2d *uv;
Vector *pos, *norm, *dpdu, *dpdv;
{
	Vector vtmp;
	Float nx, ny, nz, phi, theta;

	VecSub(*pos, map->center, &vtmp);
	if (VecNormalize(&vtmp) == 0.) {
		/*
		 * Point is coincident with origin of sphere.  Punt.
		 */
		uv->u = uv->v = 0.;
		return;
	}

	/*
	 * Find location of point projected onto unit sphere
	 * in the sphere's coordinate system.
	 */
	nx = dotp(&map->uaxis, &vtmp);
	ny = dotp(&map->vaxis, &vtmp);
	nz = dotp(&map->norm, &vtmp);

	if (nz > 1.)	/* roundoff */
		phi = PI;
	else if (nz < -1.)
		phi = 0;
	else
		phi = acos(-nz);

	uv->v = phi / PI;

	if (fabs(uv->v) < EPSILON || equal(uv->v, 1.))
		uv->u = 0.;
	else {
		theta = nx / sin(phi);
		if (theta > 1.)
			theta = 0.;
		else if (theta < -1.)
			theta = 0.5;
		else
			theta = acos(theta) / TWOPI;

		if (ny > 0)
			uv->u = theta;
		else
			uv->u = 1 - theta;
	}
}

void
CylinderMapping(map, obj, pos, norm, uv, dpdu, dpdv)
Mapping *map;
Geom *obj;
Vec2d *uv;
Vector *pos, *norm, *dpdu, *dpdv;
{
	Vector vtmp;
	Float nx, ny, r;

	VecSub(*pos, map->center, &vtmp);
	nx = dotp(&map->uaxis, &vtmp);
	ny = dotp(&map->vaxis, &vtmp);
	uv->v = dotp(&map->norm, &vtmp);

	r = sqrt(nx*nx + ny*ny);

	if (r < EPSILON) {
		uv->u = 0.;
		return;
	}

	nx /= r;
	ny /= r;

	if (fabs(nx) > 1.)
		uv->u = 0.5;
	else
		uv->u = acos(nx) / TWOPI;
	if (ny < 0.)
		uv->u = 1. - uv->u;

	if (dpdv)
		*dpdv = map->norm;
	if (dpdu)
		(void)VecNormCross(&map->norm, pos, dpdu);
}

void
LinearMapping(map, obj, pos, norm, uv, dpdu, dpdv)
Mapping *map;
Geom *obj;
Vec2d *uv;
Vector *pos, *norm, *dpdu, *dpdv;
{
	Vector vtmp;

	vtmp = *pos;
	VecTransform(&vtmp, &map->m);
	uv->u = vtmp.x; uv->v = vtmp.y;

	if (dpdu) {
		*dpdu = map->uaxis;
	}
	if (dpdv) {
		*dpdv = map->vaxis;
	}
}
