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
#include "libsurf/surface.h"
#include "light.h"

/*
 * Shadow stats.
 * External functions have read access via ShadowStats().
 */
static unsigned long	ShadowRays, ShadowHits, CacheMisses, CacheHits;
/*
 * Options controlling how shadowing information is determined.
 * Set by external modules via ShadowSetOptions().
 */
static long		ShadowOptions;

void LightCacheHit();

/*
 * Trace ray from point of intersection to a light.  If an intersection
 * occurs at a distance less than "dist" (the distance to the
 * light source), then the point is in shadow, and TRUE is returned.
 * Otherwise, the brightness/color of the light is computed ('result'),
 * and FALSE is returned.
 */
int
Shadowed(result, color, cache, ray, dist, noshadow)
Color *result, *color;	/* resultant intensity, light color */
ShadowCache *cache;	/* shadow cache for light */
Ray *ray;		/* ray, origin on surface, dir towards light */
Float dist;		/* distance from pos to light source */
int noshadow;		/* If TRUE, no shadow ray is cast. */
{
	int i, smooth, enter;
	HitList hitlist;
	Ray tmpray;
	ShadowCache *cp;
	Vector hitpos, norm, gnorm;
	Surface surf, *sptr, *prevsurf;
	Float s, totaldist, statten;
	Color res;

	if (noshadow || NOSHADOWS(ShadowOptions)) {
		*result = *color;
		return FALSE;
	}

	ShadowRays++;
	s = dist;
	cp = &cache[ray->depth];
	/*
	 * Check shadow cache.  SHADOWCACHE() is implied.
	 */
	if (cp->obj) {
		/*
		 * Transform ray to the space of the cached primitive.
		 */
		tmpray = *ray;
		if (cp->dotrans)
			s *= RayTransform(&tmpray, &cp->trans);
		/*
		 * s = distance to light source in 'primitive space'.
		 * Intersect ray with cached object.
		 */
		if (cp->obj->animtrans) {
			/*
			 * Geom has animated transformation --
			 * call intersect so that the transformation
			 * is resolved properly.
			 */
			if (intersect(cp->obj, &tmpray, &hitlist,
			    SHADOW_EPSILON, &s)) {
				CacheHits++;
				return TRUE;
			}
		} else if (IsAggregate(cp->obj)) {
			if ((*cp->obj->methods->intersect)(cp->obj->obj,
				&tmpray, &hitlist, SHADOW_EPSILON, &s)) {
				CacheHits++;
				return TRUE;
			}
		} else if ((*cp->obj->methods->intersect)(cp->obj->obj,
					&tmpray, SHADOW_EPSILON, &s)) {
			/* Hit cached object. */
			CacheHits++;
			return TRUE;
		}
		/*
		 * Did not hit anything -- zero out the cache.
		 */
		CacheMisses++;
		/*
		 * Transformed -- reset s for use below.
		 */
		s = dist;
		cp->obj = (Geom *)NULL;
		cp->dotrans = FALSE;
	}

	hitlist.nodes = 0;
	if (!TraceRay(ray, &hitlist, SHADOW_EPSILON, &s)) {
		/* Shadow ray didn't hit anything. */
		*result = *color;
		return FALSE;
	}

	/*
	 * Otherwise, we've hit something.
	 */
	ShadowHits++;

	/*
	 * If we're not worrying about transparent objects...
	 * This is ugly due to the fact that we have to find
	 * the surface associated with the object that was hit.
	 * GetShadingSurf() will always return a non-null value.
	 *
	 * ***NOTE**
	 * The transparency of the surface is checked below without
	 * applying textures, if any, to it.  This means that if
	 * an object may be made trasparent by a texture, its
	 * surface should have non-zero transparency *before* texturing
	 * as well.
	 */
	if (!SHADOWTRANSP(ShadowOptions)) {
		if (SHADOWCACHE(ShadowOptions))
			LightCacheHit(&hitlist, cp);
		return TRUE;
	}

	/*
	 * We've hit a transparent object.  Attenuate the color of the light
	 * source and continue the ray until we hit background or a
	 * non-transparent object.  Note that this is incorrect if DefIndex or
	 * any of the indices of refraction of the surfaces differ.
	 */

	totaldist = 0.;
	prevsurf = (Surface *)NULL;
	res = *color;

	do {
		/*
		 * Get a pointer to the surface to be used
		 * for shading...
		 */
		sptr = GetShadingSurf(&hitlist);
		if (sptr->transp < EPSILON) {
			if (SHADOWCACHE(ShadowOptions))
				LightCacheHit(&hitlist, cp);
			return TRUE;
		}
		/*
		 * Take specular transmission attenuation from
		 * previous intersection into account.
		 */
		if (prevsurf) {
			if (prevsurf->statten != 1.) {
				statten = pow(prevsurf->statten, s - totaldist);
				ColorScale(statten, res, &res);
			}
		}
		/*
		 * Perform texturing and the like in case surface
		 * transparency is modulated.
		 */
		/* copy the surface to be used... */
		surf = *sptr;
		enter = ComputeSurfProps(&hitlist, ray, &hitpos,
			&norm, &gnorm, &surf, &smooth);
		if (enter)
			prevsurf = &surf;
		else
			prevsurf = (Surface *)NULL;
		/*
		 * Attenuate light source by body color of surface.
		 */
		ColorScale(surf.transp, res, &res);
		ColorMultiply(res, surf.body, &res);
		/*
		 * Return if attenuation becomes large.
		 * In this case, the light was attenuated to nothing,
		 * so we can't cache anything...
		 */
		if (res.r < EPSILON && res.g < EPSILON && res.b < EPSILON)
			return TRUE;
		/*
		 * Min distance is previous max.
		 */
		totaldist = s + EPSILON;
		/*
		 * Max distance is dist to light source
		 */
		s = dist;
		/*
		 * Trace ray starting at new origin and in the
		 * same direction.
		 */
		hitlist.nodes = 0;
	} while (TraceRay(ray, &hitlist, totaldist, &s));

	*result = res;
	return FALSE;
}

