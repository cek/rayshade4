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
#include "picture.h"
#include "viewing.h"
#include "options.h"
#include "stats.h"

#ifdef URT
unsigned char **outptr;		/* Output buffer */
static int count_rle_rows();
#endif

/*
 * Convert floating-point (0.-1.) to unsigned char (0-255), with no gamma
 * correction.
 */
unsigned char
correct(x)
Float x;
{
	/*
	 * Truncate values < 0 or > 1.
	 */
	if (x < 0)
		return 0;
	if (x > 1.)
		return 255;
	return (unsigned char)(x * 255.);
}

#ifdef URT
/*
 * Open image file and write RLE header.
 */
void
PictureStart(argv)
char **argv;
{
	char gammacom[40];

	if (Options.framenum != Options.startframe) {
		/*
		 * We've been here before;
		 * write a new header and return.
		 */
		rle_put_setup(&rle_dflt_hdr);
		return;
	}
	/*
	 * If Appending, then we know that outfile is valid, 'cause
	 * we've already read its header.
	 */
	if (Options.appending) {
		Options.pictfile = fopen(Options.imgname, "a");
		if (Options.pictfile == (FILE *)0)
			RLerror(RL_PANIC, "Cannot append to %s?!\n",
					Options.imgname);
		rle_dflt_hdr.rle_file = Options.pictfile;
		rle_put_init(&rle_dflt_hdr);
	} else {
		/*
		 * Starting image from scatch.
		 */
		if (Options.imgname) {
			Options.pictfile = fopen(Options.imgname, "w");
			if (Options.pictfile == (FILE *)NULL)
				RLerror(RL_ABORT,"Cannot open %s for writing.",
					Options.imgname);
		} else
			Options.pictfile = stdout;

		rle_dflt_hdr.xmax = Screen.maxx;
		rle_dflt_hdr.ymax = Screen.maxy;
		rle_dflt_hdr.xmin = Screen.minx;
		rle_dflt_hdr.ymin = Screen.miny;
		rle_dflt_hdr.alpha = Options.alpha;
		if (Options.alpha)
			RLE_SET_BIT(rle_dflt_hdr, RLE_ALPHA);
		if (Options.exp_output) {
			RLE_SET_BIT(rle_dflt_hdr, RLE_BLUE + 1);
			rle_dflt_hdr.ncolors = 4;
			rle_putcom("exponential_data", &rle_dflt_hdr);
		}
		else
			rle_dflt_hdr.ncolors = 3;
		/*
	 	 * Document image gamma in RLE comment area.
		 * Options.gamma has been inverted.
	 	 */
		(void)sprintf(gammacom, "display_gamma=%g", 1./Options.gamma);
		rle_putcom(gammacom, &rle_dflt_hdr);
		/*
	 	 * Document command line in RLE history.
	 	 */
		rle_addhist(argv, (rle_hdr *)0, &rle_dflt_hdr);
		rle_dflt_hdr.rle_file = Options.pictfile;
		rle_put_setup(&rle_dflt_hdr);
		/*
	 	 * Flush the header.  If we don't, and LINDA forks off
	 	 * a bunch of workers, strange things will happen (they'll
	 	 * all flush the buffer when they die, and you end up with
	 	 * lots of headers at the end of the file).
	 	 */
		(void)fflush(rle_dflt_hdr.rle_file);
	}

	if (rle_row_alloc(&rle_dflt_hdr, &outptr) < 0)
		RLerror(RL_PANIC, "Unable to allocate image memory.\n");
}

/*
 * Read RLE header to which we are appending in order determine
 * old resolution, window location, and the like.
 */
