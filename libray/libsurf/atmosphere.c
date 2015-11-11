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
#include "atmosphere.h"

Atmosphere *
AtmosCreate(data, method)
char *data;
void (*method)();
{
	Atmosphere *ef;

	ef = (Atmosphere *)Malloc(sizeof(Atmosphere));
	ef->data = data;
	ef->method = method;
	ef->next = (Atmosphere *)0;
	return ef;
}

Atmosphere *
AtmosphereCopy(atmos)
Atmosphere *atmos;
{
	Atmosphere *res;

	if (atmos == (Atmosphere *)NULL)
		return (Atmosphere *)NULL;
	res = AtmosCreate(atmos->data, atmos->method);
	res->next = AtmosphereCopy(atmos->next);
	return res;
}

Medium *
MediumPush(index, statten, media)
Float index, statten;
Medium *media;
{
	Medium *new;

	new = (Medium *)Malloc(sizeof(Medium));
	new->index = index;
	new->statten = statten;
	new->next = media;

	return new;
}

void
Atmospherics(effects, ray, dist, pos, color)
Atmosphere *effects;
Ray *ray;
Float dist;
Vector *pos;
Color *color;
{
	Atmosphere *etmp;

	for (etmp = effects; etmp; etmp = etmp->next)
		(*etmp->method)(etmp->data, ray, pos, dist, color);
}

Float
ExpAtten(dist, trans)
Float dist, trans;
{
	Float atten;

	if (trans < EPSILON)
		return 0.;
	atten = LNHALF * dist / trans;
	return (atten < -10. ? 0. : exp(atten));
}
