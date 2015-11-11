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
#ifndef VIEWING_H
#define VIEWING_H

/*
 * Screen
 */
typedef struct RSScreen {
	int	xres, yres,		/* Resolution of entire screen */
		xsize, ysize,		/* Resolution of window */
		minx, miny, maxx, maxy; /* Window to be rendered */
	Vector	scrnx, scrny,		/* Horizontal & vertical screen axes */
		scrni, scrnj,		/* Normalized versions of the above */
		firstray;		/* Direction from eye to screen UL  */
	Color	background;		/* Background color */
} RSScreen;

/*
 * Camera
 */
typedef struct {
	Vector	pos,			/* Eye position */
		lookp,			/* Look position */
		dir,			/* Look direction */
		up;			/* 'Up' vector */
	Float	hfov, vfov,		/* Horizontal/vertical field of view */
		lookdist,		/* Eye pos/look pos distance */
		aperture,		/* Aperture width (0 == pinhole) */
		focaldist;		/* Distance from eye to focal plane */
} RSCamera;

extern RSScreen Screen;
extern RSCamera Camera;

#endif /* VIEWING_H */
