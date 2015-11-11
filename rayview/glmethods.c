/*
 * Copyright (C) 1989-2015, Craig E. Kolb, Allan Snyder
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
#include <gl.h>
#include <device.h>
#include "rayshade.h" 
#include "options.h"
#include "viewing.h"

#include "libsurf/surface.h"

#include "libobj/box.h"
#include "libobj/cone.h"
#include "libobj/csg.h"
#include "libobj/cylinder.h"
#include "libobj/disc.h"
#include "libobj/hf.h"
#include "libobj/grid.h"
#include "libobj/instance.h"
#include "libobj/list.h"
#include "libobj/plane.h"
#include "libobj/poly.h"
#include "libobj/sphere.h"
#include "libobj/triangle.h"

#include "liblight/light.h"
#include "liblight/extended.h"
#include "liblight/infinite.h"
#include "liblight/point.h"
#include "liblight/spot.h"

#define CIRCLE_SAMPLES	20	/* # of samples around circle (cone/cyl/etc) */
#define DEFAULT_NEAR	.2	/* default value for near clipping plane */
#define DEFAULT_FAR	350.	/* default far clipping plane */
#define PLANE_RAD	450.	/* radius of disc used to represent planes */

#ifndef SPHERELIB
#define SPHERE_LEVEL	3
#endif

/*
 * Pass a normal stored in a Vector to the geometry pipeline.
 */
#define GLNormal(w)	(glnrm[0] = (w)->x, glnrm[1] = (w)->y, \
			 glnrm[2] = (w)->z, n3f(glnrm))

static void	GLBoxDraw(), GLConeDraw(), GLCsgDraw(), GLCylinderDraw(),
		GLDiscDraw(),
		GLGridDraw(), GLHfDraw(), GLInstanceDraw(),	
		GLListDraw(), GLPlaneDraw(), GLPolygonDraw(),
		GLSphereDraw(), GLTorusDraw(), GLTriangleDraw(),
		GLBoundsDraw(),
		GLInfiniteLight(), GLPointLight(), GLSpotLight(),
		GLExtendedLight();

static short	cursurf = 1,
		curlight = 1;

static Object	GLBoxObject,
		GLCylObject,
		GLSphereObject,
		GLBoxObjectDefine(),
		GLCylObjectDefine();

extern Object	GLSphereObjectDefine();

static int	Doublebuffered;

static float	Ident[4][4] =	{1., 0., 0., 0.,
				 0., 1., 0., 0.,
				 0., 0., 1., 0.,
				 0., 0., 0., 1.},
		glnrm[3];

static float **CirclePoints;

static void LightDraw(), GeomDraw(), GLMultMatrix(),
		GLPushSurface(), GLPopSurface(), LightDrawInit(),
		ObjectInit(), GLDrawFrame(), ScreenDrawInit(), DrawInit(),
		GLUnitCircle(), GLDisc(), ComputeClippingPlanes();

Float CurrentTime;

static unsigned long BackPack;	/* Packed background color */
static long Zinit;		/* maximum Zbuffer value */

MethodsRegister()
{
	BoxMethodRegister(GLBoxDraw);
	ConeMethodRegister(GLConeDraw);
	CsgMethodRegister(GLCsgDraw);
	CylinderMethodRegister(GLCylinderDraw);
	DiscMethodRegister(GLDiscDraw);
	GridMethodRegister(GLGridDraw);
#ifdef sgi
	HfMethodRegister(GLHfDraw);
#endif
	InstanceMethodRegister(GLInstanceDraw);
	ListMethodRegister(GLListDraw);
	PlaneMethodRegister(GLPlaneDraw);
	PolygonMethodRegister(GLPolygonDraw);
	SphereMethodRegister(GLSphereDraw);
	/*TorusMethodRegister(GLTorusDraw);*/
	TriangleMethodRegister(GLTriangleDraw);

	InfiniteMethodRegister(GLInfiniteLight);
	PointMethodRegister(GLPointLight);
	ExtendedMethodRegister(GLExtendedLight);
	SpotMethodRegister(GLSpotLight);
}

