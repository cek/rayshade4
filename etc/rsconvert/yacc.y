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
		{
			NEWLINE();
		}
		;
Objdefs		: Objdefs Objdef
		|
		;
Objdef		: Primitive
		| Surface
		| Child
		| List
		| Grid
		| Object
		;
Textures	: Textures Texture
		|
		;
Texture		: TEXTURE Texturetype Transforms
		{
			NEWLINE();
		}
		;
Texturetype	: CHECKER String
		{
			NEWLINE();
		}
		| BLOTCH Fnumber String
		{
			NEWLINE();
		}
		| BUMP Fnumber
		{
			NEWLINE();
		}
		| MARBLE
		{
			NEWLINE();
		}
		| MARBLE String
		{
			NEWLINE();
		}
		| FBM Fnumber Fnumber Fnumber Fnumber Int Fnumber
		{
			NEWLINE();
		}
		| FBM Fnumber Fnumber Fnumber Fnumber Int Fnumber String
		{
			NEWLINE();
		}
		| FBMBUMP Fnumber Fnumber Fnumber Fnumber Int
		{
			NEWLINE();
		}
		| WOOD
		{
			NEWLINE();
		}
		| GLOSS Fnumber
		{
			NEWLINE();
		}
		;
Child		: Childdef Textures
		{
			NEWLINE();
		}
		;
Childdef	: OBJECT String Transforms
		{
			NEWLINE();
		}
		;
Transforms	: Transforms Transform
		| /* empty */
		;
Transform	: TRANSLATE Vector
		{
			NEWLINE();
		}
		| ROTATE Vector sFnumber
		{
			WriteFloat(-$3);
			NEWLINE();
		}
		| SCALE Fnumber Fnumber Fnumber
		{
			NEWLINE();
		}
		| TRANSFORM sFnumber sFnumber sFnumber
				sFnumber sFnumber sFnumber
				sFnumber sFnumber sFnumber
		{
			/* have to transpose... */
			WriteFloat($2); WriteFloat($5); WriteFloat($8);
			WriteFloat($3); WriteFloat($6); WriteFloat($9);
			WriteFloat($4); WriteFloat($7); WriteFloat($10);
			NEWLINE();
		}
		| TRANSFORM sFnumber sFnumber sFnumber
				sFnumber sFnumber sFnumber
				sFnumber sFnumber sFnumber
				sFnumber sFnumber sFnumber
		{
			/* transpose it */
			WriteFloat($2); WriteFloat($5); WriteFloat($8);
			WriteFloat($3); WriteFloat($6); WriteFloat($9);
			WriteFloat($4); WriteFloat($7); WriteFloat($10);
			WriteFloat($11); WriteFloat($12); WriteFloat($13);
			NEWLINE();
		}
		;
Eyep		: EYEP Vector Transforms
		{
			NEWLINE();
		}
		;
Lookp		: LOOKP Vector
		{
			NEWLINE();
		}
		;
Up		: UP Vector
		{
			NEWLINE();
		}
		;
Fov		: FOV Fnumber Fnumber
		{
			NEWLINE();
		}
		| FOV Fnumber
		{
			NEWLINE();
		}
		;
Samples		: SAMPLES Int
		{
			NEWLINE();
		}
		;
Adaptive	: ADAPTIVE sInt
		{
			WriteFloat((Float)($2+1));
			NEWLINE();
		}
		;
Contrast	: CONTRAST Fnumber Fnumber Fnumber
		{
			NEWLINE();
		}
		;
Cutoff		: CUTOFF Fnumber
		{
			NEWLINE();
		}
		;
Jittered	: JITTERED
		{
			NEWLINE();
		}
		;
Screen		: SCREEN Int Int
		{
			NEWLINE();
		}
		| RESOLUTION Int Int
		{
			NEWLINE();
		}
		;
Aperture	: APERTURE Fnumber
		{
			NEWLINE();
		}
		;
Focaldist	: FOCALDIST Fnumber
		{
			NEWLINE();
		}
		;
Maxdepth	: MAXDEPTH Int
		{
			NEWLINE();
		}
		;
Background	: BACKGROUND Color
		{
			NEWLINE();
		}
		;
Light		: Lightdef POINT Vector
		{
			NEWLINE();
		}
		| Lightdef DIRECTIONAL Vector
		{
			NEWLINE();
		}
		| Lightdef EXTENDED sVector sFnumber
		{
			WriteFloat($4);
			WriteVector(&$3);
			NEWLINE();
		}
		;
