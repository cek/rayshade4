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
#include <stdio.h>
#ifdef SYSV
#include <memory.h>
#endif
#include "libcommon/common.h"

extern FILE *yyin;

#define usage(v)		fprintf(stderr,"usage: %s [oldfile]\n",v)

main(argc, argv)
int argc;
char **argv;
{
	if (argc > 2) {
		usage(argv[0]);
		exit(1);
	}

	if (argc == 2) {
		yyin = fopen(argv[1], "r");
		if (yyin == (FILE *)NULL) {
			fprintf(stderr,"Cannot open %s\n",argv[1]);
			exit(2);
		}
	} else
		yyin = stdin;
	printf("/* Converted by rsconvert */\n");
	printf("#define ENDCAPS\n");
	yyparse();
}	

char *
strsave(s)
char *s;
{
	extern voidstar Malloc();
	char *r;

	r = (char *)Malloc(strlen(s) + 1);
	strcpy(r, s);
	return r;
}

voidstar
Malloc(n)
unsigned n;
{
	voidstar r;
	extern voidstar malloc();

	r = malloc(n);
	if (r == (voidstar)NULL) {
		fprintf(stderr,"Out of memory allocating %d bytes.\n");
		exit(1);
	}
	return r;
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
