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
#ifndef OPTIONS_H
#define OPTIONS_H

/*
 * Constants for Stereo mode
 */
#define LEFT		1
#define RIGHT		2

/*
 * Options
 */
typedef struct RSOptions {
	int	stereo,			/* Stereo mode? */
		verbose,		/* Babbling mode? */
		quiet,			/* Don't complain? */
		jitter,			/* use jittered sampling? */
		samples,		/* Sqrt of # of samples */
		maxdepth,		/* Maximum ray tree depth */
		report_freq,		/* Frequency, in lines, of report */
		no_shadows,		/* Trace shadow rays? */
		shadowtransp,		/* ... through transparent objects? */
		cache,			/* Cache shadowing info? */
		appending,		/* Append to image file? */
		resolution_set,		/* resolution set on command line */
		contrast_set,		/* contrast overridden ... */
		samples_set,		/* samples overridden ... */
		cutoff_set,		/* cutoff ... */
		maxdepth_set,		/* adaptive depth ... */
		window_set,		/* subwindow ... */
		crop_set,		/* crop window ... */
		freq_set,		/* report frequency ... */
		jitter_set,		/* use jittering */
		eyesep_set,		/* eye separation ... */
		csg,			/* CSG object someplace in world */
		flipnorm,		/* flip normals of polygonal objs */
		samplemap,		/* output sample map? */
		gaussian,		/* Use gaussian pixel filter? */
		framenum,		/* current frame number */
		startframe,		/* Starting frame number. */
		endframe,		/* ending frame number */
		totalframes,		/* total # of frames */
		totalframes_set,	/* set on command line? */
		cpp;			/* run CPP? */
#ifdef URT
	int	alpha;			/* Write alpha channel? */
	int	exp_output;		/* Write exponential RLE file? */
#endif
	Float	eyesep,			/* Eye separation (for Stereo mode) */
		gamma,			/* Gamma value (0 == no correction) */
		starttime,		/* Think about it ... */
		shutterspeed,		/* time shutter is open */
		framestart,		/* start time of the current frame */
		framelength,		/* length of the current frame */
		filterwidth;		/* Pixel filter width. */
	Color	contrast,		/* Max. allowable contrast */
		cutoff,			/* Ray tree depth control */
		ambient;		/* Ambient light multiplier */
	char	*progname,		/* argv[0] */
		*statsname,		/* Name of stats file. */
		*imgname,		/* Name of output image file */
		*inputname,		/* Name of input file, NULL == stdin */
		*cppargs;		/* arguments to pass to cpp */
	int	window[2][2];		/* Subwindow corners */
	Float	crop[2][2];		/* Crop window, lo/hi normalized */
#ifdef LINDA
	int	workers,		/* # of worker processes */
		workernum,		/* worker #, 0 == supervisor */
		verbose_worker;		/* Babble while you work? */
#endif
	FILE	*pictfile;		/* output file pointer */
} RSOptions;

extern RSOptions Options;
extern void OptionsList(), OptionsSet();

#endif /* OPTIONS_H */
