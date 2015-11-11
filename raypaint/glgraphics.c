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

#include <gl.h>
#include <device.h>

#define CPACK(x)	cpack(((((x)[2] << 8) | (x)[1]) << 8) | x[0])

GraphicsInit(xsize, ysize, name)
int xsize, ysize;
char *name;
{
#ifndef _IBMR2
	foreground();
#endif
	prefsize(xsize, ysize);
	winopen(name);
	ortho2(-0.5, (float)xsize -0.5, -0.5, (float)ysize - 0.5);
	viewport(0, xsize -1, 0, ysize -1);
	color(BLACK);
	clear();
	RGBmode();
	gconfig();
	unqdevice(INPUTCHANGE);
	qdevice(LEFTMOUSE);  /* Pop a square on request */
	qdevice(MIDDLEMOUSE);
	qdevice(RIGHTMOUSE);
	qdevice(REDRAW);
}

/*
 * Draw the pixel at (xp, yp) in the color given by the rgb-triple,
 * 0 indicating 0 intensity, 255 max intensity.
 */
GraphicsDrawPixel(xp, yp, color)
int xp, yp;
unsigned char color[3];
{
	unsigned long int pix;

	pix = (((color[2] << 8) | color[1]) << 8) | color[0];
	lrectwrite(xp, yp, xp, yp, &pix);
}

/*
 * Draw the rectangle with lower left corner (xp, yp) and upper right
 * corner (xp+ys, yp+ys).  The colors of the l-l, l-r, u-r, and u-l
 * corners are given as arrays of unsigned chars as above.
 */
GraphicsDrawRectangle(xp, yp, xs, ys, ll, lr, ur, ul)
int xp, yp, xs, ys;
unsigned char ll[3], lr[3], ur[3], ul[3];
{
	int   p[2];

#if defined(_IBMR2) && !defined(SHARED_EDGES)
	/*
	 * RS6000 doesn't seem to draw lower and left edges
	 * of rectangles correctly.
	 */
	xp--; yp--;
	xs++; ys++;
#endif
	bgnpolygon();

	p[0] = xp; p[1] = yp;
	CPACK(ll);
	v2i(p);

	p[0] += xs;
	CPACK(lr);
	v2i(p);

	p[1] += ys;
	CPACK(ur);
	v2i(p);
	
	p[0] = xp;
	CPACK(ul);
	v2i(p);

	endpolygon();
}

GraphicsLeftMouseEvent()
{
	/*
	 * Return TRUE if left mouse button is down.
	 */
	return getbutton(LEFTMOUSE);
}

GraphicsMiddleMouseEvent()
{
	return getbutton(MIDDLEMOUSE);
}

GraphicsRightMouseEvent()
{
	return getbutton(RIGHTMOUSE);
}

/*
 * Return position of mouse in unnormalized screen coordinates.
 */
GraphicsGetMousePos(x, y)
int *x, *y;
{
	int xo, yo;

	getorigin(&xo, &yo);
	*x = getvaluator( MOUSEX ) - xo;
	*y = getvaluator( MOUSEY ) - yo;
}

GraphicsRedraw()
{
	Device dev;
	short val;

	while (qtest()) {
		dev = qread(&val);
		if (dev == REDRAW) {
			reshapeviewport();
			return TRUE;
		}
	}
	return FALSE;
}
