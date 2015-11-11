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

char rcsid[] = "$Id: main.c,v 4.0 91/07/17 14:50:39 kolb Exp Locker: kolb $";

#include "rayshade.h"
#include "options.h"
#include "stats.h"
#include "viewing.h"
#include "picture.h"

int
#ifdef LINDA
rayshade_main(argc, argv)
#else
main(argc, argv)
#endif
int argc;
char **argv;
{
	Float utime, stime, lasttime;
	int i;
	extern Geom *World;

#ifdef LINDA
	Options.workernum = 0;	/* we're the supervisor */
#endif

	RSInitialize(argc, argv);


	/*
	 * Start the first frame.
	 */
	RSStartFrame(Options.startframe);
	/*
 	 * Print more information than we'll ever need to know...
	 */
	if (Options.verbose) {
		/* World object info. */
		AggregatePrintInfo(World, Stats.fstats);
		/* Print info about rendering options and the like. */
		RSOptionsList();
	}
	/*
	 * Start new picture.
	 */
	PictureStart(argv);
	/*
	 * Print preprocessing time.
	 */
	RSGetCpuTime(&utime, &stime);
	fprintf(Stats.fstats,"Preprocessing time:\t");
	fprintf(Stats.fstats,"%2.2fu  %2.2fs\n", utime, stime);
	fprintf(Stats.fstats,"Starting trace.\n");
	(void)fflush(Stats.fstats);
	lasttime = utime+stime;
	/*
	 * Render the first frame
	 */
	raytrace(argc, argv);
	/*
	 * Render the remaining frames.
	 */
	for (i = Options.startframe +1; i <= Options.endframe ; i++) {
		PictureFrameEnd();	/* End the previous frame */
		RSGetCpuTime(&utime, &stime);
		fprintf(Stats.fstats, "Total CPU time for frame %d: %2.2f \n", 
			i - 1, utime+stime - lasttime);
		PrintMemoryStats(Stats.fstats);
		(void)fflush(Stats.fstats);
		lasttime = utime+stime;
		RSStartFrame(i);
		if (Options.verbose) {
			AggregatePrintInfo(World, Stats.fstats);
			(void)fflush(Stats.fstats);
		}
		PictureStart(argv);
		raytrace(argc, argv);
	}
	/*
	 * Close the image file.
	 */
	PictureFrameEnd();	/* End the last frame */
	PictureEnd();
	StatsPrint();
	return 0;
}
