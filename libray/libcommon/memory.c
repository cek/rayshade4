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
#ifdef SYSV
#include <memory.h>
#endif
#include "common.h"

unsigned long TotalAllocated;

voidstar
Malloc(bytes)
unsigned bytes;
{
	voidstar res;

	TotalAllocated += bytes;

	res = (voidstar)malloc(bytes);
	if (res == (voidstar)NULL)
		RLerror(RL_PANIC,
			"Out of memory trying to allocate %d bytes.\n",bytes);
	return res;
}

voidstar
Calloc(nelem, elen)
unsigned nelem, elen;
{
	voidstar res;

	res = Malloc(nelem*elen);
	bzero(res, (int)nelem*elen);
	return res;
}

void
PrintMemoryStats(fp)
FILE *fp;
{
	fprintf(fp,"Total memory allocated:\t\t%lu bytes\n",
			TotalAllocated);
}

/*
 * Allocate space for a string, copy string into space.
 */
char *
strsave(s)
char *s;
{
	char *tmp;

	if (s == (char *)NULL)
		return (char *)NULL;

	tmp = (char *)Malloc((unsigned)strlen(s) + 1);
	(void)strcpy(tmp, s);
	return tmp;
}

#ifdef MULTIMAX

char *
share_calloc(num, siz)
int num;
unsigned int siz;
{
	char *res;

	res = share_malloc(num*siz);
	bzero(res, num*siz);
	return res;
}
#endif