void
ShadowStats(shadowrays, shadowhit, cachehit, cachemiss)
unsigned long *shadowrays, *shadowhit, *cachehit, *cachemiss;
{
	*shadowrays = ShadowRays;
	*shadowhit = ShadowHits;
	*cachehit = CacheHits;
	*cachemiss = CacheMisses;
}

void
ShadowSetOptions(options)
long options;
{
	ShadowOptions = options;
}

void
LightCacheHit(hitlist, cache)
HitList *hitlist;
ShadowCache *cache;
{
	HitNode *np;
	int i, n;
	extern long ShadowOptions;

	i = 0;

	if (SHADOWCSG(ShadowOptions)) {
		/*
		 * There's possibly a CSG object in the
		 * hitlist, so we can't simply cache the
		 * primitive that was hit.  Find the
		 * object lowest in hit that's not part
		 * of a CSG object, and cache it.
		 */
		i = FirstCSGGeom(hitlist);
	}

	if (SHADOWBLUR(ShadowOptions)) {
		/*
		 * Something might be animated --
		 * gotta cache the puppy.
		 */
		n = FirstAnimatedGeom(hitlist);
		if (n > i)
			i = n;
	}

	/*
	 * Compute total world-->cached object
	 * transformation and store in cache->trans.
	 */
	/*
	 * Find the first transformation...
	 */
	np = &hitlist->data[i];
	cache->obj = np->obj;
	/*
	 * If the cached object is animated, then we don't
	 * want to include the object's transformation(s)
	 * in cache->trans (it's taken care of in shadowed()
	 * by calling intersect).
	 */
	if (cache->obj->animtrans) {
		i++;
		np++;
	}
	cache->dotrans = FALSE;
	while (i < hitlist->nodes -1) {
		if (np->obj->trans) {
			if (cache->dotrans) {
				MatrixMult(
				    &np->obj->trans->itrans,
				    &cache->trans,
				    &cache->trans);
			} else {
				MatrixCopy(
				    &np->obj->trans->itrans,
				    &cache->trans);
				cache->dotrans = TRUE;
			}
		}
		i++;
		np++;
	}
}
