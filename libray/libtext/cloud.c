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
#include "texture.h"
#include "cloud.h"

/*
 * Gardner-style textured ellipsoid.  Designed to be used on unit spheres
 * centered at the origin.  (Of course, the spheres may be transformed
 * into ellipsoids, translated, etc.)
 */
CloudText *
CloudTextCreate(scale, h, lambda, octaves, cthresh, lthresh, transcale)
Float scale, h, lambda, cthresh, lthresh, transcale;
int octaves;
{
	CloudText *cloud;
	
	cloud = (CloudText *)Malloc(sizeof(CloudText));
	cloud->beta = 1. + 2 * h;
	cloud->omega = pow(lambda, -0.5 * cloud->beta);
	cloud->lambda = lambda;
	cloud->scale = scale;
	cloud->cthresh = cthresh;
	cloud->range = lthresh - cthresh;
	cloud->transcale = transcale;
	cloud->maxval = 1. / (1. - cloud->beta);
	cloud->octaves = octaves;
	return cloud;
}

void
CloudTextApply(cloud, prim, ray, pos, norm, gnorm, surf)
CloudText *cloud;
Geom *prim;
Ray *ray;
Vector *pos, *norm, *gnorm;
Surface *surf;
{
	Ray pray;
	Float alpha, beta, It, dsquared, d, limb;

	/*
	 * Transform ray to prim. space.
	 */
	pray = *ray;
	(void)TextRayToPrim(&pray);
	dsquared = dotp(&pray.pos, &pray.pos);
	if (fabs(dsquared) < 1. + EPSILON) {
		surf->transp = 1.;
		surf->amb.r = surf->amb.g = surf->amb.b = 0.;
		surf->diff.r = surf->diff.g = surf->diff.b = 0.;
		return;
	}
	It = fBm(pos,cloud->omega,cloud->lambda,cloud->octaves);
	It = (cloud->maxval + It) * 0.5/cloud->maxval;
	if (It < 0.)
		It = 0;
	else if (It > 1.)
		It = 1;
	d = sqrt(dsquared);
	beta = sqrt(dsquared - 1) / d;
	alpha = -dotp(&pray.pos, &pray.dir) / d;
	limb = (alpha - beta) / (1 - beta);
	/*
	 * limb is 0 on the limb, 1 at the center, < 1 outside.
	 */
	surf->transp = 1. - (It-cloud->cthresh-cloud->range*(1.-limb))/
			cloud->transcale;

	if (surf->transp > 1)
		surf->transp = 1.;
	if (surf->transp < 0)
		surf->transp = 0.;

	ColorScale((1. - surf->transp) *
		   (1. - cloud->scale + cloud->scale*It),
			surf->diff, &surf->diff);
	ColorScale(1. - surf->transp, surf->amb, &surf->amb);
}
