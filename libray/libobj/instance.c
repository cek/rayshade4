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
#include "instance.h"

static Methods *iInstanceMethods = NULL;
static char instanceName[] = "instance";

Instance *
InstanceCreate(obj)
Geom *obj;
{
	Instance *inst;

	if (obj == (Geom *)NULL) {
		RLerror(RL_WARN, "Instance of NULL?\n");
		return (Instance *)NULL;
	}
	inst = (Instance *)share_malloc(sizeof(Instance));
	inst->obj = obj;
	BoundsCopy(obj->bounds, inst->bounds);
	return inst;
}

char *
InstanceName()
{
	return instanceName;
}


/*
 * Intersect ray & an instance by calling intersect.
 */
int
InstanceIntersect(inst, ray, hitlist, mindist, maxdist)
Instance *inst;
Ray *ray;
HitList *hitlist;
Float mindist, *maxdist;
{
	return intersect(inst->obj, ray, hitlist, mindist, maxdist);
}

Methods *
InstanceMethods()
{
	/*
	 * Instances are special in that there is no
	 * "convert" method -- when created, they are passed
	 * a pointer to the object being instantiated.
	 * This means that you will need to set an instance's
	 * 'prims' field by hand (e.g., inst->prims = object->prims).
	 */
	if (iInstanceMethods == (Methods *)NULL) {
		iInstanceMethods = MethodsCreate();
		iInstanceMethods->methods = InstanceMethods;
		iInstanceMethods->create = (GeomCreateFunc *)InstanceCreate;
		iInstanceMethods->name = InstanceName;
		iInstanceMethods->intersect = InstanceIntersect;
		iInstanceMethods->bounds = InstanceBounds;
		iInstanceMethods->convert = (voidstar)NULL;
		iInstanceMethods->checkbounds = FALSE;
		iInstanceMethods->closed = TRUE;
	}
	return iInstanceMethods;
}

void
InstanceBounds(inst, bounds)
Instance *inst;
Float bounds[2][3];
{
	GeomComputeBounds(inst->obj);
	BoundsCopy(inst->obj->bounds, inst->bounds);
	BoundsCopy(inst->bounds, bounds);
}

void
InstanceMethodRegister(meth)
UserMethodType meth;
{
	if (iInstanceMethods)
		iInstanceMethods->user = meth;
}
