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

#include "rayshade.h"
#include "libcommon/sampling.h"
#include "libsurf/atmosphere.h"
#include "viewing.h"
#include "options.h"
#include "stats.h"
#include "picture.h"

/*
 * "Dither matrices" used to encode the 'number' of a ray that samples a
 * particular portion of a pixel.  Hand-coding is ugly, but...
 */
static int		*SampleNumbers;
static int OneSample[1] = 	{0};
static int TwoSamples[4] =	{0, 2,
				 3, 1};
static int ThreeSamples[9] =	{0, 2, 7,
				 6, 5, 1,
				 3, 8, 4};
static int FourSamples[16] =	{ 0,  8,  2, 10,
				 12,  4, 14,  6,
				  3, 11,  1,  9,
				 15,  7, 13,  5};
static int FiveSamples[25] =	{ 0,  8, 23, 17,  2,
				 19, 12,  4, 20, 15,
				  3, 21, 16,  9,  6,
				 14, 10, 24,  1, 13,
				 22,  7, 18, 11,  5};
static int SixSamples[36] =	{ 6, 32,  3, 34, 35,  1,
				  7, 11, 27, 28,  8, 30,
				 24, 14, 16, 15, 23, 19,
				 13, 20, 22, 21, 17, 18,
				 25, 29, 10,  9, 26, 12,
				 36,  5, 33,  4,  2, 31};
static int SevenSamples[49] =	{22, 47, 16, 41, 10, 35,  4,
				  5, 23, 48, 17, 42, 11, 29,
				 30,  6, 24, 49, 18, 36, 12,
				 13, 31,  7, 25, 43, 19, 37,
				 38, 14, 32,  1, 26, 44, 20,
				 21, 39,  8, 33,  2, 27, 45,
				 46, 15, 40,  9, 34,  3, 28};
static int EightSamples[64] =	{ 8, 58, 59,  5,  4, 62, 63,  1,
				 49, 15, 14, 52, 53, 11, 10, 56,
				 41, 23, 22, 44, 45, 19, 18, 48,
				 32, 34, 35, 29, 28, 38, 39, 25,
				 40, 26, 27, 37, 36, 30, 31, 33,
				 17, 47, 46, 20, 21, 43, 42, 24,
				  9, 55, 54, 12, 13, 51, 50, 16,
				 64,  2,  3, 61, 60,  6,  7, 57};
#define RFAC	0.299
#define GFAC	0.587
#define BFAC	0.114

#define NOT_CLOSED 0
#define CLOSED_PARTIAL   1
#define CLOSED_SUPER 2
/*
 * If a region has area < MINAREA, it is considered to be "closed",
 * (a permanent leaf).  Regions that meet this criterion
 * are drawn pixel-by-pixel rather.
 */
#define MINAREA		9

#define SQ_AREA(s)	(((s)->xsize+1)*((s)->ysize+1))

#define PRIORITY(s)	((s)->var * SQ_AREA(s))

#define INTENSITY(p)	((RFAC*(p)[0] + GFAC*(p)[1] + BFAC*(p)[2])/255.)

#define OVERLAPS_RECT(s) (!Rectmode || \
				((s)->xpos <= Rectx1 && \
				 (s)->ypos <= Recty1 && \
				 (s)->xpos+(s)->xsize >= Rectx0 && \
				 (s)->ypos+(s)->ysize >= Recty0))

typedef unsigned char RGB[3];

static RGB **Image;
static char **SampleMap;

/*
 * Sample square
 */
typedef struct SSquare {
	short xpos, ypos, xsize, ysize;
	short depth;
	short leaf, closed;
	float mean, var, prio;
	struct SSquare *child[4], *parent;
} SSquare;

SSquare *SSquares, *SSquareCreate(), *SSquareInstall(), *SSquareSelect(),
	*SSquareFetchAtMouse();

Float SSquareComputeLeafVar();

Ray	TopRay;				/* Top-level ray. */
int	Rectmode = FALSE,
	Rectx0, Recty0, Rectx1, Recty1;
