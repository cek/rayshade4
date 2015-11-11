case $CONFIG in
'')
    if test ! -f config.sh; then
	ln ../config.sh . || \
	ln ../../config.sh . || \
	ln ../../../config.sh . || \
	(echo "Can't find config.sh."; exit 1)
    fi
    . config.sh
    ;;
esac
: This forces SH files to create target in same directory as SH file.
: This is so that make depend always knows where to find SH derivatives.
case "$0" in
*/*) cd `expr X$0 : 'X\(.*\)/'` ;;
esac
echo "Extracting Makefile (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >Makefile <<!GROK!THIS!
#
# Makefile for rayview 
#
# Craig Kolb
#
# \$Id: Makefile.SH,v 4.0 91/07/17 14:55:03 kolb Exp Locker: kolb $
#
# Bin directory
#
BINDIR = $bin 
#
# If you are using LINDA, add -DLINDA
# If you are running on a Multimax, add -DMULTIMAX -DSHAREDMEM
# Be sure to add any necessary floating point hardware switches.
# 
OPTIMIZE = $optimize
URTINC = $urtinc
CCFLAGS = $ccflags $large
URTLIB = $urtlib
LDFLAGS = $libs $ldflags
CC = $cc
MKDEP = $mkdep
YACC = $yacc

!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>Makefile <<'!NO!SUBS!'
LIBRAYDIR = ../libray
LIBSHADEDIR = ../libshade
INCLUDE = -I$(LIBRAYDIR) -I$(LIBSHADEDIR) -I..
YFLAGS = -d

#
# If using GL on an SGI machine, use:
GRAPHICSLIBS = -lsphere -lgl_s
SPHERELIB = -DSPHERELIB

#
# If using GL on an RS6000, use:
#GRAPHICSLIBS = -lgl
#SPHERELIB=

LIBRAY = $(LIBRAYDIR)/libray.a
LIBSHADE = $(LIBSHADEDIR)/libshade.a

CFLAGS = $(CCFLAGS) $(SPHERELIB) $(URTINC) $(INCLUDE) $(OPTIMIZE)
SHELL = /bin/sh

#
# If you have a fast malloc library, use it (e.g., -lmalloc on MIPS machines)
#
LIBS = $(LIBSHADE) $(LIBRAY) $(URTLIB)

DRIVE_C =	main.c glmethods.c spheregen.c version.c

DRIVE_O = $(DRIVE_C:.c=.o)

CFILES = $(DRIVE_C)

SHFILES = Makefile.SH

OBJ = $(DRIVE_O)

DEPENDSRC = $(DRIVE_C)

#
# Change $(CC) below to $(LCC) if using Linda.
#
rayview: $(OBJ) $(LIBS)
	$(CC) $(OPTIMIZE) -o rayview $(OBJ) $(GRAPHICSLIBS) $(LIBS) $(LDFLAGS)

#
# Uncomment the following rule if using Linda.
#
#raytrace.lo: raytrace.cl
#	$(LCC) $(CFLAGS) -c raytrace.cl

#
# End of configuration section
#
install:	rayview	
		mv rayview $(BINDIR)/rayview

clean:
	rm -f $(OBJ) core

realclean:
	rm -f $(OBJ) core y.tab.h
	rm -f *.orig Makefile

lint:
	lint $(CFLAGS) -x $(CFILES) -lm

tags:
	ctags -t $(CFILES)

depend:
	(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	 $(MKDEP) $(DEPENDSRC) | sed 's/: \.\//: /; /\/usr\/include/d' \
	) >Makefile.new
	cp Makefile Makefile.bak
	cp Makefile.new Makefile
	rm -f Makefile.new


# DO NOT DELETE THIS LINE
!NO!SUBS!
chmod 755 Makefile
$eunicefix Makefile