void
PictureSetWindow()
{
	if (Options.imgname == (char *)NULL)
		RLerror(RL_ABORT,
			"No partially-completed image file specified.\n");

	/*
	 * Open image and read RLE header.
	 */
	Options.pictfile = fopen(Options.imgname, "r");
	if (Options.pictfile == (FILE *)NULL) {
		RLerror(RL_ABORT, "Cannot open image file %s.\n",
					Options.imgname);
	}
	rle_dflt_hdr.rle_file = Options.pictfile;
	rle_get_setup_ok(&rle_dflt_hdr, "rayshade", Options.imgname);

	/*
	 * If user specified a window that does not match what's in
	 * the header, complain.
	if (Screen.minx != UNSET && Screen.minx != rle_dflt_hdr.xmin ||
	    Screen.miny != UNSET && Screen.miny != rle_dflt_hdr.ymin ||
	    Screen.maxx != UNSET && Screen.maxx != rle_dflt_hdr.xmax ||
	    Screen.maxy != UNSET && Screen.maxy != rle_dflt_hdr.ymax)
		RLerror(RL_ADVISE, "Image window: %d - %d, %d - %d.\n",
			rle_dflt_hdr.xmin, rle_dflt_hdr.xmax,
			rle_dflt_hdr.ymin, rle_dflt_hdr.ymax);
	 */
	/*
	 * Set window.
	 */
	Screen.minx = rle_dflt_hdr.xmin;
	Screen.miny = rle_dflt_hdr.ymin;
	Screen.maxx = rle_dflt_hdr.xmax;
	Screen.maxy = rle_dflt_hdr.ymax;

	/*
	 * Set alpha.  Warn the user if the alpha option doesn't reflect
	 * what's already been rendered.
	 */
	if (Options.alpha != rle_dflt_hdr.alpha)
		RLerror(RL_WARN, "Image %s %s an alpha channel.\n",
			Options.imgname,
			rle_dflt_hdr.alpha ? "has" : "does not have");

	Options.alpha = rle_dflt_hdr.alpha;

	/*
	 * Determine number of scanlines written to file.
	 */
	Screen.miny += count_rle_rows(&rle_dflt_hdr);
	if (Screen.miny >= Screen.maxy) {
		fprintf(stderr, "\"%s\" is a complete image.\n",
			Options.imgname);
		exit(0);
	}
	fprintf(Stats.fstats,"Continuing \"%s\" at scanline #%d.\n",
		Options.imgname, Screen.miny);
	(void)fclose(Options.pictfile);
}

static int
count_rle_rows( hdr )
rle_hdr *hdr;
{
	rle_op **raw;
	int *nraw, y, ynext;

	if (rle_raw_alloc( hdr, &raw, &nraw ) < 0)  {
		RLerror(RL_PANIC,
			"Unable to allocate memory in count_rle_rows.\n");
	}

	y = hdr->ymin;
	while ((ynext = rle_getraw( hdr, raw, nraw )) != 32768) {
		y = ynext+1;
		rle_freeraw( hdr, raw, nraw );
	}

	/* Free memory. */
	rle_raw_free( hdr, raw, nraw );

	return y - hdr->ymin;
}

/*
 * Write a scanline of output.
 * "buf" is an array of Color structures of size Screen.xsize.  Each color
 * component is normalized to [0, 1.].
 */
void
PictureWriteLine(buf)
Pixel *buf;
{
	register int i, chan;
	float floats[3];
	rle_pixel pixels[4];

	for(i = 0; i < Screen.xsize; i++) {
		if (!Options.exp_output) {
			/*
			 * Scale colors to fit unsigned char and check for
			 * over/underflow.
			 */
			outptr[0][i] = CORRECT(buf[i].r);
			outptr[1][i] = CORRECT(buf[i].g);
			outptr[2][i] = CORRECT(buf[i].b);
		} else {
			/*
			 * Convert 3 floats to 4 unsigned chars for
			 * 'exponential_data' RLE file.
			 */
			floats[0] = GAMMACORRECT(buf[i].r);
			floats[1] = GAMMACORRECT(buf[i].g);
			floats[2] = GAMMACORRECT(buf[i].b);
			float_to_exp( 3, floats, pixels );
			for (chan = 0; chan <= 3; chan++)
				outptr[chan][i] = pixels[chan];
		}
		if (Options.alpha)
			/*
			 * Don't gamma correct alpha channel.
			 */
			outptr[-1][i] = correct(buf[i].alpha);
	}
	rle_putrow(outptr, Screen.xsize, &rle_dflt_hdr);
}

/*
 * End the frame.
 */
void
PictureFrameEnd()
{
	rle_puteof(&rle_dflt_hdr);
}

/*
 * Close image file.
 */
void
PictureEnd()
{
	(void)fclose(Options.pictfile);
}

#else /* !URT */
void
PictureStart(argv)
char **argv;
{
	if (Options.imgname) {
		Options.pictfile = fopen(Options.imgname, "w");
		if (Options.pictfile == (FILE *)NULL)
			RLerror(RL_ABORT, "Cannot open %s for writing.",
				Options.imgname);
	} else
		Options.pictfile = stdout;

	fprintf(Options.pictfile,"%d %d\n",Screen.xsize, Screen.ysize);

	(void)fflush(Options.pictfile);
}

void
PictureWriteLine(buf)
Pixel *buf;
{
	register int i;

	for (i = 0; i < Screen.xsize; i++) {
		(void)fputc((int)CORRECT(buf[i].r), Options.pictfile);
		(void)fputc((int)CORRECT(buf[i].g), Options.pictfile);
		(void)fputc((int)CORRECT(buf[i].b), Options.pictfile);
	}
	(void)fflush(Options.pictfile);
}

void
PictureFrameEnd()
{
	/*
	 * Don't do anything -- generic format has no end-of-image marker.
	 */
}

void
PictureEnd()
{
	(void)fclose(Options.pictfile);
}

#endif /* URT */