void
Render()
{
	short val;
	float tmp;

	/*
	 * We're only sampling the scene once, so we need
	 * not do lots of work to determine exactly what
	 * animated transformations are doing...
	 */
	Options.samples = 1;
	SamplingSetOptions(Options.samples, Options.gaussian,
			   Options.filterwidth);

	DrawInit();
	qdevice(ESCKEY);
	qdevice(SPACEKEY);
	qdevice(REDRAW);

	DrawFrames();

	while (TRUE) {
		switch (qread(&val)) {
			case ESCKEY:
				exit(0);
				break;
			case REDRAW:
				reshapeviewport();
				DrawFrames();
				break;
			case SPACEKEY:
				if (!val)
					DrawFrames();
				break;
		}
	}
}

DrawFrames()
{
	int i;
	for (i = 0; i < Options.totalframes; i++)
		GLDrawFrame(i);
}

static void
DrawInit()
{
	extern Surface DefaultSurface;

	ScreenDrawInit();
	ObjectInit();
	LightDrawInit();

	/*
	 * Push the default surface.
	 */
	GLPushSurface(&DefaultSurface);
}

static void
ScreenDrawInit()
{
	/*
	 * Open window, initialize graphics, etc.
	 */
#ifdef sgi
	foreground();
#endif
	prefsize(Screen.xsize, Screen.ysize);
	winopen("rayview");

#ifdef sgi
	glcompat(GLC_OLDPOLYGON, 0);
#endif

	RGBmode();
	mmode(MVIEWING);

	if (Options.totalframes > 1) {
		Doublebuffered = TRUE;
		doublebuffer();
	}


	gconfig();
	blendfunction(BF_SA, BF_MSA);
	zbuffer(TRUE);

	/*
	 * Initialize viewing matrix.
	 */
	GLViewingInit();

	BackPack = (unsigned char)(255*Screen.background.r) |
		((unsigned char)(255*Screen.background.g) << 8) |
		((unsigned char)(255*Screen.background.b) << 16);
	Zinit = getgdesc(GD_ZMAX);
	lsetdepth(0, Zinit);
}

/*
 * Draw the World object.
 */
static void
GLDrawFrame(frame)
int frame;
{
	extern Geom *World;

	RSStartFrame(frame);
	CurrentTime = Options.framestart;
	TimeSet(CurrentTime);

	czclear(BackPack, Zinit);	

	/*
	 * Draw the World object
	 */
	GeomDraw(World);
	if (Doublebuffered)
		swapbuffers();
}

GLViewingInit()
{
	float near, far, aspect, dist, T[4][4];
	short ang;
	extern Geom *World;

	ang = Camera.vfov * 10 + 0.5;
	aspect = Camera.hfov / Camera.vfov;

	T[0][0]=Screen.scrni.x; T[0][1]=Screen.scrnj.x; T[0][2]= -Camera.dir.x;
	T[1][0]=Screen.scrni.y; T[1][1]=Screen.scrnj.y; T[1][2]= -Camera.dir.y;
	T[2][0]=Screen.scrni.z; T[2][1]=Screen.scrnj.z; T[2][2]= -Camera.dir.z;

	T[0][3] = T[1][3] = T[2][3] = 0.;

	T[3][0] = -dotp(&Camera.lookp, &Screen.scrni);
	T[3][1] = -dotp(&Camera.lookp, &Screen.scrnj);
	T[3][2] = dotp(&Camera.lookp, &Camera.dir);
	T[3][3] = 1.;

	ComputeClippingPlanes(&near, &far, World->bounds);

	loadmatrix(Ident);
	perspective(ang, aspect, near, far);
	polarview((float)Camera.lookdist, 0, 0, 0);
	multmatrix(T);
}


static void
ObjectInit()
{
	CircleDefine();
	GLBoxObject = GLBoxObjectDefine();

#ifdef SPHERELIB
	GLSphereObject = GLSphereObjectDefine();
#else
	GLSphereObject = GLSphereObjectDefine(SPHERE_LEVEL);
#endif

	GLCylObject = GLCylObjectDefine();
}

