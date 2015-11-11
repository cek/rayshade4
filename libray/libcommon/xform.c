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
#include "common.h"
#include "xform.h"

TransMethods *iXformMethods;

/*
 * Create and return reference to an Xform structure.
 * In this case, we return a NULL pointer, as the Xform
 * structure does not hold any data.
 */
Xform *
XformCreate()
{
	return (Xform *)NULL;
}

/*
 * Return a pointer to collection of methods for the
 * Xform transformation.
 */
TransMethods *
XformMethods()
{
	if (iXformMethods == (TransMethods *)NULL) {
		iXformMethods = (TransMethods *)Malloc(sizeof(TransMethods));
		iXformMethods->create = (TransCreateFunc *)XformCreate;
		iXformMethods->propagate = XformPropagate;
	}
	return iXformMethods;	
}

/*
 * Given an Xform structure and forward and inverse transformations,
 * propagate the information in the Xform structure to the
 * transformations.
 * In this case, the information "in" the Xform structure is
 * actually stored in the forward transformation; the Xform
 * points to NULL.
 */
void
XformPropagate(xform, trans, itrans)
Xform *xform;
RSMatrix *trans, *itrans;
{
	/*
	 * The Xform methods change the forward trans
	 * directly, so it's already all set.
	 * Build the inverse...
	 */
	MatrixInvert(trans, itrans);
}