Lightdef	: LIGHT Fnumber
		{
			NEWLINE();
		}
		| LIGHT Color
		{
			NEWLINE();
		}
		;
Surface		: SURFACE String
			sColor sColor sColor
			sFnumber sFnumber sFnumber sFnumber
		{
			if ($3.r || $3.g || $3.b) {
				WriteString("\tambient");
				WriteColor(&$3);
				WriteNewline();
			}
			if ($4.r || $4.g || $4.b) {
				WriteString("\tdiffuse");
				WriteColor(&$4);
				WriteNewline();
			}
			if ($5.r || $5.g || $5.b) {
				WriteString("\tspecular");
				WriteColor(&$5);
				WriteNewline();
				if ($6) {
					WriteString("\tspecpow");
					WriteFloat($6);
					WriteNewline();
				}
			}
			if ($7) {
				WriteString("\treflect");
				WriteFloat($7);
				WriteNewline();
			}
			if ($8) {
				WriteString("\ttransp");
				WriteFloat($8);
				WriteString("index");
				WriteFloat($9);
				WriteNewline();
			}
		}
		| SURFACE String sColor sColor sColor
			sFnumber sFnumber sFnumber sFnumber sFnumber sFnumber
		{
			if ($3.r || $3.g || $3.b) {
				WriteString("\tambient");
				WriteColor(&$3);
				WriteNewline();
			}
			if ($4.r || $4.g || $4.b) {
				WriteString("\tdiffuse");
				WriteColor(&$4);
				WriteNewline();
			}
			if ($5.r || $5.g || $5.b) {
				WriteString("\tspecular");
				WriteColor(&$5);
				WriteNewline();
				if ($6) {
					WriteString("\tspecpow");
					WriteFloat($6);
					WriteNewline();
				}
			}
			if ($7) {
				WriteString("\treflect");
				WriteFloat($7);
				WriteNewline();
			}
			if ($8) {
				WriteString("\ttransp");
				WriteFloat($8);
				WriteString("index");
				WriteFloat($9);
				WriteNewline();
			}
			if ($10) {
				WriteString("\ttranslu");
				WriteFloat($10);
				WriteString("1 1 1");
				WriteFloat($11);
				WriteNewline();
			}
		}
		;
HeightField	: HEIGHTFIELD String String
		{
			NEWLINE();
		}
		;
Poly		: POLY String Polypoints
		{
			NEWLINE();
		}
		;
Polypoints	: /* empty */
		| Polypoints Polypoint
		;
Polypoint	: Vector
		{
			NEWLINE();
		}
		;
Cone		: CONE String sVector sVector sFnumber sFnumber
		{
			/* Radii now precede points */
			WriteFloat($5);
			WriteVector(&$3);
			WriteFloat($6);
			WriteVector(&$4);
			NEWLINE();
		}
		;
Cylinder	: CYL sString sVector sVector sFnumber
		{
			Vector tmp;

			WriteString($2);
			/* Radius now goes first */
			WriteFloat($5);
			WriteVector(&$3);
			WriteVector(&$4);
			NEWLINE();
			WriteVerbatim("#ifdef ENDCAPS\n");
				VecSub($3, $4, &tmp);
				WriteVerbatim("disc ");
				WriteString($2);
				WriteFloat($5);	/* radius */
				WriteVector(&$3); /* pos */
				WriteVector(&tmp);
				WriteVerbatim("\ndisc ");
				WriteString($2);
				VecScale(-1, tmp, &tmp);
				WriteFloat($5);	/* radius */
				WriteVector(&$4); /* pos */
				WriteVector(&tmp);
			WriteVerbatim("\n#endif\n");
		}
		;
Sphere		: SPHERE String Fnumber Vector
		{
			NEWLINE();
		}
		;
Box		: BOX String
			sFnumber sFnumber sFnumber
			sFnumber sFnumber sFnumber
		{
			/* give box corners */
			WriteFloat($3 - $6);
			WriteFloat($4 - $7);
			WriteFloat($5 - $8);
			WriteFloat($3 + $6);
			WriteFloat($4 + $7);
			WriteFloat($5 + $8);
			NEWLINE();
		}
		;