static void
GeomDraw(obj)
Geom *obj;
{
	Trans *ct;
	/*
	 * If object has a surface associated with it,
	 * start using it.
	 */
	if (obj->surf)
		GLPushSurface(obj->surf);
	if (obj->trans) {
		/*
		 * Take care of any animated transformations that
		 * exist.
		 */
		if (obj->animtrans && !equal(obj->timenow, CurrentTime)) {
			TransResolveAssoc(obj->trans);
			obj->timenow = CurrentTime;
		}
		pushmatrix();
		/*
	 	 * Apply in reverse order.
		 */
		for (ct = obj->transtail; ct; ct = ct->prev)
			GLMultMatrix(&ct->trans);
	}

	if (obj->methods->user) {
		/*
	 	 * Call proper method
	 	 */
		(*obj->methods->user)(obj->obj);
	} else {
		/*
		 * Draw bounding box.
		 */
		GLBoundsDraw(obj->bounds);
	}

	if (obj->surf)
		GLPopSurface();
	if (obj->trans)
		popmatrix();
}

static float surfprops[] =	{AMBIENT, 0, 0, 0,
				 DIFFUSE, 0, 0, 0,
				 SPECULAR, 0, 0, 0,
				 SHININESS, 0,
				 ALPHA, 1,
				 LMNULL};
static float	*amb = &surfprops[1],
		*diff = &surfprops[5],
		*spec = &surfprops[9],
		*shine = &surfprops[13],
		*alpha = &surfprops[15];

static void
GLPushSurface(surf)
Surface *surf;
{
	static Surface *lastsurf = NULL;

	/*
	 * Start using the given surface.
	 * In the case of the use of an "applysurf",
	 * the same surface will be applied to many
	 * primitives individually.  By saving the
	 * pointer to the last surface, we keep from
	 * lmdef'ing the surface when we don't need to.
	 */
	if (surf != lastsurf) {
		amb[0] = surf->amb.r; amb[1] = surf->amb.g;
			amb[2] = surf->amb.b;
		diff[0] = surf->diff.r; diff[1] = surf->diff.g;
			diff[2] = surf->diff.b;
		spec[0] = surf->spec.r; spec[1] = surf->spec.g;
			spec[2] = surf->spec.b;
		shine[0] = surf->srexp;
		*alpha = 1. - surf->transp;
		lmdef(DEFMATERIAL, cursurf, sizeof(surfprops)/sizeof(float),
			surfprops);
		lastsurf = surf;
	}
	lmbind(MATERIAL, cursurf);
	cursurf++;
}

static void
GLMultMatrix(trans)
RSMatrix *trans;
{
	float newmat[4][4];
	/*
	 * Multiply in the given transformation.
	 */
	newmat[0][0] = trans->matrix[0][0]; newmat[0][1] = trans->matrix[0][1];
	newmat[0][2] = trans->matrix[0][2]; newmat[0][3] = 0.;
	newmat[1][0] = trans->matrix[1][0]; newmat[1][1] = trans->matrix[1][1];
	newmat[1][2] = trans->matrix[1][2]; newmat[1][3] = 0.;
	newmat[2][0] = trans->matrix[2][0]; newmat[2][1] = trans->matrix[2][1];
	newmat[2][2] = trans->matrix[2][2]; newmat[2][3] = 0.;
	newmat[3][0] = trans->translate.x; newmat[3][1] = trans->translate.y;
	newmat[3][2] = trans->translate.z ; newmat[3][3] = 1.;
	multmatrix(newmat);
}

static void
GLPopSurface()
{
	cursurf--;
	lmbind(MATERIAL, cursurf-1);
}

static void
GLBoundsDraw(bounds)
Float bounds[2][3];
{
	float sx, sy, sz;

	pushmatrix();

	translate(bounds[LOW][X], bounds[LOW][Y], bounds[LOW][Z]);
	scale(	bounds[HIGH][X] - bounds[LOW][X],
		bounds[HIGH][Y] - bounds[LOW][Y],
		bounds[HIGH][Z] - bounds[LOW][Z]);
	pushmatrix();
	callobj(GLBoxObject);
	popmatrix();
	popmatrix();
}

static void
GLBoxDraw(box)
Box *box;
{
	GLBoundsDraw(box->bounds);
}


