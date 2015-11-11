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
#ifndef COMMON_H 
#define COMMON_H

#include <stdio.h>
#include <math.h>
#ifdef I_STDLIB
#include <stdlib.h>
#endif
#include "config.h"

typedef double Float;

#if (VOIDFLAGS & 8) == 8
typedef void * voidstar;
#else
typedef char * voidstar;
#endif

#include "expr.h"
#include "vector.h"
#include "ray.h"
#include "color.h"
#include "transform.h"
#include "error.h"

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

/*
 * Various useful constants and macros.
 */

/*
 * Minimum vector length & fp value.
 * Modify depending upon Float typedef.
 */
#define EPSILON		(Float)0.00001

#ifndef PI
#define	PI		3.14159265358979323846
#endif
#define TWOPI		(2. * PI)
#define INV_TWOPI	(1. / TWOPI)
#define deg2rad(x)	(Float)(x * PI/180.)
#define LNHALF		(-.69314718)

#ifndef NULL
#	define NULL 0
#endif

#define UNSET		-1

/*
 * Some systems, such as the RS6000, have fast fabs already defined.
 */
#ifndef fabs
extern Float RSabstmp;
#define fabs(x) 		((RSabstmp=x) < 0 ? -RSabstmp : RSabstmp)
#endif

#ifdef MULTIMAX
/*
 * On the multimax, allocate large pieces of memory as shared memory.
 */
extern char *share_malloc(), *share_calloc();
#else
/*
 * Otherwise, malloc is malloc, etc.
 */
#define share_malloc(x)		Malloc(x)
#define share_calloc(x,y)	Calloc(x,y)
#endif

/*
 * Close enough for us.
 */
#define equal(a, b)		(fabs((a) - (b)) < EPSILON)
/*
 * Maximum/Minimum functions
 */
#define max(a, b)		((a) > (b) ? (a) : (b))
#define min(a, b)		((a) < (b) ? (a) : (b))

extern voidstar Malloc(), Calloc();
extern char	*strsave();
extern double	drand48();	/* just in case */

#endif /* COMMON_H */
