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
#ifndef OBJECT_H
#define OBJECT_H

#include "libcommon/common.h"
#include "libcommon/transform.h"
#include "bounds.h"

/*
 * Constants for enter flag in HitNode.
 */
#define EXITING		1
#define ENTERING	2

#define MAXMODELDEPTH	32		/* Maximum height of DAG. */

typedef char * GeomRef;
typedef GeomRef GeomCreateFunc();

/*
 * If the object has a normal method, it's a primitive
 * otherwise it's an aggregate (or an instance)
 */
#define IsAggregate(o)		((o)->methods->normal == NULL)

/*
 * Geom methods.
 * (p) means applies only to primitive objects
 * (a) means applies only to aggregate objects
 */
typedef struct Methods {
	char		*(*name)();		/* Geom name */
	GeomRef		(*create)();		/* Create and return ref */
	int		(*intersect)(),		/* Ray/obj intersection */
			(*normal)(),		/* Geom normal (p) */
			(*enter)(),		/* Ray enter or exit? (p) */
			(*convert)();		/* Convert from list (a) */
	void		(*uv)(),		/* 2D mapping (p) */
			(*stats)(),		/* Statistics */
			(*bounds)(),		/* Bounding volume */
			(*user)();		/* User-defined method */
	struct Methods	*(*methods)();		/* object methods func. */
	char		checkbounds,		/* check bbox before int.? */
			closed;			/* properly closed? */
} Methods;

typedef void (*UserMethodType)();

/*
 * Geom definition
 */
typedef struct Geom {
	char *name;			/* Geom name, if any. */
	GeomRef obj;			/* Pointer to object info. */
	Methods *methods;
	unsigned long prims;		/* sum of # primitive objects */
	Float bounds[2][3];		/* Bounding box */
	Float timenow;			/* Geom's idea of what time it is */
	short int animtrans;		/* transformation is animated */
	short int frame;		/* frame for which obj is inited */
	struct Surface *surf;		/* surface, if any */
	struct Trans *trans;		/* Transformation information */
	struct Trans *transtail;	/* Double linked list end */
	struct Texture *texture;	/* Texture mapping info. */
#ifdef SHAREDMEM
	unsigned long *counter;		/* Geoms are shared, counters aren't */
#else
	unsigned long counter;		/* "mailbox" for grid intersection */
#endif
	struct Geom *next;		/* Next object. */
} Geom;

/*
 * Linked list of pointers to objects.
 */
typedef struct GeomList {
	Geom *obj;
	struct GeomList *next;
} GeomList;

/*
 * Array of hit information.  Stores a path through an object DAG,
 * as well as the ray in 'model' (object) space and the distance from
 * the ray origin to the point of intersection.
 */
typedef struct HitNode {
	Geom *obj;			/* Geom hit */
	Ray	ray;			/* Ray */
	Float	mindist;		/* Amount of ray to ignore */
	Float	dist;			/* Distance from ray origin to hit */
	short	enter,			/* Enter (TRUE) or Leave (FALSE) obj */
		dotrans;		/* transformations non-identity? */
	Trans	trans;			/* parent-->obj and inverse trans */
} HitNode;

/*
 * Structure holding a list of HitNodes.  A maximum of MAXMODELDEPTH
 * nodes can be referenced.
 */
typedef struct HitList {
	int nodes;
	HitNode data[MAXMODELDEPTH];
} HitList;

extern char	*GeomName();

extern Geom	*GeomCreate(), *GeomCopy(), *GeomCopyNamed(),
		*GeomComputeAggregateBounds();


extern GeomList	*GeomStackPush(), *GeomStackPop();

extern void 	PrimUV(), AggregatePrintInfo(),
		IntersectStats();

extern int	AggregateConvert(), PrimNormal(),
		TraceRay();	/* application-provided */

extern Methods	*MethodsCreate();

#endif /* OBJECT_H */