static void
GLConeDraw(cone)
Cone *cone;
{
	int i, j;
	float norm[3], normconst, ZeroVect[3], tmpv[3];

	ZeroVect[0] = ZeroVect[1] = ZeroVect[2] = 0.;
	/*
	 * Sides.
	 * For the normal, assume we are finding the normal at
	 * (C_P[i].x, C_P[i].y, 1.) at this point, the unnormalized
	 * normal = (C_P[i].x, C_P[i].y, -tantheta^2).  When we normalize,
	 * then length of the vector is simply equal to:
	 * sqrt(CP[i].x^2 + CP[i].y^2 + tantheta^4)  ==
	 * sqrt(1. + tantheta^4)
	 * In our case, tantheta = 1., so...
	 */

	normconst = 1. / sqrt(2.);
	norm[2] = -normconst;

	pushmatrix();
	GLMultMatrix(&cone->trans.trans);

	for (i = 0; i < CIRCLE_SAMPLES; i++) {
		j = (i + 1) % CIRCLE_SAMPLES;
		norm[0] = CirclePoints[i][0] * normconst;
		norm[1] = CirclePoints[i][1] * normconst;
		n3f(norm);
		bgnpolygon();
		if (cone->start_dist > EPSILON) {
			tmpv[2] = cone->start_dist;
			tmpv[0] = CirclePoints[i][0] * cone->start_dist;
			tmpv[1] = CirclePoints[i][1] * cone->start_dist;
			v3f(tmpv);
			tmpv[0] = CirclePoints[j][0] * cone->start_dist;
			tmpv[1] = CirclePoints[j][1] * cone->start_dist;
			v3f(tmpv);
		} else
			v3f(ZeroVect);
		tmpv[2] = 1.;
		tmpv[0] = CirclePoints[j][0];
		tmpv[1] = CirclePoints[j][1];
		v3f(tmpv);
		tmpv[0] = CirclePoints[i][0];
		tmpv[1] = CirclePoints[i][1];
		v3f(tmpv);
		endpolygon();
	}
	popmatrix();
}

static void
GLCsgDraw(csg)
Csg *csg;
{
	/*
	 * Punt by drawing both objects.
	 */
	GeomDraw(csg->obj1);
	GeomDraw(csg->obj2);
}

static void
GLCylinderDraw(cyl)
Cylinder *cyl;
{
	pushmatrix();
	GLMultMatrix(&cyl->trans.trans);
	callobj(GLCylObject);
	popmatrix();
}

static void
GLDiscDraw(disc)
Disc *disc;
{
	GLDisc((Float)sqrt(disc->radius), &disc->pos, &disc->norm);
}

static void
GLDisc(rad, pos, norm)
Float rad;
Vector *pos, *norm;
{
	RSMatrix m, tmp;
	Vector atmp;

	/*
	 * This is kinda disgusting...
	 */
	ScaleMatrix(rad, rad, 1., &m);
	if (fabs(norm->z) == 1.) {
		atmp.x = 1.;
		atmp.y = atmp.z = 0.;
	} else {
		atmp.x = norm->y;
		atmp.y = -norm->x;
		atmp.z= 0.;
	}

	RotationMatrix(atmp.x, atmp.y, atmp.z, -acos(norm->z), &tmp);
	MatrixMult(&m, &tmp, &m);
	TranslationMatrix(pos->x, pos->y, pos->z, &tmp);
	MatrixMult(&m, &tmp, &m);
	pushmatrix();
	GLMultMatrix(&m);
	/*
	 * Draw unit circle.
	 */
	GLUnitCircle(0., TRUE);
	popmatrix();
}

static void
GLGridDraw(grid)
Grid *grid;
{
	Geom *ltmp;

	for (ltmp = grid->unbounded; ltmp; ltmp = ltmp->next)
		GeomDraw(ltmp);
	for (ltmp = grid->objects; ltmp; ltmp = ltmp->next)
		GeomDraw(ltmp);
}

