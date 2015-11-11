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
#include "options.h"
#include "stats.h"

static Geom *Objects = NULL;		/* named objects */
Geom *World;				/* top-level object */


/*
 * Return pointer to named object, NULL if no such object has been defined.
 */
Geom *
GeomGetNamed(name)
char *name;
{
	Geom *otmp;
	for (otmp = Objects; otmp; otmp = otmp->next)
		if (strcmp(name, otmp->name) == 0)
			return otmp;
	return (Geom *)NULL;
}

/*
 * Add object to list of defined objects.  At this point, the object has
 * been converted to the correct type, and obj->next is either NULL or
 * garbage.
 */
void
GeomAddToDefined(obj)
Geom *obj;
{
	obj->next = Objects;
	Objects = obj;
	if (Options.verbose)
		AggregatePrintInfo(obj, Stats.fstats);
	else
		AggregatePrintInfo(obj, (FILE *)NULL);
}

/*
 * Return a copy of the named object.
 */
Geom *
GeomCopyNamed(name)
char *name;
{
	Geom *child;

	child = GeomGetNamed(name);
	if (child == (Geom *)NULL)
		RLerror(RL_PANIC, "There is no object named \"%s\".", name);
	child = GeomCopy(child);
	return child;
}

void
WorldSetup()
{
	extern GeomList *Defstack;

	/*
	 * Define World object, if not done previously.
	 */
	if (World == (Geom *)NULL) {
		World = Defstack->obj;	/* World is topmost object on stack */
		if (Defstack->next)
			RLerror(RL_ABORT, "Geom def stack is screwey.\n");
		World->prims = AggregateConvert(World, World->next);
	}

	GeomComputeBounds(World);

	/*
	 * Complain if there are no primitives to be rendered.
	 */
	if (World->prims == 0) {
		RLerror(RL_PANIC, "Nothing to be rendered.\n");
	}
}

/*
 * Main ray-spwaning routine required by libray
 */
int
TraceRay(ray, hitlist, mindist, maxdist)
Ray *ray;
HitList *hitlist;
Float mindist, *maxdist;
{
	return intersect(World, ray, hitlist, mindist, maxdist);
}
