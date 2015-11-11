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

char rcsid[] = "$Id: main.c,v 4.0 91/07/17 17:36:46 kolb Exp Locker: kolb $";

#include "rayshade.h"
#include "options.h"
#include "stats.h"
#include "viewing.h"
#include "picture.h"

void RSInitialize(), RSStartFrame();

int
main(argc, argv)
int argc;
char **argv;
{
	Float utime, stime;

	/*
 	 * Initialize variables, etc.
	 */
	RSInitialize(argc, argv);
	RSStartFrame(Options.startframe);
	/*
 	 * Print more information than we'll ever need to know...
	 */
	if (Options.verbose) {
		extern Geom *World;
		/* World object info. */
		AggregatePrintInfo(World, Stats.fstats);
		/* Print info about rendering options and the like. */
		RSOptionsList();
	}
	/*
	 * Print preprocessing time.
	 */
	RSGetCpuTime(&utime, &stime);
	fprintf(Stats.fstats,"Preprocessing time:\t");
	fprintf(Stats.fstats,"%2.2fu  %2.2fs\n", utime, stime);
	fprintf(Stats.fstats,"Starting trace.\n");
	(void)fflush(Stats.fstats);
	/*
	 * Render the image.
	 */
	Render(argc, argv);
	StatsPrint();
	return 0;
}

static void
RSStartFrame(frame)
int frame;
{
	/*
	 * Set the frame start time
	 */
	Options.framenum = frame;
	Options.framestart = Options.starttime +
			Options.framenum*Options.framelength;
	SamplingSetTime(Options.framestart, Options.shutterspeed,
			Options.framenum);
	/*
	 * Set up viewing parameters.
	 */
	RSViewing();
	/*
	 * Initialize world
	 */
	WorldSetup();
}

/*
 * Initialize non-time-varying goodies.
 */
static void
RSInitialize(argc, argv)
int argc;
char **argv;
{
	/*
 	 * Initialize variables, etc.
	 */
	RSSetup();
	/*
	 * Parse options from command line.
	 */
	RSOptionsSet(argc, argv);
	/*
	 * Process input file.
	 */
	if (Options.verbose) {
		VersionPrint();
		fprintf(Stats.fstats,"Reading input file...\n");
		(void)fflush(Stats.fstats);
	}
	RSReadInputFile();
	/*
	 * Set variables that weren't set on command line
	 * or in input file.
	 */
	RSCleanup();
	/*
	 * Set sampling options.
	 */
	SamplingSetOptions(Options.samples, Options.gaussian,
			   Options.filterwidth);
}