int	SuperSampleMode = 0;
#define SSCLOSED (SuperSampleMode + 1)

Render(argc, argv)
int argc;
char **argv;
{
	/*
	 * Do an adaptive trace, displaying results in a
	 * window as we go.
	 */
	SSquare *cursq;
	Pixel *pixelbuf;
	int y, x;

	/*
	 * The top-level ray TopRay always has as its origin the
	 * eye position and as its medium NULL, indicating that it
	 * is passing through a medium with index of refraction
	 * equal to DefIndex.
	 */
	TopRay.pos = Camera.pos;
	TopRay.media = (Medium *)0;
	TopRay.depth = 0;
	/*
	 * Doesn't handle motion blur yet.
	 */
	TopRay.time = Options.framestart;

	GraphicsInit(Screen.xsize, Screen.ysize, "rayview");
	/*
	 * Allocate array of samples.
	 */
	Image=(RGB **)Malloc(Screen.ysize*sizeof(RGB *));
	SampleMap = (char **)Malloc(Screen.ysize * sizeof(char *));
	for (y = 0; y < Screen.ysize; y++) {
		Image[y]=(RGB *)Calloc(Screen.xsize, sizeof(RGB));
		SampleMap[y] = (char *)Calloc(Screen.xsize, sizeof(char));
	}
	switch (Sampling.sidesamples) {
		case 1:
			SampleNumbers = OneSample;
			break;
		case 2:
			SampleNumbers = TwoSamples;
			break;
		case 3:
			SampleNumbers = ThreeSamples;
			break;
		case 4:
			SampleNumbers = FourSamples;
			break;
		case 5:
			SampleNumbers = FiveSamples;
			break;
		case 6:
			SampleNumbers = SixSamples;
			break;
		case 7:
			SampleNumbers = SevenSamples;
			break;
		case 8:
			SampleNumbers = EightSamples;
			break;
		default:
			RLerror(RL_PANIC,
				"Sorry, %d rays/pixel not supported.\n",
					Sampling.totsamples);
	}
	/*
	 * Take initial four samples
	 */
	SSquareSample(0, 0, FALSE);
	SSquareSample(Screen.xsize -1, 0, FALSE);
	SSquareSample(Screen.xsize -1, Screen.ysize -1, FALSE);
	SSquareSample(0, Screen.ysize -1, FALSE);
	SSquares = SSquareInstall(0, 0, Screen.xsize -1, Screen.ysize -1,
				  0, (SSquare *) NULL);

	for (y = 1; y <= 5; y++) {
		/*
		 * Create and draw every region at depth y.
		 */
		SSquareDivideToDepth(SSquares, y);
	}
		

	while ((cursq = SSquareSelect(SSquares)) != (SSquare *)NULL) {
		SSquareDivide(cursq);
		if (GraphicsRedraw())
			SSquareDraw(SSquares);
		if (GraphicsMiddleMouseEvent()) 
			SSetRectMode();
	}

	/*
	 * Finished the image; write to image file.
	 */
	pixelbuf = (Pixel *)Malloc(Screen.xsize * sizeof(Pixel));
	PictureStart(argv);
	for (y = 0; y < Screen.ysize; y++) {
		/*
		 * This is really disgusting.
		 */
		for (x = 0; x < Screen.xsize; x++) {
			pixelbuf[x].r = Image[y][x][0] / 255.;
			pixelbuf[x].g = Image[y][x][1] / 255.;
			pixelbuf[x].b = Image[y][x][2] / 255.;
			pixelbuf[x].alpha = SampleMap[y][x];
		}
		PictureWriteLine(pixelbuf);
	}
	PictureEnd();
	free((voidstar)pixelbuf);
}

Float
SampleTime(sampnum)
int sampnum;
{
	Float window, jitter = 0.0, res;

	if (Options.shutterspeed <= 0.)
		return Options.framestart;
	if (Options.jitter)
		jitter = nrand();
	window = Options.shutterspeed / Sampling.totsamples;
	res = Options.framestart + window * (sampnum + jitter);
	TimeSet(res);
	return res;
}