Triangle	: TRIANGLE String Vector Vector Vector
		{
			NEWLINE();
		}
		| TRIANGLE String Vector Vector Vector Vector Vector Vector
		{
			NEWLINE();
		}
		;
Superq		: SUPERQ String
			Fnumber Fnumber Fnumber
			Fnumber Fnumber Fnumber
			Fnumber
		{
			WriteVerbatim("*/");
			NEWLINE();
		}
		;
Plane		: PLANE String sVector sVector
		{
			/* reverse order of point/normal */
			WriteVector(&$4);
			WriteVector(&$3);
			NEWLINE();
		}
		;
Outfile		: OUTFILE String
		{
			NEWLINE();
		}
		;
Mist		: MIST Color Color Fnumber Fnumber
		{
			NEWLINE();
		}
		;
Fog		: FOG sFnumber sColor
		{
			WriteColor(&$3);
			WriteFloat($2); WriteFloat($2); WriteFloat($2);
			NEWLINE();
		}
		;
Color		: Fnumber Fnumber Fnumber
		;
sColor		: sFnumber sFnumber sFnumber
		{
			$$.r = $1; $$.g = $2; $$.b = $3;
		}
		;
Vector		: Fnumber Fnumber Fnumber
		;
sVector		: sFnumber sFnumber sFnumber
		{
			$$.x = $1; $$.y = $2; $$.z = $3;
		}
		;
Fnumber		: tFLOAT
		{ WriteFloat($1); }
		| tINT
		{ WriteFloat((Float)$1); }
		;
Int		: tINT
		{ WriteFloat((Float)$1); };
sInt		: tINT
		{ $$ = (int)$1; };
sFnumber	: tFLOAT
		| tINT
		{ $$ = (double)$1; }
		;
String		: tSTRING
		{ WriteString($1); }
sString		: tSTRING
		{ $$ = $1; }
ADAPTIVE	: tADAPTIVE	{ WriteString("samples"); }
APERTURE	: tAPERTURE	{ WriteString("aperture"); }
BACKGROUND	: tBACKGROUND	{ WriteString("background"); }
BLOTCH		: tBLOTCH	{ WriteString("blotch"); }
BOX		: tBOX		{ WriteString("box"); }
BUMP		: tBUMP		{ WriteString("bump"); }
CONE		: tCONE		{ WriteString("cone"); }
CYL		: tCYL		{ WriteString("cylinder"); }
DIRECTIONAL	: tDIRECTIONAL	{ WriteString("directional"); }
ENDDEF		: tENDDEF	{ EndDefine(); }
EXTENDED	: tEXTENDED	{ WriteString("extended"); }
EYEP		: tEYEP		{ WriteString("eyep"); }
FBM		: tFBM		{ WriteString("fbm"); }
FBMBUMP		: tFBMBUMP	{ WriteString("fbmbump"); }
FOCALDIST	: tFOCALDIST	{ WriteString("focaldist"); }
FOG		: tFOG		{ WriteString("atmosphere fog"); }
FOV		: tFOV		{ WriteString("fov"); }
GLOSS		: tGLOSS	{ WriteString("gloss"); }
GRID		: tGRID	tINT tINT tINT	{ SetTypeGrid($2,$3,$4); }
HEIGHTFIELD	: tHEIGHTFIELD	{ WriteString("heightfield"); }
JITTERED	: tJITTERED	{ WriteString("jittered"); }
LIGHT		: tLIGHT	{ WriteString("light"); }
LIST		: tLIST		{ SetTypeList(); }
LOOKP		: tLOOKP	{ WriteString("lookp"); }
MARBLE		: tMARBLE	{ WriteString("marble"); }
MAXDEPTH	: tMAXDEPTH	{ WriteString("maxdepth"); }
MIST		: tMIST		{ WriteString("atmosphere mist"); }
OBJECT		: tOBJECT	{ WriteString("object"); }
OUTFILE		: tOUTFILE	{ WriteString("outfile"); }
PLANE		: tPLANE	{ WriteString("plane"); }
POINT		: tPOINT	{ WriteString("point"); }
POLY		: tPOLY		{ WriteString("poly"); }
ROTATE		: tROTATE	{ WriteString("rotate"); }
SAMPLES		: tSAMPLES	{ WriteString("samples"); }
SCALE		: tSCALE	{ WriteString("scale"); }
SCREEN		: tSCREEN	{ WriteString("screen"); }
SPHERE		: tSPHERE	{ WriteString("sphere"); }
STARTDEF	: tSTARTDEF tSTRING	{ StartDefine($2); }
SUPERQ		: tSUPERQ	{ WriteString("/* superq"); }
SURFACE		: tSURFACE	{ WriteString("surface"); }
RESOLUTION	: tRESOLUTION	{ WriteString("resolution"); }
TRANSLATE	: tTRANSLATE	{ WriteString("translate"); }
TRANSFORM	: tTRANSFORM	{ WriteString("transform"); }
TRIANGLE	: tTRIANGLE	{ WriteString("triangle"); }
UP		: tUP		{ WriteString("up"); }
ENDFILE		: tENDFILE	{ /* Don't do a thing. */ }
TEXTURE		: tTEXTURE	{ WriteString("texture"); }
CHECKER		: tCHECKER	{ WriteString("checker"); }
WOOD		: tWOOD		{ WriteString("wood"); }
CONTRAST	: tCONTRAST	{ WriteString("contrast"); }
CUTOFF		: tCUTOFF	{ WriteString("cutoff"); }
%%

