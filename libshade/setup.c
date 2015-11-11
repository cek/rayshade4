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
#include "rayshade.h"
#include "defaults.h"
#include "libsurf/surface.h"
#include "libsurf/atmosphere.h"
#include "liblight/light.h"
#include "liblight/infinite.h"
#include "libobj/list.h"
#include "options.h"
#include "stats.h"
#include "viewing.h"
#include "picture.h"

#ifdef MULTIMAX
#include <parallel.h>
#define SHARED_BYTES	23	/* 2^23 bytes of shared memory */
#endif

extern GeomList *Defstack;

static void SetupWorld();

/*
 * Set default parameters
 */
void
RSSetup()
{
	extern SurfList *CurSurf;
	extern Medium TopMedium;
	extern void NoiseInit();
#ifdef MULTIMAX
	unsigned int bytes;

	/*
	 * Initialize shared memory stuff.
	 */
	bytes = 1 << SHARED_BYTES;
	if (share_malloc_init(bytes) == -1) {
		RLerror(RL_PANIC, "Cannot share_malloc %d bytes.\n",bytes);
	} else
		fprintf(fstats,"Malloced %d bytes of shared memory.\n",
				bytes);
#endif

	Camera.hfov = HFOV;
	Camera.vfov = UNSET;
	Camera.pos.x = EYEX;
	Camera.pos.y = EYEY;
	Camera.pos.z = EYEZ;
	Camera.lookp.x = LOOKX;
	Camera.lookp.y = LOOKY;
	Camera.lookp.z = LOOKZ;
	Camera.up.x = UPX;
	Camera.up.y = UPY;
	Camera.up.z = UPZ;
	Camera.focaldist = UNSET;
	Camera.aperture = 0.;

	Screen.xres = Screen.yres = UNSET;

	Options.cpp = TRUE;
	Options.maxdepth = MAXDEPTH;
	Options.report_freq = REPORTFREQ;
	Options.jitter = TRUE;
	Options.samples = UNSET;
	Options.gaussian = GAUSSIAN;
	Options.filterwidth = UNSET;
	Options.contrast.r = UNSET;
	Options.ambient.r = Options.ambient.g =
		Options.ambient.b = 1.0;
	Options.cutoff.r = UNSET;
	Options.cache = TRUE;
	Options.shadowtransp = TRUE;
	Options.crop[LOW][X] = Options.crop[LOW][Y] = 0.;
	Options.crop[HIGH][X] = Options.crop[HIGH][Y] = 1.;
	Stats.fstats = stderr;
	Options.pictfile = stdout;
#ifdef URT
	Options.alpha = TRUE;
	Options.exp_output = FALSE;
#endif
	Options.gamma = GAMMA;
	Options.eyesep = UNSET;
#ifdef LINDA
	Options.workers = WORKERS;
#endif

	Options.totalframes = 1;
	Options.startframe = 0;
	Options.starttime = 0.;
	Options.framelength = 1.;
	Options.shutterspeed = 0.;

	TopMedium.index = DEFAULT_INDEX;
	TopMedium.statten = 1.0;
	NoiseInit();			/* Initialize values for Noise() */

	/*
	 * Top of object definition stack points to the World object.
	 * The World object is always a list.
	 */
	Defstack = GeomStackPush(GeomListCreate(), (GeomList *)NULL);
	Defstack->obj->name = strsave("World");
	/* Initialize surface stack */
	CurSurf = SurfPush((Surface *)NULL, (SurfList *)NULL);
}

/*
 * cleanup()
 *
 * Initialize options/variables not set on command line or in input file.
 * Perform sanity checks on widow dimension, maxdepth, etc.
 */
void
RSCleanup()
{
	extern Light *Lights;
	extern void OpenStatsFile();
	extern FILE *yyin;

	yyin = (FILE *)NULL;	/* mark that we're done reading input */

	if (Options.samples == UNSET)
		Options.samples = DEFSAMPLES;

	if (Options.filterwidth == UNSET) {
		if (Options.gaussian)
			Options.filterwidth = FILTERWIDTH;
		else
			/* Default box filter width of 1.0 */
			Options.filterwidth = 1.0;
	}

	Options.endframe = Options.startframe + Options.totalframes -1;

	OpenStatsFile();

	ViewingSetup();

	if (Options.cutoff.r == UNSET)
		Options.cutoff.r = Options.cutoff.g =
			Options.cutoff.b = DEFCUTOFF;

	/*
	 * Set contrast.
	 */
	if (Options.contrast.r == UNSET) {
		Options.contrast.r = DEFREDCONT;
		Options.contrast.g = DEFGREENCONT;
		Options.contrast.b = DEFBLUECONT;
	}

	/*
	 * Image gamma is inverse of display gamma.
	 */
	if (fabs(Options.gamma) > EPSILON)
		Options.gamma = 1. / Options.gamma;
	else
		Options.gamma = FAR_AWAY;

	if (Options.maxdepth < 0)
		Options.maxdepth = 0;


	LightSetup();
}

void
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
	 * Can't animate camera yet; when possible, this will
	 * need to be much smarter.
	 * RSViewing();
	 */

	/*
	 * Initialize world
	 */
	WorldSetup();
}

/*
 * Initialize non-time-varying goodies.
 */
void
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
	/*
	 * Camera is currently static; initialize it here.
	 */
	RSViewing();
}