static void
GLHfDraw(hf)
Hf *hf;
{
	int x, y;
	float n[3], v[3], del, del2, del4, dz1, delz, za, zb, zc;
	float bot, top, left, right, len;

	del = 1. / (hf->size - 1);
	del2 = del*del;
	del4 = del2*del2;

	bot = 0.;
	top = del;

	for (y = 0; y < hf->size -1; y++) {
		za = hf->data[y+1][0];
		zb = hf->data[y][0];
		left = 0;
		right = del;
		for (x = 1; x < hf->size; x++) {
			/*
			 * A +-+ C
			 *   |/
			 * B +
			 */
		
			zc = hf->data[y+1][x];
			dz1 = za - zb;
			delz = za - zc;
			len = sqrt(del2*delz*delz + del2*dz1*dz1 + del4);
			bgnpolygon();
			n[0] = del*delz/len;
			n[1] = -del*dz1/len;
			n[2] = del2/len;
			n3f(n);
			v[0] = left; v[1] = top; v[2] = za; v3f(v);
			v[1] = bot; v[2] = zb; v3f(v);
			v[0] = right; v[1] = top; v[2] = zc; v3f(v);
			endpolygon();

			/*
			 *   B +
			 *    /|
			 * A +-+ C
			 */
			za = zb; zb = zc; zc = hf->data[y][x];
			dz1 = zc - za;
			delz = zc - zb;
			len = sqrt(del2*dz1*dz1 + del2*delz*delz + del4);
			n[0] = -del*dz1/len;
			n[1] = del*delz/len;
			n[2] = del2/len;
			bgnpolygon();
			n3f(n);
			v[0] = left; v[1] = bot; v[2] = za; v3f(v);
			v[0] = right; v[2] = zc; v3f(v);
			v[1] = top; v[2] = zb; v3f(v);
			endpolygon();
			left = right;
			right += del;
			za = zb;
			zb = zc;
		}
		bot = top;
		top += del;
	}
	
}

static void
GLInstanceDraw(inst)
Instance *inst;
{
	GeomDraw(inst->obj);
}

static void
GLListDraw(list)
List *list;
{
	Geom *ltmp;

	for (ltmp = list->unbounded; ltmp; ltmp = ltmp->next)
		GeomDraw(ltmp);
	for (ltmp = list->list; ltmp; ltmp = ltmp->next)
		GeomDraw(ltmp);
}

static void
GLPlaneDraw(plane)
Plane *plane;
{
	/*
	 * Draw a really big disc.
	 */
	GLDisc((Float)PLANE_RAD, &plane->pos, &plane->norm);
}

static void
GLPolygonDraw(poly)
register Polygon *poly;
{
	register int i;

	GLNormal(&poly->norm);

	bgnpolygon();
	for (i = 0; i < poly->npoints; i++)
		v3d(&poly->points[i]);
	endpolygon();
}

static void
GLSphereDraw(sph)
Sphere *sph;
{
	pushmatrix();
	translate(sph->x, sph->y, sph->z);
	scale(sph->r, sph->r, sph->r);
	callobj(GLSphereObject);
	popmatrix();
}

static void
GLTorusDraw(){}

static void
GLTriangleDraw(tri)
register Triangle *tri;
{
	/*
	 * If Float is a double, use v3d,
	 * otherwise use v3f.
	 */
	if (tri->type != PHONGTRI) {
		GLNormal(&tri->nrm);	
		bgnpolygon();
		v3d(&tri->p[0]); v3d(&tri->p[1]); v3d(&tri->p[2]);
		endpolygon();
	} else {
		bgnpolygon();
		GLNormal(&tri->vnorm[0]);
		v3d(&tri->p[0]);
		GLNormal(&tri->vnorm[1]);
		v3d(&tri->p[1]);
		GLNormal(&tri->vnorm[2]);
		v3d(&tri->p[2]);
		endpolygon();
	}
}

float lightprops[] =	{POSITION, 0., 0., 0., 0.,
			 LCOLOR, 0, 0, 0,
			 SPOTDIRECTION, 0., 0., 0.,
			 SPOTLIGHT, 0., 180.,
			 LMNULL};

float 	*lpos = &lightprops[1],
	*lcolor = &lightprops[6],
	*spotdir = &lightprops[10],
	*spotexp = &lightprops[14],
	*spotspread = &lightprops[15];

float lmodel[] =	{AMBIENT, 1., 1., 1.,
			 ATTENUATION, 1., 0.,
			 LOCALVIEWER, 0.,
#ifdef sgi
			 ATTENUATION2, 0.,
			 TWOSIDE, 1.,
#endif
			 LMNULL};