SSetRectMode()
{
	int x1,y1,x2,y2;

	if (Rectmode) {
		Rectmode = FALSE;
		RecomputePriority(SSquares);
	}
	GraphicsGetMousePos(&x1, &y1);
	while (GraphicsMiddleMouseEvent())
		;
	GraphicsGetMousePos(&x2, &y2);
	if (x1 < x2) {
		Rectx0 = (x1 < 0) ? 0 : x1;
		Rectx1 = (x2 >= Screen.xsize) ? Screen.xsize - 1 : x2;
	} else {
		Rectx0 = (x2 < 0) ? 0 : x2;
		Rectx1 = (x1 >= Screen.xsize) ? Screen.xsize - 1 : x1;
	} if (y1 < y2) {
		Recty0 = (y1 < 0) ? 0 : y1;
		Recty1 = (y2 >= Screen.ysize) ? Screen.ysize - 1 : y2;
	} else {
		Recty0 = (y2 < 0) ? 0 : y2;
		Recty1 = (y1 >= Screen.ysize) ? Screen.ysize - 1 : y1;
	}
	Rectmode = TRUE;
	/* setup current rect */
	(void)RecomputePriority(SSquares);
}

RecomputePriority(sq)
SSquare *sq;
{
	Float maxp;

	if (!OVERLAPS_RECT(sq)) {
		sq->closed = SSCLOSED;
		return FALSE;
	}

	if (sq->leaf) {
		if (SQ_AREA(sq) >= MINAREA)
			sq->closed = NOT_CLOSED;
		return TRUE;
	}
	maxp = 0.;
	if (RecomputePriority(sq->child[0]))
		maxp = max(maxp, sq->child[0]->prio);
	if (RecomputePriority(sq->child[1]))
		maxp = max(maxp, sq->child[1]->prio);
	if (RecomputePriority(sq->child[2]))
		maxp = max(maxp, sq->child[2]->prio);
	if (RecomputePriority(sq->child[3]))
		maxp = max(maxp, sq->child[3]->prio);
	sq->prio = maxp;
#if 0
	if ((sq->child[0]->closed == CLOSED_SUPER) &&
	    (sq->child[1]->closed == CLOSED_SUPER) &&
	    (sq->child[2]->closed == CLOSED_SUPER) &&
	    (sq->child[3]->closed == CLOSED_SUPER))
		sq->closed = CLOSED_SUPER;
	else if (sq->child[0]->closed && sq->child[1]->closed &&
		 sq->child[2]->closed && sq->child[3]->closed)
		sq->closed = CLOSED_PARTIAL;
	else
		sq->closed = NOT_CLOSED;
#else
	if ((sq->child[0]->closed >= SSCLOSED) &&
	    (sq->child[1]->closed >= SSCLOSED) &&
	    (sq->child[2]->closed >= SSCLOSED) &&
	    (sq->child[3]->closed >= SSCLOSED))
		sq->closed = SSCLOSED;
	else
		sq->closed = NOT_CLOSED;
#endif
	return TRUE;
}