#define STARTBUFSIZ	(1 << 18)

typedef struct db {
	int bufsiz, curpos;
	int type, x, y, z;
	char *name;
	struct db *next;
	char *memory;
} DefBuf;

DefBuf *defbuf = NULL;

yyerror(s)
{
	fprintf(stderr,"rsconvert: %s, line %d: %s \n",
			yyfilename[0] ? yyfilename : "stdin",
			yylineno, s);
}

StartDefine(name)
char *name;
{
	DefBuf *new;
	/*
	 * Push new buffer onto define stack.
	 */	
	new = (DefBuf *)Malloc(sizeof(DefBuf));
	new->bufsiz = STARTBUFSIZ;
	new->type = LIST;
	new->curpos = 0;
	new->name = name;
	new->memory = (char *)Calloc(new->bufsiz, sizeof(char));
	new->next = defbuf;
	defbuf = new;
}

EndDefine()
{
	char buf[BUFSIZ];
	DefBuf *old;

	old = defbuf;
	defbuf = defbuf->next;
	if (old->type == LIST) {
		sprintf(buf, "name %s list", old->name);
	} else {
		sprintf(buf, "name %s grid %d %d %d", old->name,
				old->x, old->y, old->z);
	}
	/*
	 * dump goodies
	 */
	WriteVerbatim(buf);
	WriteVerbatim(old->memory);
	WriteVerbatim("end");
	free((voidstar)old->memory);
	free((voidstar)old);
	
}

WriteString(str)
char *str;
{
	WriteVerbatim(str);
	WriteChar(' ');
}

WriteVerbatim(str)
char *str;
{
	int n;

	for (n = strlen(str); n; n--)
		WriteChar(*(str++));
}

WriteChar(c)
char c;
{
	if (defbuf) {
		if (defbuf->curpos == defbuf->bufsiz -1) {
			defbuf->bufsiz *= 2;
			defbuf->memory = (char *)realloc(defbuf->memory,
				defbuf->bufsiz * sizeof(char));
			if (defbuf->memory == (char *)NULL) {
				fprintf(stderr,"realloc failed!\n");
				exit(-1);
			}
		}
		defbuf->memory[defbuf->curpos++] = c;
		defbuf->memory[defbuf->curpos] = (char)NULL;
	} else
		putchar(c);
}

WriteVector(v)
Vector *v;
{
	WriteFloat(v->x);
	WriteFloat(v->y);
	WriteFloat(v->z);
}

WriteFloat(x)
Float x;
{
	char buf[BUFSIZ];
	sprintf(buf, "%g ", x);
	WriteVerbatim(buf);
}

WriteNewline()
{
	WriteVerbatim("\n");
}

SetTypeList()
{
	if (defbuf)
		defbuf->type = LIST;
	/* else set world type */
}

SetTypeGrid(x,y,z)
int x, y, z;
{
	if (defbuf) {
		defbuf->type = GRID;
		defbuf->x = x; defbuf->y = y; defbuf->z = z;
	} /* else set world type */
}

WriteColor(c)
Color *c;
{
	WriteFloat(c->r);
	WriteFloat(c->g);
	WriteFloat(c->b);
}
