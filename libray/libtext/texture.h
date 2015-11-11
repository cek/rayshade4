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
#ifndef TEXTURE_H
#define TEXTURE_H

#include "libobj/geom.h"
#include "libsurf/surface.h"
#include "mapping.h"

/*
 * Surface properties
 */
#define COLOR		0	/* AMBIENT, DIFFUSE, SPECULAR and BODY */
#define AMBIENT		1
#define DIFFUSE		2
#define SPECULAR	3
#define BODY		4
#define REFLECT		5
#define TRANSP		6
#define SPECPOW		7
#define BUMP		8
#define INDEX		9

#define TextPointToModel(p)	PointTransform(p, &model2text.itrans)
#define TextPointToPrim(p)	PointTransform(p, &prim2text.itrans)
#define TextPointToWorld(p)	PointTransform(p, &world2text.itrans)
#define TextRayToModel(p)	RayTransform(r, &model2text.itrans)
#define TextRayToPrim(r)	RayTransform(r, &prim2text.itrans)
#define TextRayToWorld(r)	RayTransform(r, &world2text.itrans)
#define TextNormToModel(n)	NormalTransform(n, &model2text.trans)
#define TextNormToPrim(n)	NormalTransform(n, &prim2text.trans)
#define TextNormToWorld(n)	NormalTransform(n, &world2text.trans)

#define ModelPointToText(p)	PointTransform(p, &model2text.trans)
#define ModelNormToText(n)	NormalTransform(n, &model2text.itrans)
#define ModelRayToText(r)	RayTransform(r, &model2text.trans)

typedef char *TextRef;

/*
 * Texture structure
 */
typedef struct Texture {
	TextRef data;			/* Texturing info */
	void	(*method)();		/* method */
	Trans	*trans;			/* transformation info */
	short	animtrans;		/* is the transformation animated? */
	struct Texture *next;		/* next in list */
} Texture;

extern Texture	*TextCreate(), *TextAppend();
extern void	DNoise3(), VfBm(), TextApply(), MakeBump(), Wrinkled();
extern Float	Noise3(), Noise2(), Chaos(), Marble(), fBm();
extern int	TileValue();
Color		*ColormapRead();

extern Trans	model2text, prim2text, world2text;

#endif TEXTURE_H
