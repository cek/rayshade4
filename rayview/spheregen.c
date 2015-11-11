/*
 * Routines to define a unit sphere Object.
 * If you have a sphere library (-lsphere on most SGI machines), you can
 * use that by defining SPHERELIB on the cc command line.  Otherwise,
 * we use Jon Leech's code to generate a sphere.
 * C. Kolb 6/91
 */
#ifdef SPHERELIB
#include <gl.h>

/*
 * Define unit sphere Object.  Here, we make use of sgi's spherelib.
 * If not available, undefine SPHERELIB, and the code below will be used.
 */
Object
GLSphereObjectDefine()
{
	int sphere;

	sphere = genobj();
	sphobj(sphere);
	return sphere;
}
#else

/*
 * The following code has been slightly modified.  The original sphere
 * code is available via anonymous
 * ftp from weedeater.math.yale.edu (130.132.23.17) in pub/sphere.c
 *
 * Modifications include changing print_triangle to draw a GL object,
 * the inclusion of gl.h,
 * the renaming of main() to GLSphereObjectDefine(), the nuking of anything
 * phigs-related, and setting the ccw flag to be true.
 * 
 * C. Kolb 6/91
 */

/*% cc -g sphere.c -o sphere -lm
 *
 * sphere - generate a triangle mesh approximating a sphere by
 *  recursive subdivision. First approximation is an octahedron;
 *  each level of refinement increases the number of triangles by
 *  a factor of 4.
 * Level 3 (128 triangles) is a good tradeoff if gouraud
 *  shading is used to render the database.
 *
 * Usage: sphere [level] [-p] [-c]
 *	level is an integer >= 1 setting the recursion level (default 1).
 *	-p causes generation of a PPHIGS format ASCII archive
 *	    instead of the default generic output format.
 *	-c causes triangles to be generated with vertices in counterclockwise
 *	    order as viewed from the outside in a RHS coordinate system.
 *	    The default is clockwise order.
 *
 *  The subroutines print_object() and print_triangle() should
 *  be changed to generate whatever the desired database format is.
 *
 * Jon Leech (leech@cs.unc.edu) 3/24/89
 */
#include <stdio.h>
#include <math.h>

#include <gl.h>	

typedef struct {
    double  x, y, z;
} point;

typedef struct {
    point     pt[3];	/* Vertices of triangle */
    double    area;	/* Unused; might be used for adaptive subdivision */
} triangle;

typedef struct {
    int       npoly;	/* # of triangles in object */
    triangle *poly;	/* Triangles */
} object;

/* Six equidistant points lying on the unit sphere */
#define XPLUS {  1,  0,  0 }	/*  X */
#define XMIN  { -1,  0,  0 }	/* -X */
#define YPLUS {  0,  1,  0 }	/*  Y */
#define YMIN  {  0, -1,  0 }	/* -Y */
#define ZPLUS {  0,  0,  1 }	/*  Z */
#define ZMIN  {  0,  0, -1 }	/* -Z */

/* Vertices of a unit octahedron */
triangle octahedron[] = {
    { { XPLUS, ZPLUS, YPLUS }, 0.0 },
    { { YPLUS, ZPLUS, XMIN  }, 0.0 },
    { { XMIN , ZPLUS, YMIN  }, 0.0 },
    { { YMIN , ZPLUS, XPLUS }, 0.0 },
    { { XPLUS, YPLUS, ZMIN  }, 0.0 },
    { { YPLUS, XMIN , ZMIN  }, 0.0 },
    { { XMIN , YMIN , ZMIN  }, 0.0 },
    { { YMIN , XPLUS, ZMIN  }, 0.0 }
};

/* A unit octahedron */
object oct = {
    sizeof(octahedron) / sizeof(octahedron[0]),
    &octahedron[0]
};

/* Forward declarations */
point *normalize(/* point *p */);
point *midpoint(/* point *a, point *b */);
void print_object(/* object *obj, int level */);
void print_triangle(/* triangle *t */);

/*extern char *malloc(/* unsigned );*/