static void
LightDrawInit()
{
	Light *light;
	extern Light *Lights;

	for (light = Lights; light; light = light->next)
		LightDraw(light);

	switch (curlight-1) {
		case 8:
			lmbind(LIGHT7, 8);
		case 7:
			lmbind(LIGHT6, 7);
		case 6:
			lmbind(LIGHT5, 6);
		case 5:
			lmbind(LIGHT4, 5);
		case 4:
			lmbind(LIGHT3, 4);
		case 3:
			lmbind(LIGHT2, 3);
		case 2:
			lmbind(LIGHT1, 2);
		case 1:
			lmbind(LIGHT0, 1);
	}

	lmodel[1] = Options.ambient.r;
	lmodel[2] = Options.ambient.g;
	lmodel[3] = Options.ambient.b;

	lmdef(DEFLMODEL, 1, sizeof(lmodel) / sizeof(float), lmodel);
	lmbind(LMODEL, 1);
}

static void
LightDraw(light)
Light *light;
{
	if (!light || !light->methods->user)
		return;
	lcolor[0] = light->color.r;
	lcolor[1] = light->color.g;
	lcolor[2] = light->color.b;
	(*light->methods->user)(light->light);
}

static void
GLExtendedLight(ext)
Extended *ext;
{
	lpos[0] = ext->pos.x;
	lpos[1] = ext->pos.y;
	lpos[2] = ext->pos.z;
	lpos[3] = 1.;
	lmdef(DEFLIGHT, curlight++, sizeof(lightprops)/sizeof(float),
		lightprops);
}


static void
GLInfiniteLight(inf)
Infinite *inf;
{
	lpos[0] = inf->dir.x;
	lpos[1] = inf->dir.y;
	lpos[2] = inf->dir.z;
	lpos[3] = 0.;
	lmdef(DEFLIGHT, curlight++, sizeof(lightprops)/sizeof(float),
		lightprops);
}

static void
GLPointLight(pt)
Pointlight *pt;
{
	lpos[0] = pt->pos.x;
	lpos[1] = pt->pos.y;
	lpos[2] = pt->pos.z;
	lpos[3] = 1.;
	lmdef(DEFLIGHT, curlight++, sizeof(lightprops) / sizeof(float),
			lightprops);
}

static void
GLSpotLight(spot)
Spotlight *spot;
{
	lpos[0] = spot->pos.x;
	lpos[1] = spot->pos.y;
	lpos[2] = spot->pos.z;
	lpos[3] = 1.;
	spotdir[0] = spot->dir.x;
	spotdir[1] = spot->dir.y;
	spotdir[2] = spot->dir.z;
	*spotexp = spot->coef;
	*spotspread = 180 * acos(spot->falloff) / PI;
	lmdef(DEFLIGHT, curlight++, sizeof(lightprops) / sizeof(float),
			lightprops);
	/* fix up spot defs so other source methods needn't reset them. */
	*spotspread = 180.;
	*spotexp = 1.;
}

static float boxfaces[6][4][3] = {
{ 	{1., 1., 1.},
	{0., 1., 1.},
	{0., 0., 1.},
	{1., 0., 1.}	},
{	{1., 0., 1.},
	{0., 0., 1.},
	{0., 0., 0.},
	{1., 0., 0.}	},
{	{1., 0., 0.},
	{0., 0., 0.},
	{0., 1., 0.},
	{1., 1., 0.}	},
{	{0., 1., 0.},
	{0., 1., 1.},
	{1., 1., 1.},
	{1., 1., 0.}	},
{	{1., 1., 1.},
	{1., 0., 1.},
	{1., 0., 0.},
	{1., 1., 0.}	},
{	{0., 0., 1.},
	{0., 1., 1.},
	{0., 1., 0.},
	{0., 0., 0.}}};

float boxnorms[6][3] = {
	{0, 0, 1},
	{0, -1, 0},
	{0, 0, -1},
	{0, 1, 0},
	{1, 0, 0,},
	{-1, 0, 0}};

/*
 * Define a unit cube centered at (0.5, 0.5, 0.5)
 */