SSquareSample(x, y, supersample)
int x, y, supersample;
{
	Float upos, vpos, u, v;
	int xx, yy, xp, sampnum, usamp, vsamp;
	Pixel ctmp;
	Pixel p;
	extern unsigned char correct();

	if (SampleMap[y][x] >= 128 + supersample)
		/* already a sample there */
		return;
	SampleMap[y][x] = 128 + supersample;
	if (supersample) {
		p.r = p.g = p.b = p.alpha = 0;
		sampnum = 0;
		xp = x + Screen.minx;
		vpos = Screen.miny + y - 0.5*Sampling.filterwidth;
		for (yy = 0; yy < Sampling.sidesamples; yy++,
		     vpos += Sampling.filterdelta) {
			upos = xp - 0.5*Sampling.filterwidth;
			for (xx = 0; xx < Sampling.sidesamples; xx++,
			     upos += Sampling.filterdelta) {
				if (Options.jitter) {
					u = upos + nrand()*Sampling.filterdelta;
					v = vpos + nrand()*Sampling.filterdelta;
				} else {
					u = upos;
					v = vpos;
				}
				TopRay.time = SampleTime(SampleNumbers[sampnum]);
				SampleScreen(u, v, &TopRay, &ctmp,
					     SampleNumbers[sampnum]);
				p.r += ctmp.r*Sampling.filter[xx][yy];
				p.g += ctmp.g*Sampling.filter[xx][yy];
				p.b += ctmp.b*Sampling.filter[xx][yy];
				if (++sampnum == Sampling.totsamples)
					sampnum = 0;
			}
		}
	}
	else {
		sampnum = nrand() * Sampling.totsamples;
		usamp = sampnum % Sampling.sidesamples;
		vsamp = sampnum / Sampling.sidesamples;

		vpos = Screen.miny + y - 0.5*Sampling.filterwidth
			+ vsamp * Sampling.filterdelta;
		upos = x + Screen.minx - 0.5*Sampling.filterwidth +
				usamp*Sampling.filterdelta;
		if (Options.jitter) {
			vpos += nrand()*Sampling.filterdelta;
			upos += nrand()*Sampling.filterdelta;
		}
		TopRay.time = SampleTime(SampleNumbers[sampnum]);
		SampleScreen(upos, vpos, &TopRay, &p, SampleNumbers[sampnum]);
	}
	Image[y][x][0] = CORRECT(p.r);
	Image[y][x][1] = CORRECT(p.g);
	Image[y][x][2] = CORRECT(p.b);
}

SSquare *
SSquareCreate(xp, yp, xs, ys, d, parent)
int xp, yp, xs, ys, d;
SSquare *parent;
{
	SSquare *new;
	Float i1, i2, i3, i4;

	new = (SSquare *)Calloc(1, sizeof(SSquare));
	new->xpos = xp; new->ypos = yp;
	new->xsize = xs; new->ysize = ys;
	new->depth = d;
	new->parent = parent;
	i1 = INTENSITY(Image[new->ypos][new->xpos]);
	i2 = INTENSITY(Image[new->ypos+new->ysize][new->xpos]);
	i3 = INTENSITY(Image[new->ypos+new->ysize][new->xpos+new->xsize]);
	i4 = INTENSITY(Image[new->ypos][new->xpos+new->xsize]);
	new->mean = 0.25 * (i1+i2+i3+i4);
	if (SQ_AREA(new) < MINAREA) {
		new->prio = 0;
		new->closed = SSCLOSED;
	} else {
		new->var = SSquareComputeLeafVar(new, i1, i2, i3, i4);
		new->prio = PRIORITY(new);
		new->closed = NOT_CLOSED;
	}
	new->leaf = TRUE;
	return new;
}

Float
SSquareComputeLeafVar(sq, i1, i2, i3, i4)
SSquare *sq;
Float i1, i2, i3, i4;
{
	Float res, diff;

	diff = i1 - sq->mean;
	res = diff*diff;
	diff = i2 - sq->mean;
	res += diff*diff;
	diff = i3 - sq->mean;
	res += diff*diff;
	diff = i4 - sq->mean;
	return res + diff*diff;
}

SSquareDivideToDepth(sq, d)
SSquare *sq;
int d;
{
	if (sq->depth == d)
		return;
	if (sq->leaf)
		SSquareDivide(sq);
	SSquareDivideToDepth(sq->child[0], d);
	SSquareDivideToDepth(sq->child[1], d);
	SSquareDivideToDepth(sq->child[2], d);
	SSquareDivideToDepth(sq->child[3], d);
}

