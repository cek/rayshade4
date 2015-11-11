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
#include "rotate.h"

TransMethods *iRotateMethods;
void RotationMatrix();

/*
 * Create and return reference to Rotate structure.
 */
Rotate *
RotateCreate()
{
	Rotate *res;

	res = (Rotate *)Malloc(sizeof(Rotate));
	res->x = res->y = res->theta = 0.;
	res->z = 1.;
	return res;
}

/*
 * Return a pointer to collection of methods for the
 * Rotate transformation.
 */
TransMethods *
RotateMethods()
{
	if (iRotateMethods == (TransMethods *)NULL) {
		iRotateMethods = (TransMethods *)Malloc(sizeof(TransMethods));
		iRotateMethods->create = (TransCreateFunc *)RotateCreate;
		iRotateMethods->propagate = RotatePropagate;
	}
	return iRotateMethods;	
}

/*
 * Given a Rotate structure and forward and inverse transformations,
 * propagate the information in the Rotate structure to the
 * transformations.
 */
void
RotatePropagate(rotate, trans, itrans)
Rotate *rotate;
RSMatrix *trans, *itrans;
{
	Vector axis;

	RotationMatrix(rotate->x, rotate->y, rotate->z, deg2rad(rotate->theta), trans);
	/*
	 * Build the inverse...
	 */
	MatrixInvert(trans, itrans);
}

/*
 * Initialize a rotation matrix given an axis of rotation and an
 * angle.  Right-handed rotation is applied.
 */
void
RotationMatrix(x, y, z, theta, trans)
Float x, y, z, theta;
RSMatrix *trans;
{
	Float n1, n2, n3, sintheta, costheta;
	Vector vector;

	MatrixInit(trans);
	vector.x = x;
	vector.y = y;
	vector.z = z;

	if (VecNormalize(&vector) == 0.)
		RLerror(RL_WARN, "Degenerate rotation axis.\n");

	sintheta = sin(theta);
	costheta = cos(theta);

	n1 = vector.x; n2 = vector.y; n3 = vector.z;
	trans->matrix[0][0] = (Float)(n1*n1 + (1. - n1*n1)*costheta);
	trans->matrix[0][1] = (Float)(n1*n2*(1 - costheta) + n3*sintheta);
	trans->matrix[0][2] = (Float)(n1*n3*(1 - costheta) - n2*sintheta);
	trans->matrix[1][0] = (Float)(n1*n2*(1 - costheta) - n3*sintheta);
	trans->matrix[1][1] = (Float)(n2*n2 + (1 - n2*n2)*costheta);
	trans->matrix[1][2] = (Float)(n2*n3*(1 - costheta) + n1*sintheta);
	trans->matrix[2][0] = (Float)(n1*n3*(1 - costheta) + n2*sintheta);
	trans->matrix[2][1] = (Float)(n2*n3*(1 - costheta) - n1*sintheta);
	trans->matrix[2][2] = (Float)(n3*n3 + (1 - n3*n3)*costheta);
}
