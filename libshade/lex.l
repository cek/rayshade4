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
skipcomments()
{
	char c;

	while (1) {
		while (input() != '*')
			;
		if ((c = input()) == '/')
			return;
		unput(c);
	}
}
/*
 * Deal with ccp-produced lines of the form:
 * # n "filename"
 * and
 * # n
 * Where filename is the name of the file being processed, and n is
 * the current line number in that file.
 */
handlehash()
{
	char buf[BUFSIZ];
	int i;
	extern int yylineno;
	extern char yyfilename[];

	/*
	 * Read the entire line into buf.
	 */
	for (i = 0; (buf[i] = input()) != '\n'; i++)
			;
	unput(buf[i]);		/* To make sure consecutive # lines work. */
	buf[i] = (char)NULL;	/* Replace newline with NULL. */

	/*
	 * Complain if the line was not of the form #n "filename"
	 */
	if ((i = sscanf(buf,"%d \"%[^\"]s\"", &yylineno, yyfilename)) == 0 &&
	    (i = sscanf(buf," line %d \"%[^\"]s\"",&yylineno,yyfilename))==0) {
		RLerror(RL_PANIC, "Unknown '#' control (%s).\n",buf);
		exit(1);
	}
	if (i == 1 && (index(buf,'"') != (char *)NULL)) {
		/*
		 * Filename was given as ""
		 */
		(void)strcpy(yyfilename, "stdin");
	}
	yylineno--;  /* The newline we unput will increment yylineno */
}
