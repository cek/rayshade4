/*
 * Copyright (C) 1989-2015, Mark Podlipec, Craig E. Kolb
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
#ifndef BLOB_H
#define BLOB_H

#define GeomBlobCreate(t,m,n) GeomCreate((GeomRef)BlobCreate(t,m,n), BlobMethods())

#define R_START 1
#define R_END   0

/*
 * Blob
 */
typedef struct {
	Float rs;	/* radius */
	Float c4;	/* coeefficient */
	Float c2;	/* coeefficient */
	Float c0;	/* coeefficient */
	Float x;	/* x position */
	Float y;	/* y position */
	Float z;	/* z position */
} MetaVector;

typedef struct {
	int type,pnt;
	Float bound;
	Float c[5];
} MetaInt;

typedef struct {
	Float T;		/* Threshold   */
        int num;		/* number of points */
        MetaVector *list;	/* list of points */
	MetaInt *ilist, **iarr;
        
} Blob;

typedef struct MetaList {
	MetaVector mvec;
	struct MetaList *next;
} MetaList;

extern int	BlobIntersect(), BlobNormal();
extern void	BlobBounds(), BlobStats();
extern Blob	*BlobCreate();
extern char	*BlobName();
extern Methods	*BlobMethods();

#endif /* BLOB_H */