static Object
GLBoxObjectDefine()
{
	int i, box;

	makeobj(box = genobj());
	for (i = 0; i < 6; i++)	{
		n3f(boxnorms[i]);
		polf(4, boxfaces[i]);
	}
	closeobj();
	return box;
}

static void
GLUnitCircle(z, up)
float z;
int up;
{
	int i;
	float norm[3];

	norm[0] = norm[1] = 0.;
	bgnpolygon();

	if (up) {
		norm[2] = 1.;
		n3f(norm);	
		for (i = 0; i < CIRCLE_SAMPLES; i++) {
			CirclePoints[i][2] = z;
			v3f(CirclePoints[i]);
		}
	} else {
		norm[2] = -1.;
		n3f(norm);	
		for (i = CIRCLE_SAMPLES -1; i; i--) {
			CirclePoints[i][2] = z;
			v3f(CirclePoints[i]);
		}
	}

	endpolygon();
}

static Object
GLCylObjectDefine()
{
	int i, j, cyl;
	float norm[3];

	makeobj(cyl = genobj());
	norm[2] = 0;
	for (i = 0; i < CIRCLE_SAMPLES; i++) {
		j = (i+1)%CIRCLE_SAMPLES;
		norm[0] = CirclePoints[i][0];
		norm[1] = CirclePoints[i][1];
#ifdef sgi
		n3f(norm);
		bgnpolygon();
		CirclePoints[i][2] = 0;
		v3f(CirclePoints[i]);
		CirclePoints[j][2] = 0;
		v3f(CirclePoints[j]);
		CirclePoints[j][2] = 1;
		v3f(CirclePoints[j]);
		CirclePoints[i][2] = 1;
		v3f(CirclePoints[i]);
		endpolygon();
#else
		n3f(norm);
		pmv(CirclePoints[i][0], CirclePoints[i][1], 0.);
		pdr(CirclePoints[j][0], CirclePoints[j][1], 0.);
		pdr(CirclePoints[j][0], CirclePoints[j][1], 1.);
		pdr(CirclePoints[i][0], CirclePoints[i][1], 1.);
		pclos();
#endif
	}
	closeobj();
	return cyl;
}

/*
 * Fill CirclePoints[t] with X and Y values cooresponding to points
 * along the unit circle.  The size of CirclePoints is equal to
 * CIRCLE_SAMPLES.
 */
CircleDefine()
{
	int i;
	double theta, dtheta;

	dtheta = 2.*M_PI / (double)CIRCLE_SAMPLES;
	CirclePoints = (float **)malloc(CIRCLE_SAMPLES * sizeof(float *));
	for (i = 0, theta = 0; i < CIRCLE_SAMPLES; i++, theta += dtheta) {
		CirclePoints[i] = (float *)malloc(3 * sizeof(float));
		CirclePoints[i][0] = cos(theta);
		CirclePoints[i][1] = sin(theta);
		/* Z is left unset. */	
	}
}

/*
 * Given world bounds, determine near and far
 * clipping planes.  Only problem occurs when there are
 * unbbounded objects (planes)...
 */
static void
ComputeClippingPlanes(near, far, bounds)
float *near, *far;
Float bounds[2][3];
{
	Vector e, c;
	double rad, d;


	/*
	 * Compute 'radius' of scene.
	 */
	rad = max(max((bounds[HIGH][X] - bounds[LOW][X])*0.5,
		      (bounds[HIGH][Y] - bounds[LOW][Y])*0.5),
		      (bounds[HIGH][Z] - bounds[LOW][Z])*0.5);

	c.x = (bounds[LOW][X] + bounds[HIGH][X])*0.5;
	c.y = (bounds[LOW][Y] + bounds[HIGH][Y])*0.5;
	c.z = (bounds[LOW][Z] + bounds[HIGH][Z])*0.5;

	/*
	 * Compute position of eye relative to the center of the sphere.
	 */
	VecSub(Camera.pos, c, &e);
	d = VecNormalize(&e);
	if (d <= rad)
		/*
		 * Eye is inside sphere.
		 */
		*near = DEFAULT_NEAR;
	else
		*near = (d - rad)*0.2;
	*far = (d + rad)*2.;
}