SSquareDivide(sq)
SSquare *sq;
{
	int lowx, lowy, midx, midy, hix, hiy;
	int newxsize, newysize, ndepth, supersample;
	/*
	 * Divide the square into fourths by tracing 12
	 * new samples if necessary.
	 */
	newxsize = sq->xsize / 2;
	newysize = sq->ysize / 2;
	lowx = sq->xpos; lowy = sq->ypos;
	midx = sq->xpos + newxsize;
	midy = sq->ypos + newysize;
	hix  = sq->xpos + sq->xsize;
	hiy  = sq->ypos + sq->ysize;
	ndepth = sq->depth + 1;
	/* create new samples */
	supersample = FALSE;
	SSquareSample(midx, lowy, supersample);
	SSquareSample(lowx, midy, supersample);
	SSquareSample(midx, midy, supersample);
	SSquareSample(hix,  midy, supersample);
	SSquareSample(midx, hiy, supersample);
#ifdef SHARED_EDGES
	/* create and draw new squares */
	sq->child[0] = SSquareInstall(lowx,lowy,newxsize,newysize,ndepth,sq);
	sq->child[1] = SSquareInstall(midx, lowy, sq->xsize - newxsize,
			newysize, ndepth,sq);
	sq->child[2] = SSquareInstall(lowx, midy, newxsize,
			sq->ysize - newysize, ndepth,sq);
	sq->child[3] = SSquareInstall(midx, midy, sq->xsize - newxsize,
			 sq->ysize - newysize, ndepth,sq);
#else
	/*
	 *  draw additional samples in order to subdivide such that
	 * edges of regions do not overlap
	 */
	SSquareSample(midx +1, lowy, supersample);
	SSquareSample(midx +1, midy, supersample);
	SSquareSample(lowx, midy +1, supersample);
	SSquareSample(midx, midy +1, supersample);
	SSquareSample(midx +1, midy +1, supersample);
	SSquareSample(hix, midy +1, supersample);
	SSquareSample(midx +1, hiy, supersample);

	/* create and draw new squares */
	sq->child[0] = SSquareInstall(lowx,lowy,
				newxsize,newysize,ndepth,sq);
	sq->child[1] = SSquareInstall(midx+1, lowy, sq->xsize - newxsize -1,
			newysize, ndepth,sq);
	sq->child[2] = SSquareInstall(lowx, midy+1, newxsize,
			sq->ysize - newysize-1, ndepth,sq);
	sq->child[3] = SSquareInstall(midx+1, midy+1, sq->xsize - newxsize-1,
			 sq->ysize - newysize-1, ndepth,sq);
#endif
	sq->leaf = FALSE;
	/*
	 * Recompute parent's mean and variance.
	 */
	if (OVERLAPS_RECT(sq))
		SSquareRecomputeStats(sq);
}

SSquareRecomputeStats(sq)
SSquare *sq;
{
	Float maxp;
	int in[4], closeflag;

	in[0] = OVERLAPS_RECT(sq->child[0]);
	in[1] = OVERLAPS_RECT(sq->child[1]);
	in[2] = OVERLAPS_RECT(sq->child[2]);
	in[3] = OVERLAPS_RECT(sq->child[3]);

	if ((in[0] && (sq->child[0]->closed < SSCLOSED)) ||
	    (in[1] && (sq->child[1]->closed < SSCLOSED)) ||
	    (in[2] && (sq->child[2]->closed < SSCLOSED)) ||
	    (in[3] && (sq->child[3]->closed < SSCLOSED))) {
		maxp = 0.;
		if (in[0])
			maxp = max(maxp, sq->child[0]->prio);
		if (in[1])
			maxp = max(maxp, sq->child[1]->prio);
		if (in[2])
			maxp = max(maxp, sq->child[2]->prio);
		if (in[3])
			maxp = max(maxp, sq->child[3]->prio);
		sq->closed = NOT_CLOSED;
		sq->prio = maxp;
	} else if ((sq->child[0]->closed == CLOSED_SUPER) &&
		   (sq->child[1]->closed == CLOSED_SUPER) &&
		   (sq->child[2]->closed == CLOSED_SUPER) &&
		   (sq->child[3]->closed == CLOSED_SUPER)) {
		sq->closed = CLOSED_SUPER;
		sq->prio = 0;
#if 0
	} else if ((!in[0] || sq->child[0]->closed >= SSCLOSED) &&
		   (!in[1] || sq->child[1]->closed >= SSCLOSED) &&
		   (!in[2] || sq->child[2]->closed >= SSCLOSED) &&
		   (!in[3] || sq->child[3]->closed >= SSCLOSED)) {
		sq->closed = SSCLOSED;
		sq->prio = 0;
#endif
	} else {
		sq->closed = SSCLOSED;
		sq->prio = 0;
	}
	if (sq->parent)
		SSquareRecomputeStats(sq->parent);
}

