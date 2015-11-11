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
#include "stripe.h"

Stripe *
StripeCreate(surf, width, bump, mapping)
Surface *surf;
Float width, bump;
Mapping *mapping;
{
	Stripe *stripe;

	stripe = (Stripe *)Malloc(sizeof(Stripe));
	stripe->surf = surf;
	stripe->mapping = mapping;
	stripe->width = width;
	stripe->bump = bump;
	return stripe;
}

void
StripeApply(stripe, prim, ray, pos, norm, gnorm, surf)
Stripe *stripe;
Geom *prim;
Vector *ray, *pos, *norm, *gnorm;
Surface *surf;
{
	Vector dpdu, dpdv;
	Float fu, fv, u, v;

	TextToUV(stripe->mapping, prim, pos, gnorm, &u, &v, &dpdu, &dpdv);

	u -= floor(u);
	v -= floor(v);

	/*
	 *    s s          s
	 *   | | |        | |
	 * 1 +-+------------+
	 *   |X|\^^^^^^^^^^/| } s
	 *   |X|<+--------+>|
	 *   |X|<|        |>|
	 *   |X|<|        |>|
	 *   |X|<|        |>|
	 * v |X|<|        |>|
	 *   |X|<|        |>|
	 *   |X|<|        |>|
	 *   |X|<+--------+>|
	 *   |X|/vvvvvvvvvv\| } s
	 *   |X+------------+
	 *   |XXXXXXXXXXXXXX| } s
	 * 0 +--------------+
	 *   0              1
	 *	    u
	 *
	 * where ^ == positive fv, 0 fu, original surf.
	 *	 v == negative fv, 0 fu, original surf.
	 *	 > == positive fu, 0 fv, original surf.
	 *	 < == negative fu, 0 fv, original surf.
	 *   blank == 0 fu, 0 fv, original surf.
	 *       X == 0 fu, 0 fv, alternate surf.
	 * for stripe->bump > 0.  For stripe->bump < 0., change signs.
	 */
	 
	if (u > 2*stripe->width && v > 2*stripe->width &&
	    u <= 1. - stripe->width && v <= 1. - stripe->width)
		/* flat surface */
		return;
	else if (u < stripe->width || v < stripe->width) {
		/* on the bottom of the bump. */
		*surf = *stripe->surf;
		return;
	}

	/*
	 * Lower u & v edges are the 'flat' part of the bump --
	 * make our lives simpler below by 'removing' this area
	 * from u & v.
	 */
	u = (u - stripe->width) / (1. - stripe->width);
	v = (v - stripe->width) / (1. - stripe->width);
	/*
	 * Now the hard part -- where's the bump?
	 */
	if (v < u) { 
		if (v < 1. - u) {
			/* bottom */
			fu = 0.;
			fv = -stripe->bump;
		} else {
			/* right */
			fu = stripe->bump;
			fv = 0.;
		}
	} else {
		if (v < 1. - u) {
			/* left */
			fu = -stripe->bump;
			fv = 0.;
		} else {
			/* top */
			fu = 0.;
			fv = stripe->bump;
		}
	}

	MakeBump(norm, &dpdu, &dpdv, fu, fv);
}
