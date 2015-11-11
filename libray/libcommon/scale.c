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
#include "scale.h"

TransMethods *iScaleMethods;
void ScaleMatrix();

Scale *
ScaleCreate()
{
	Scale *res;

	res = (Scale *)Malloc(sizeof(Scale));
	res->x = res->y = res->z = 1.;
	return res;
}

TransMethods *
ScaleMethods()
{
	if (iScaleMethods == (TransMethods *)NULL) {
		iScaleMethods = (TransMethods *)Malloc(sizeof(TransMethods));
		iScaleMethods->create = (TransCreateFunc *)ScaleCreate;
		iScaleMethods->propagate = ScalePropagate;
	}
	return iScaleMethods;	
}

void
ScalePropagate(scale, trans, itrans)
Scale *scale;
RSMatrix *trans, *itrans;
{
	if (equal(scale->x, 0.) || equal(scale->y, 0.) || equal(scale->z, 0.))
		RLerror(RL_PANIC, "Degenerate scale %g %g %g\n", scale->x, scale->y, scale->z);
	ScaleMatrix(scale->x, scale->y, scale->z, trans);
	/*
	 * Build the inverse
	 */
	MatrixInit(itrans);
	itrans->matrix[0][0] = 1. / scale->x;
	itrans->matrix[1][1] = 1. / scale->y;
	itrans->matrix[2][2] = 1. / scale->z;
}

void
ScaleMatrix(x, y, z, mat)
Float x, y, z;
RSMatrix *mat;
{
	MatrixInit(mat);
	mat->matrix[0][0] = x;
	mat->matrix[1][1] = y;
	mat->matrix[2][2] = z;
}
