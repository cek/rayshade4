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
#ifndef XFORM_H
#define XFORM_H

#define TransXformCreate()	TransCreate((TransRef)XformCreate(), XformMethods())

#define TransXformSetX0(t, v)	TransAssoc(t, &t->trans.matrix[0][0], v)
#define TransXformSetY0(t, v)	TransAssoc(t, &t->trans.matrix[0][1], v)
#define TransXformSetZ0(t, v)	TransAssoc(t, &t->trans.matrix[0][2], v)
#define TransXformSetX1(t, v)	TransAssoc(t, &t->trans.matrix[1][0], v)
#define TransXformSetY1(t, v)	TransAssoc(t, &t->trans.matrix[1][1], v)
#define TransXformSetZ1(t, v)	TransAssoc(t, &t->trans.matrix[1][2], v)
#define TransXformSetX2(t, v)	TransAssoc(t, &t->trans.matrix[2][0], v)
#define TransXformSetY2(t, v)	TransAssoc(t, &t->trans.matrix[2][1], v)
#define TransXformSetZ2(t, v)	TransAssoc(t, &t->trans.matrix[2][2], v)
#define TransXformSetXt(t, v)	TransAssoc(t, &t->trans.translate.x, v)
#define TransXformSetYt(t, v)	TransAssoc(t, &t->trans.translate.y, v)
#define TransXformSetZt(t, v)	TransAssoc(t, &t->trans.translate.z, v)

typedef char Xform;	/* Dummy; Xform has no private data. */

extern Xform *XformCreate();
extern TransMethods *XformMethods();
extern void XformPropagate();

#endif /* XFORM_H */
