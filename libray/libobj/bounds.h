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
#ifndef BOUNDS_H
#define BOUNDS_H

/*
 * Used to make indices into bounding box arrays more readable.
 */
#define X	0
#define Y	1
#define Z	2
#define LOW	0
#define HIGH	1

/*
 * If minimum X is greater than maximum, then
 * is considered to be unbounded.
 */

#define UNBOUNDED(o)	((o)->bounds[LOW][X] > (o)->bounds[HIGH][X])

/*
 * Is the point p outside of the bounding box "b"?
 */
#define OutOfBounds(p,b) ((p)->x < b[0][0] || (p)->x > b[1][0] ||\
			  (p)->y < b[0][1] || (p)->y > b[1][1] ||\
			  (p)->z < b[0][2] || (p)->z > b[1][2])

extern void 	BoundsCopy(), BoundsPrint(),
		BoundsInit(), BoundsEnlarge(),
		BoundsTransform();

extern int	BoundsIntersect();
#endif /* BOUNDS_H */
