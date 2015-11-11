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
#include "geom.h"
#include "list.h"

static Methods *iListMethods = NULL;
static char listName[] = "list";

List *
ListCreate()
{
	return (List *)share_calloc(1, sizeof(List));
}

char *
ListName()
{
	return listName;
}

/*
 * Take a list whose DATA field points to a linked list of objects and
 * turn it into a List.
 */
int
ListConvert(list, objlist)
List *list;
Geom *objlist;
{
	int num;

	/*
	 * Find the unbounded objects on the list as well as the
	 * bounding box of the list.
	 */
	list->list = objlist;
	for (num = 0; objlist; objlist = objlist->next)
		num += objlist->prims;
	return num;
}

/*
 * Intersect ray & list of objects.
 */
int
ListIntersect(list, ray, hitlist, mindist, maxdist)
List *list;
Ray *ray;
HitList *hitlist;
Float mindist, *maxdist;
{
	Geom *objlist;
	Vector vtmp;
	Float s;
	int hit;

	hit = FALSE;
	/*
	 * Intersect with unbounded objects.
	 */
	for (objlist = list->unbounded; objlist ; objlist = objlist->next) {
		if (intersect(objlist, ray, hitlist, mindist, maxdist))
			hit = TRUE;
	}

	/*
	 * Check for intersection with bounding box.
	 */
	s = *maxdist;	/* So maxdist won't be reset. */
	VecAddScaled(ray->pos, mindist, ray->dir, &vtmp);
	if (OutOfBounds(&vtmp, list->bounds) &&
	    !BoundsIntersect(ray, list->bounds, mindist, &s))
		/*
		 * Ray never hit list.
		 */
		return hit;
	/*
	 * Else the ray enters list-space before it hits an
	 * unbounded object. Intersect with objects on list.
	 */
	for (objlist = list->list; objlist ; objlist = objlist->next) {
		if (intersect(objlist, ray, hitlist, mindist, maxdist))
			hit = TRUE;
	}

	return hit;
}

Methods *
ListMethods()
{
	if (iListMethods == (Methods *)NULL) {
		iListMethods = MethodsCreate();
		iListMethods->methods = ListMethods;
		iListMethods->create = (GeomCreateFunc *)ListCreate;
		iListMethods->name = ListName;
		iListMethods->intersect = ListIntersect;
		iListMethods->bounds = ListBounds;
		iListMethods->convert = ListConvert;
		iListMethods->checkbounds = FALSE;
		iListMethods->closed = TRUE;
	}
	return iListMethods;
}

void
ListBounds(list, bounds)
List *list;
Float bounds[2][3];
{
	Geom *obj, *next;

	BoundsInit(list->bounds);
	/*
	 * For each object on the list,
	 * compute its bounds...
	 */
	list->unbounded  = GeomComputeAggregateBounds(&list->list, 
				list->unbounded, list->bounds);
	BoundsCopy(list->bounds, bounds);
}

void
ListMethodRegister(meth)
UserMethodType meth;
{
	if (iListMethods)
		iListMethods->user = meth;
}
