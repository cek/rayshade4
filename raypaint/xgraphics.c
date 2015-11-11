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

#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

char *display_name = NULL;
Display *dpy = NULL;
Screen *scrn;
Visual *vis;
Colormap cmap;
GC gc;
Window win;
int screen_height;

unsigned long graymap[256];
int max_colors;
double one_over_gamma = 0.4;

/*****************************************************************
 * Sets the gray color map for the device.  A 2.5 gamma map is used
 * by default.
 */
setup_gray_gamma_map()
{
    int cc, col;
    int gamma_color;

    XColor xcolor;

    /* Use the default colormap if possible. */
    if ( vis == DefaultVisualOfScreen( scrn ) )
        cmap = DefaultColormapOfScreen( scrn );
    else
        cmap = XCreateColormap( dpy, RootWindowOfScreen( scrn ),
                               vis, AllocNone );

    /* try to share with current colormap */
    for (max_colors = 256; max_colors >= 16; max_colors = max_colors >> 1) {
        xcolor.flags= DoRed | DoGreen | DoBlue;
        for(col=0; col < max_colors; col++) {
            gamma_color = (pow((float) col / (float) max_colors,
                               one_over_gamma) * 65536);
            xcolor.red= gamma_color;
            xcolor.green= gamma_color;
            xcolor.blue= gamma_color;
            if (!XAllocColor(dpy, cmap, &xcolor)) {
                for (cc=0; cc < col; cc++)
                    XFreeColors(dpy, cmap, &graymap[cc], 1, 0);
                col = 0;
                break;
            }
            graymap[col] = xcolor.pixel;
        }
        if (col)
            return;
    }

    /* use new map */
    cmap = XCreateColormap( dpy, RootWindowOfScreen( scrn ),
                           vis, AllocNone );
    if (cmap == NULL)  {
        fprintf(stderr, "Could not create color map for visual\n");
        exit(-2);
    }
    for(cc=0; cc < 256; cc++)
        if (!XAllocColorCells(dpy, cmap, False, NULL, 0, &graymap[cc], 1))
            break;
    max_colors = cc;

    xcolor.flags= DoRed | DoGreen | DoBlue;
    for(col=0; col < max_colors; col++) {
        xcolor.pixel= graymap[col];
        gamma_color = (pow((float) col / (float) max_colors,
                           one_over_gamma) * 65536);
        xcolor.red= gamma_color;
        xcolor.green= gamma_color;
        xcolor.blue= gamma_color;
        XStoreColor(dpy, cmap, &xcolor);
    }
}

GraphicsInit(xsize, ysize, name)
int xsize, ysize;
char *name;
{
    int win_size;
    XSetWindowAttributes attrs;
    XSizeHints sh;

    /* Open the display. */
    if ( ! dpy )
    {
        XVisualInfo vis_temp, *vis_list, *max_vis;
        int n_ret, i;

        dpy = XOpenDisplay( display_name );
        if ( ! dpy )
        {
            fprintf( stderr, "rayview: Can't open display %s\n",
                     XDisplayName( display_name ) );
            exit(1);
        }

        /* Get a PseudoColor visual that has the maximum number of planes. */
        vis_temp.class = PseudoColor;
        vis_list = XGetVisualInfo( dpy, VisualClassMask, &vis_temp, &n_ret );
        if ( n_ret == 0 )
        {
            fprintf(stderr,
                    "Can't find any PseudoColor visual from display %s.\n",
                    XDisplayName( display_name ));
            exit(1);
        }
        max_vis = &vis_list[0];
        for ( i = 1; i < n_ret; i++ )
        {
            if ( max_vis->depth < vis_list[i].depth )
                max_vis = &vis_list[i];
        }
        vis = max_vis->visual;
        scrn = ScreenOfDisplay( dpy, max_vis->screen );
        gc = DefaultGCOfScreen( scrn );

        setup_gray_gamma_map();

        XFree( (char *)vis_list );
    }

    screen_height = ysize;

    attrs.backing_store = Always;
    attrs.colormap = cmap;
    attrs.event_mask = ExposureMask;
    attrs.background_pixel = BlackPixelOfScreen(scrn);
    attrs.border_pixel = WhitePixelOfScreen(scrn);

    win = XCreateWindow( dpy, RootWindowOfScreen( scrn ),
                         0, 0, xsize, ysize, 2,
                         0, 0, vis,
                         CWBackingStore | CWColormap | CWEventMask |
                         CWBackPixel | CWBorderPixel,
                         &attrs );

    sh.flags = PSize | PMinSize | PMaxSize;
    sh.width = sh.min_width = sh.max_width = xsize;
    sh.height = sh.min_height = sh.max_height = ysize;
    XSetStandardProperties( dpy, win, name, name, None, NULL, 0, &sh );

    XMapWindow( dpy, win );

    XFlush( dpy );
}

