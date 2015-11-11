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
#ifndef HF_H
#define HF_H

#define GeomHfCreate(f) GeomCreate((GeomRef)HfCreate(f), HfMethods())

/*
 * Any height values <= Hf_UNSET is not considered to be part of the
 * height field. Any trianges containing such a vertex will not be
 * rendered.  This allows one to render non-square height fields.
 */
#define HF_UNSET		(-1000.)
/*
 * Number of datapoints in a single cell.  If you've got loads of memory,
 * decrease this number.  The 'optimal' number is quite system-dependent,
 * but something around 20 seems to work well. For systems without much
 * memory, this constant should be greater than or equal to the largest
 * height field which will be rendered, thus making the algorithm
 * non-hierarchical.
 */
#define BESTSIZE		16
/*
 * Size of triangle cache.
 */
#define CACHESIZE		6
/*
 * Used to differentiate between the two triangles used to represent a cell:
 *	a------d
 *      |\     |
 *      | \TRI2|	TRI2 == c-->d-->a-->c
 *      |  \   |
 *      |   \  |
 *	|    \ |
 *      |TRI1 \|	TRI1 == c-->a-->b-->c
 *      b------c
 */
#define TRI1			1
#define TRI2			2

typedef struct hfTri {
	Vector v1, v2, v3, norm;
	Float d;
	char type;
	struct hfTri *next, *prev;
} hfTri;

typedef struct {
	int len;
	hfTri *head, *tail;
} TriQueue;

typedef struct {
	float **data;		/* Altitude points */
	float minz, maxz;
	int size, *lsize;	/* # of points/side */
	int BestSize; 		/* "best" division size */
	float iBestSize;	/* inverse of above (for faster computation) */
	int levels;		/* log base BestSize of size */
	float ***boundsmax;	/* high data values at various resolutions. */
	float ***boundsmin;
	float *spacing;
	hfTri hittri, **q;	/* hit triangle and triangle cache */
	int qtail, qsize;	/* end and length of cache */
	Float boundbox[2][3];	/* bounding box of Hf */
} Hf;

extern Hf	*HfCreate();
extern int	HfIntersect(), HfEnter(), HfNormal();
extern void	HfBounds(), HfUV(), HfStats();
extern char	*HfName();
extern Methods	*HfMethods();

#endif /* HF_H */
