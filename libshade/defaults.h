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
#ifndef DEFAULTS_H
#define DEFAULTS_H

#define DEFSAMPLES	3	/* sqrt of number of samples */

#define XRESOLUTION	512		/* Default screen size (pixels) */
#define YRESOLUTION	512
#define DEFCUTOFF	0.002		/* Default tree cutoff value */
#define MAXDEPTH	5		/* Maximum ray tree depth. */
#define HFOV		45		/* Field-of-view */
#define EYEX		0		/* Eye position */
#define EYEY		-10
#define EYEZ		0
#define LOOKX		0		/* Look point */
#define LOOKY		0
#define LOOKZ		0
#define UPX		0		/* Up vector */
#define UPY		0
#define UPZ		1
#define GAMMA		1.		/* Default gamma */
#define GAUSSIAN	FALSE		/* Use gaussian pixel filter? */
#define FILTERWIDTH	1.8		/* Default gaussian filter width */

#if defined(LINDA) && !defined(WORKERS)
#define WORKERS		4		/* Default # of workers. */
#endif

#define REPORTFREQ	10		/* Frequency of status report */

#define DEFREDCONT	0.2		/* Default contrast threshold values. */
#define DEFGREENCONT	0.15
#define DEFBLUECONT	0.3

#endif /* DEFAULTS_H */