/*
 * Draw the pixel at (xp, yp) in the color given by the rgb-triple,
 * 0 indicating 0 intensity, 255 max intensity.
 */
GraphicsDrawPixel(xp, yp, color)
int xp, yp;
unsigned char color[3];
{
    float bwvalue;
    int val;

    bwvalue = ( 0.35*color[0] + 0.55*color[1] + 0.10*color[2] ) / 256.0;
    val = (int) ( bwvalue * max_colors );
    XSetForeground( dpy, gc, graymap[val] );
    XFillRectangle( dpy, win, gc, xp, (screen_height - (yp + 1)),
                    1, 1 );
}

/*
 * Draw the rect with lower left corner (xp, yp) and upper right
 * corner (xp+ys, yp+ys).  The colors of the l-l, l-r, u-r, and u-l
 * corners are given as arrays of unsigned chars as above.
 */
GraphicsDrawRectangle(xp, yp, xs, ys, ll, lr, ur, ul)
int xp, yp, xs, ys;
unsigned char ll[3], lr[3], ur[3], ul[3];
{
    float bwvalue;
    int val;

    bwvalue = ( 0.35*ll[0] + 0.55*ll[1] + 0.10*ll[2] ) / 256.0;
    val = (int) ( bwvalue * max_colors );
    XSetForeground( dpy, gc, graymap[val] );
    XFillRectangle( dpy, win, gc, xp, (screen_height - (yp + ys + 1)),
                    xs+1, ys+1 );
    XFlush( dpy );
}

GraphicsLeftMouseEvent()
{
    Window root_ret, child_ret;
    int rx, ry, wx, wy;
    unsigned int mask;

    if (XQueryPointer(dpy, win, &root_ret, &child_ret,
                      &rx, &ry, &wx, &wy, &mask)) {
        return mask & Button1Mask;
    }
    else
        return 0;
}

GraphicsMiddleMouseEvent()
{
    Window root_ret, child_ret;
    int rx, ry, wx, wy;
    unsigned int mask;

    if (XQueryPointer(dpy, win, &root_ret, &child_ret,
                      &rx, &ry, &wx, &wy, &mask)) {
        return mask & Button2Mask;
    }
    else
        return 0;
}

GraphicsRightMouseEvent()
{
    Window root_ret, child_ret;
    int rx, ry, wx, wy;
    unsigned int mask;

    if (XQueryPointer(dpy, win, &root_ret, &child_ret,
                      &rx, &ry, &wx, &wy, &mask)) {
        return mask & Button3Mask;
    }
    else
        return 0;
}

GraphicsGetMousePos(x, y)
int *x, *y;
{
    Window root_ret, child_ret;
    int rx, ry, wx, wy;
    unsigned int mask;

    if (XQueryPointer(dpy, win, &root_ret, &child_ret,
                      &rx, &ry, &wx, &wy, &mask)) {
        *x = wx;
        *y = screen_height - wy - 1;
    }
    else {
        *x = 0;
        *y = 0;
    }
}

GraphicsRedraw()
{
        XEvent event;
        if (XCheckTypedEvent(dpy, Expose, &event)) {
                XSetForeground( dpy, gc, graymap[0] );
                XFillRectangle( dpy, win, gc, event.xexpose.x, event.xexpose.y,
                    event.xexpose.width, event.xexpose.height );
                XFlush( dpy );
                return 1;
        }
        else
                return 0;
}