SSquare *
SSquareInstall(xp, yp, xs, ys, d, parent)
int xp, yp, xs, ys, d;
SSquare *parent;
{
	SSquare *new;

	new = SSquareCreate(xp, yp, xs, ys, d, parent);
	SSquareDraw(new);
	return new;
}

SSquare *
SSquareSelect(list)
SSquare *list;
{
	int i;
	SSquare *res, *which;

	/*
	 * If mousebutton is pressed,
	 * find the square in which the mouse is located and
	 * return that if not a leaf (pixel-sized square).
	 */
	if (GraphicsLeftMouseEvent()) {
		SuperSampleMode = 0;
		if ((res = SSquareFetchAtMouse(list)) != (SSquare *)NULL)
			return res;
	}
	else if (GraphicsRightMouseEvent()) {
		SuperSampleMode = 1;
		if ((res = SSquareFetchAtMouse(list)) != (SSquare *)NULL) {
			return res;
		}
	}
	if (list->closed >= SSCLOSED) {
		if (Rectmode) {
			Rectmode = FALSE;
			RecomputePriority(SSquares);
			return SSquareSelect(list);
		}
		return (SSquare *)NULL;
	}
	/*
	 * Otherwise, find the square with the greatest
	 * 'priority'.
	 */
	res = list;
	while (res && !res->leaf) {
		which = (SSquare *)NULL;
		for (i = 0; i < 4; i++) {
			if ((res->child[i]->closed < SSCLOSED) &&
			    OVERLAPS_RECT(res->child[i])) {
				which = res->child[i];
				break;
			}
		}
		while (++i < 4) {
			if ((res->child[i]->closed < SSCLOSED) &&
			    which->prio < res->child[i]->prio &&
			    OVERLAPS_RECT(res->child[i]))
				which = res->child[i];
		}
		res = which;
	}
	return res;
}

SSquare *
SSquareFetchAtMouse(list)
SSquare *list;
{
	SSquare *res;
	int x, y;

	/*
	 * Get mouse position.
	 */
	GraphicsGetMousePos(&x, &y);
	res = list;
	while (!res->leaf && (res->closed < SSCLOSED)) { 
		/*
		 * Find in which child the mouse is located.
		 */
		if (x < res->child[1]->xpos) {
			if (y < res->child[2]->ypos)
				res = res->child[0];
			else
				res = res->child[2];
		} else if (y < res->child[3]->ypos)
			res = res->child[1];
		else
			res = res->child[3];
	}
	if (res->closed >= SSCLOSED)
		return (SSquare *)NULL;
	return res;
}

SSquareDraw(sq)
SSquare *sq;
{
	if (SQ_AREA(sq) >= MINAREA)
		GraphicsDrawRectangle(sq->xpos, sq->ypos, sq->xsize, sq->ysize,
			Image[sq->ypos][sq->xpos],
			Image[sq->ypos][sq->xpos+sq->xsize],
			Image[sq->ypos+sq->ysize][sq->xpos+sq->xsize],
			Image[sq->ypos+sq->ysize][sq->xpos]);
	else
		DrawPixels(sq->xpos, sq->ypos, sq->xsize, sq->ysize);
	if (!sq->leaf) {
		SSquareDraw(sq->child[0]);
		SSquareDraw(sq->child[1]);
		SSquareDraw(sq->child[2]);
		SSquareDraw(sq->child[3]);
	}
}

DrawPixels(xp, yp, xs, ys)
int xp, yp, xs, ys;
{
	int x, y, xi, yi;

	yi = yp;
	for (y = 0; y <= ys; y++, yi++) {
		xi = xp;
		for (x = 0; x <= xs; x++, xi++) {
			SSquareSample(xi, yi, SuperSampleMode);
			GraphicsDrawPixel(xi, yi, Image[yi][xi]);
		}
	}
}
