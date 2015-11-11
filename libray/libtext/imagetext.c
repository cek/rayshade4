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
#include "libimage/image.h"
#include "imagetext.h"

#define INTERP(v)	(text->lo + (v)*(text->hi - text->lo))

/*
 * Create Image texture.
 * Image texture has so many options that there's usually no
 * nice way to parse without resorting to additional keywords.
 * Thus the ImageTextCreate routine only reads the image to be used;
 * all but ->component must be set by hand.  The routine
 * ImageTextSetComponent is provided to set the component and
 * ensure that the image being used is appropriate. 
 */
ImageText *
ImageTextCreate(imagefile)
char *imagefile;
{
	ImageText *text;

	text = (ImageText *)Calloc(1, sizeof(ImageText));
	text->component = COLOR; /* texture all colors by default*/
	/*
	 * Only apply one copy of the texture by default
	 */
	text->tileu = text->tilev = 0;
	text->lo = 0.; text->hi = 1.;
	text->smooth = FALSE;
	text->mapping = UVMappingCreate();
	text->image = ImageRead(imagefile);
	return text;
}

/*
 * Set image texture to apply to given component,
 * ensuring that the image that is being used is compatible.
 */
void
ImageTextSetComponent(text, component)
ImageText *text;
int component;
{
	switch (component) {
	case COLOR: /* usual case, texture on amb, diff, spec, body */
	case AMBIENT:
	case DIFFUSE:
	case SPECULAR:
	case BODY:
		/* all of the above can use 1 or 3 channel images */
		if (text->image->chan != 1 && text->image->chan != 3) {
			RLerror(RL_ABORT,
				"Image %s must have 1 or 3 channels\n", 
				text->image->filename);
		}
		break;
	case REFLECT:
	case TRANSP:
	case SPECPOW:
	case BUMP:
	case INDEX:
		/* the above need 1 channel images */
		if (text->image->chan != 1) {
			RLerror(RL_ABORT, "Image %s must have 1 channel\n",
				text->image->filename);
		}
		break;
	}
	text->component = component;
}

void
ImageTextApply(text, prim, ray, pos, norm, gnorm, surf)
ImageText *text;
Geom *prim;
Ray *ray;
Vector *pos, *norm, *gnorm;
Surface *surf;
{
	Float fx, fy;
	Float outval[4], outval_u[4], outval_v[4];
	Float u, v;
	Surface tmpsurf;
	int ix, iy;
	int rchan, gchan, bchan;
	Vector dpdu, dpdv, ntmp;

	/*
	 * First, find the floating point location in image coords.
	 * Then set ix, iy to the integer location in image coords and
	 * use fx, fy to represent the subpixel position.
	 */
	if (text->component == BUMP)
		TextToUV(text->mapping, prim, pos, gnorm, &u, &v,
			 &dpdu, &dpdv);
	else
		TextToUV(text->mapping, prim, pos, gnorm, &u, &v, 
			 (Vector *)NULL, (Vector *)NULL);
	/*
	 * Handle tiling at this point.
	 */
	if (TileValue(text->tileu, text->tilev, u, v))
		return;
	u -= floor(u);
	v -= floor(v);
	fx = u * (Float) text->image->width;
	fy = v * (Float) text->image->height;
	ix = fx;
	iy = fy;
	fx = fx - (Float) ix;
	fy = fy - (Float) iy;

	if (text->image->has_alpha) {
		/* Alpha channel is 0 */
		rchan = 1;
		gchan = 2;
		bchan = 3;
	} else {
		rchan = 0;
		gchan = 1;
		bchan = 2;
	}

	if (text->image->chan == 1) {
		gchan = rchan;
		bchan = rchan;
	}

	ImageIndex(text->image, ix, iy, fx, fy, text->smooth, outval);

	/*
	 * escape when alpha is zero, 'cause there is no change
	 */
	if (text->image->has_alpha && (outval[0] < 0.001))
		return;

	if (text->component != COLOR || text->surf == (Surface *)NULL) {
		tmpsurf = *surf;
	} else {
		tmpsurf = *text->surf;
	}

	switch (text->component) {
		case COLOR: /* amb, diff, spec */
			tmpsurf.spec.r *= outval[rchan];
			tmpsurf.spec.g *= outval[gchan];
			tmpsurf.spec.b *= outval[bchan];
			tmpsurf.diff.r *= outval[rchan];
			tmpsurf.diff.g *= outval[gchan];
			tmpsurf.diff.b *= outval[bchan];
			tmpsurf.amb.r *= outval[rchan];
			tmpsurf.amb.g *= outval[gchan];
			tmpsurf.amb.b *= outval[bchan];
			break;
	 	case AMBIENT: /* ambient */
			tmpsurf.amb.r *= INTERP(outval[rchan]);
			tmpsurf.amb.g *= INTERP(outval[gchan]);
			tmpsurf.amb.b *= INTERP(outval[bchan]);
			break;
		case DIFFUSE: /* diffuse */
			tmpsurf.diff.r *= INTERP(outval[rchan]);
			tmpsurf.diff.g *= INTERP(outval[gchan]);
			tmpsurf.diff.b *= INTERP(outval[bchan]);
			break;
		case SPECULAR: /* specular */
			tmpsurf.spec.r *= INTERP(outval[rchan]);
			tmpsurf.spec.g *= INTERP(outval[gchan]);
			tmpsurf.spec.b *= INTERP(outval[bchan]);
			break;
		case BODY: /* transmitted */
			tmpsurf.body.r *= INTERP(outval[rchan]);
			tmpsurf.body.g *= INTERP(outval[gchan]);
			tmpsurf.body.b *= INTERP(outval[bchan]);
			break;
		case REFLECT: /* specular reflectivity */
			tmpsurf.reflect *= INTERP(outval[rchan]);
			break;
		case TRANSP: /* specular transmittance */
			tmpsurf.transp *= INTERP(outval[rchan]);
			break;
		case SPECPOW: /* specpow */
			tmpsurf.srexp *= INTERP(outval[rchan]);
			break;
		case INDEX: /* index of refraction */
			tmpsurf.index *= INTERP(outval[rchan]);
			break;
		case BUMP: /* bump map */
			ImageIndex(text->image, 
				    (ix == text->image->width - 1) ? 0 : ix+1,
				    iy, fx, fy,
				    text->smooth, outval_u);
			ImageIndex(text->image, ix, 
				    (iy == text->image->height - 1) ? 0 : iy+1,
				    fx, fy,
				    text->smooth, outval_v);
			MakeBump(norm, &dpdu, &dpdv, 
				 INTERP(outval_u[rchan] - outval[rchan]),
				 INTERP(outval_v[rchan] - outval[rchan]));
			return;
	}

	if (text->image->has_alpha && (outval[0] < 0.999)) {
		/*
		 * image partial coverage means blend surf and text->surf
		 */
		SurfaceBlend(surf, &tmpsurf, 1. - outval[0], outval[0]);
	} else {
		/*
		 * image full coverage means use text->surf
		 */
		*surf = tmpsurf;
	}
}
