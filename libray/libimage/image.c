/*
 * Copyright (C) 1989-2015, Rod G. Bogart, Craig E. Kolb
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
#include <stdio.h>
#include "libcommon/common.h"
#include "image.h"
#ifdef URT
#include "rle.h"
#endif

Image *image_list = NULL;		/* Linked list of images */

Image *
ImageCreate(filename)
char *filename;
{
	Image *new;

	new = (Image *)Malloc(sizeof(Image));
	new->filename = strsave(filename);
	new->width = 0;
	new->height = 0;
	new->chan = 0;
	new->data = NULL;
	new->next = image_list;
	image_list = new;
	return new;
}

Image *
ImageFind(name)
char *name;
{
	Image *im;

	for (im = image_list; im; im = im->next) {
		if (strcmp(im->filename, name) == 0)
			return im;
	}

	return (Image *)NULL;
}

#ifdef URT
Image *
ImageRead(filename)
char *filename;
{
	FILE *fp;
	int i, y, chan;
	rle_hdr in_hdr;
	Image *image;
	rle_pixel **inrows;

	/*
	 * See if image has been read before.
	 */
	image = ImageFind(filename);
	if (image)
		return image;

	fp = fopen(filename, "r");
	if (fp == (FILE *)NULL) {
		RLerror(RL_ABORT, "Cannot open RLE file %s.\n",filename);
		return (Image *)NULL;
	}

	in_hdr.rle_file = fp;
   
	/* Try to open the RLE file */
	if (rle_get_setup(&in_hdr) < 0) {
		RLerror(RL_ABORT, "Error reading header of %s\n", filename);
		return (Image *)NULL;
	}

	/*
	 * Read new image
	 */
	image = ImageCreate(filename);

	in_hdr.xmax -= in_hdr.xmin;
	in_hdr.xmin = 0;
	image->width = in_hdr.xmax + 1;
	image->height = in_hdr.ymax - in_hdr.ymin + 1;
	image->chan = in_hdr.ncolors;
	image->has_alpha = in_hdr.alpha ? 1 : 0;
	image->totalchan = image->chan + image->has_alpha;
	image->chansize = image->width * image->height;

	image->data = (unsigned char *) Malloc(
		image->chansize * image->totalchan * sizeof(unsigned char));

	/*
	 * Allocate array of pointers to pass to rle_getrow.
	 */
	inrows = (rle_pixel **)Malloc(image->totalchan *
			sizeof(rle_pixel *));
	/*
	 * Set inrows to point to appropriate initial location in image.
	 */
	inrows[0] = (rle_pixel *)image->data;
	for (i = 1; i < image->totalchan; i++)
		inrows[i] = inrows[i-1] + image->chansize;
	if (image->has_alpha)
		/* Alpha channel lives in channel -1 */
		inrows++;

	/* Read the image */
	for ( y = 0; y < image->height; y++ ) {
		rle_getrow( &in_hdr, inrows );
		/*
		 * Update inrows to point to next scanline for
		 * each channel.
		 */
		for (i = 0; i < image->chan; i++)
			inrows[i] += image->width;
		if (image->has_alpha)
			inrows[-1] += image->width;
	}

	(void)fclose(fp);
	return image;
}

#else /* !URT */

Image *
ImageRead(filename)
char *filename;
{
	FILE *fp;
	char buf[80];
	Image *image;
	int y, x;
	unsigned char *rbuf, *gbuf, *bbuf;

	image = ImageFind(filename);
	if (image)
		return image;

	fp = fopen(filename, "r");
	if (fp == (FILE *)NULL) {
		RLerror(RL_ABORT, "Cannot open image file %s.\n",filename);
		return (Image *)NULL;
	}

	image = ImageCreate(filename);
	/*
	 * Read image header.
	 */
	if (fgets(buf, 100, fp) == (char *)NULL ||
	    sscanf(buf, "%d %d\n", &image->width, &image->height) != 2) {
		RLerror(RL_ABORT, "Cannot read header of image file %s.\n",
			filename);
		fclose(fp);
		return (Image *)NULL;
	}
	/*
	 * Generic image files always have 3 channels, no alpha.
	 */
	image->chan = image->totalchan = 3;
	image->has_alpha = 0;
	image->chansize = image->width * image->height;

	image->data = (unsigned char *) Malloc(
		image->chansize * image->totalchan * sizeof(unsigned char));

	rbuf = image->data;
	gbuf = &image->data[image->chansize];
	bbuf = &image->data[image->chansize+image->chansize];
	for (y = 0; y < image->height; y++ ) {
		for (x = 0; x < image->width; x++) {
			*(rbuf++) = getc(fp);
			*(gbuf++) = getc(fp);
			*(bbuf++) = getc(fp);
			if (feof(fp)) {
				RLerror(RL_ABORT,
				"Error reading image %s\n",filename);
				fclose(fp);
				return (Image *)NULL;
			}
		}
	}

	(void)fclose(fp);
	return image;
}
#endif

void
ImageIndex(img, ix, iy, fx, fy, smooth, outval)
Image *img;
int ix, iy, smooth;
Float fx, fy;
Float outval[4];
{
	int xplus, yplus, chan, offset;
	Float x0y0, x1y0, x0y1, x1y1;
	unsigned char *data;

	if (smooth) {
		/*
		 * bi-linear interp of four pixels.  Note this blends
		 * the top with the bottom, and the left with the right.
		 */
		if (ix == img->width - 1)
			xplus = 1 - img->width;
		else 
			xplus = 1;
		if (iy == img->height - 1)
			yplus = (1 - img->height) * img->width;
		else
			yplus = img->width;
		data = img->data;
		/* compute offset into first channel */
		offset = ix + iy * img->width;
		for (chan = 0; chan < img->totalchan; chan++) {
			x0y0 = (Float)data[offset] / 255.0;
			x1y0 = (Float)data[offset+xplus] / 255.0;
			x0y1 = (Float)data[offset+yplus] / 255.0;
			x1y1 = (Float)data[offset+xplus+yplus]/255.0;
			outval[chan] = (x0y0*(1.0-fx)*(1.0-fy) +
					x1y0*(fx)*(1.0-fy) +
					x0y1*(1.0-fx)*(fy) +  x1y1*(fx)*(fy));
			/* Make offset point to next channel */
			offset += img->chansize;
		}
	} else {
		/*
		 * Hard edged image pixels (rectangles)
		 * Compute offset into first channel
		 */
		offset = ix + iy * img->width;
		for (chan = 0; chan < img->totalchan; chan++) {
			outval[chan] = (Float)img->data[offset]/255.0;
			/* Make offset point to next channel */
			offset += img->chansize;
		}
	}
}