Object
GLSphereObjectDefine(maxlevel)
int maxlevel;
{
    object *old,
	   *new;
    int     ccwflag = 1,	/* Reverse vertex order if true */
	    i, level;
    Object sph;

    makeobj(sph = genobj());

    if (ccwflag) {
	/* Reverse order of points in each triangle */
	for (i = 0; i < oct.npoly; i++) {
	    point tmp;
			  tmp = oct.poly[i].pt[0];
	    oct.poly[i].pt[0] = oct.poly[i].pt[2];
	    oct.poly[i].pt[2] = tmp;
	}
    }

    old = &oct;

    /* Subdivide each starting triangle (maxlevel - 1) times */
    for (level = 1; level < maxlevel; level++) {
	/* Allocate a new object */
	new = (object *)malloc(sizeof(object));
	if (new == NULL) {
	    fprintf(stderr, "GLSphereObjectDefine: out of memory, level %d\n",
		level);
	    exit(1);
	}
	new->npoly = old->npoly * 4;

	/* Allocate 4* the number of points in the current approximation */
	new->poly  = (triangle *)malloc(new->npoly * sizeof(triangle));
	if (new->poly == NULL) {
	    fprintf(stderr, "GLSphereObjectDefine: out of memory, level %d\n",
		level);
	    exit(1);
	}

	/* Subdivide each triangle in the old approximation and normalize
	 *  the new points thus generated to lie on the surface of the unit
	 *  sphere.
	 * Each input triangle with vertices labelled [0,1,2] as shown
	 *  below will be turned into four new triangles:
	 *
	 *			Make new points
	 *			    a = (0+2)/2
	 *			    b = (0+1)/2
	 *			    c = (1+2)/2
	 *	  1
	 *	 /\		Normalize a, b, c
	 *	/  \
	 *    b/____\ c		Construct new triangles
	 *    /\    /\		    [0,b,a]
	 *   /	\  /  \		    [b,1,c]
	 *  /____\/____\	    [a,b,c]
	 * 0	  a	2	    [a,c,2]
	 */
	for (i = 0; i < old->npoly; i++) {
	    triangle
		 *oldt = &old->poly[i],
		 *newt = &new->poly[i*4];
	    point a, b, c;

	    a = *normalize(midpoint(&oldt->pt[0], &oldt->pt[2]));
	    b = *normalize(midpoint(&oldt->pt[0], &oldt->pt[1]));
	    c = *normalize(midpoint(&oldt->pt[1], &oldt->pt[2]));

	    newt->pt[0] = oldt->pt[0];
	    newt->pt[1] = b;
	    newt->pt[2] = a;
	    newt++;

	    newt->pt[0] = b;
	    newt->pt[1] = oldt->pt[1];
	    newt->pt[2] = c;
	    newt++;

	    newt->pt[0] = a;
	    newt->pt[1] = b;
	    newt->pt[2] = c;
	    newt++;

	    newt->pt[0] = a;
	    newt->pt[1] = c;
	    newt->pt[2] = oldt->pt[2];
	}

	if (level > 1) {
	    free(old->poly);
	    free(old);
	}

	/* Continue subdividing new triangles */
	old = new;
    }

    /* Print out resulting approximation */
   
    print_object(old, maxlevel);
    closeobj();
    return sph;
}

/* Normalize a point p */
point *normalize(p)
point *p;
{
    static point r;
    double mag;

    r = *p;
    mag = r.x * r.x + r.y * r.y + r.z * r.z;
    if (mag != 0.0) {
	mag = 1.0 / sqrt(mag);
	r.x *= mag;
	r.y *= mag;
	r.z *= mag;
    }

    return &r;
}

/* Return the midpoint on the line between two points */
point *midpoint(a, b)
point *a, *b;
{
    static point r;

    r.x = (a->x + b->x) * 0.5;
    r.y = (a->y + b->y) * 0.5;
    r.z = (a->z + b->z) * 0.5;

    return &r;
}

/* Write out all triangles in an object */
void print_object(obj, level)
object *obj;
int level;
{
    int i;

    /* Spit out coordinates for each triangle */
    for (i = 0; i < obj->npoly; i++)
	print_triangle(&obj->poly[i]);
}

/* Output a triangle */
void print_triangle(t)
triangle *t;
{
    int i;
    float p[3];

#ifdef sgi
    bgnpolygon();
#endif

    p[0] = t->pt[0].x; p[1] = t->pt[0].y; p[2] = t->pt[0].z;
    n3f(p);
#ifdef sgi
    v3f(p);
#else
    pmv(p[0], p[1], p[2]);
#endif
    p[0] = t->pt[1].x; p[1] = t->pt[1].y; p[2] = t->pt[1].z;
    n3f(p);
#ifdef sgi
    v3f(p);
#else
    pdr(p[0], p[1], p[2]);
#endif
    p[0] = t->pt[2].x; p[1] = t->pt[2].y; p[2] = t->pt[2].z;
    n3f(p);
#ifdef sgi
    v3f(p);
    endpolygon();
#else
    pdr(p[0], p[1], p[2]);
    pclos();
#endif
}

#endif
