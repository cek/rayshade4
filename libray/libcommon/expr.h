/*
 * Copyright (C) 1989-2015, Craig E. Kolb, Rod G. Bogart
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
#ifndef EXPR_H
#define EXPR_H

#define FLOAT_EXPR		0
#define BUILTIN_EXPR		1

typedef struct Expr {
	short	type,			/* Expr type (float/builtin/time) */
		timevary,		/* does the expr vary over time? */
		symtab,			/* is the expr in the symtab? */
		nparams;		/* # of params, if builtin */
	Float	value,			/* float val/last eval if timeexpr */
		timenow,		/* time of last evaluation */
		(*function)();
	struct Expr **params;		/* parameters, if a builtin */
} Expr;

typedef struct ExprAssoc {
	Float *lhs;			/* left-hand side */
	Expr *expr;			/* right-hand side */
	struct ExprAssoc *next;		/* Next in list */
} ExprAssoc;
	
extern Float	ExprEval();

extern Expr	*ExprResolve1(), *ExprResolve2(), *ExprResolve3(),
		*ExprResolve4(), *ExprResolve5(), *ExprFloatCreate();
extern Expr	*TimeExpr, *FrameExpr, *ExprReuseFloatCreate();

extern ExprAssoc *AssocCreate();

#endif /* EXPR_H */
