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
#ifndef COLOR_H
#define COLOR_H
/*
 * Color
 */
typedef struct Color {
	Float r, g, b;			/* Red, green, blue. */
} Color;

#define ColorScale(s,c,a)		(a)->r = (s)*(c).r, \
					(a)->g = (s)*(c).g, \
					(a)->b = (s)*(c).b

#define ColorAddScaled(x, s, y, c)	(c)->r = (x).r + (s)*(y).r, \
					(c)->g = (x).g + (s)*(y).g, \
					(c)->b = (x).b + (s)*(y).b

#define ColorMultiply(x,y,a)		(a)->r = (x).r*(y).r, \
					(a)->g = (x).g*(y).g, \
					(a)->b = (x).b*(y).b

#define ColorAdd(x,y,a)			(a)->r = (x).r+(y).r, \
					(a)->g = (x).g+(y).g, \
					(a)->b = (x).b+(y).b
#endif /* COLOR_H */
