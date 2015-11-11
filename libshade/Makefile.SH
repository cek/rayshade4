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
# Makefile for rayshade.
#
# Craig Kolb
#
# \$Id: Makefile.SH,v 4.0 91/07/17 14:44:47 kolb Exp Locker: kolb $
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
CC = $cc
MKDEP = $mkdep
YACC = $yacc
RANLIB = $ranlib

!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>Makefile <<'!NO!SUBS!'
LIBRAYDIR = ../libray
INCLUDE = -I.. -I$(LIBRAYDIR)
YFLAGS = -d

CFLAGS = $(CCFLAGS) $(URTINC) $(INCLUDE) $(OPTIMIZE)
SHELL = /bin/sh

LIB = libshade.a

SUPPORT_C =	builtin.c symtab.c misc.c lightdef.c objdef.c options.c \
		stats.c surfdef.c

SUPPORT_H =	../config.h datatypes.h funcdefs.h \
		../patchlevel.h rayshade.h

PARSE_C =	yacc.c lex.c

DRIVE_C =	setup.c viewing.c shade.c picture.c

DRIVE_H =	y.tab.h defaults.h viewing.h raytrace.h picture.h

SUPPORT_O = $(SUPPORT_C:.c=.o)

PARSE_O = $(PARSE_C:.c=.o)

DRIVE_O = $(DRIVE_C:.c=.o)

HFILES = $(DRIVE_H) $(SUPPORT_H)

CFILES = $(DRIVE_C) $(PARSE_C) $(SUPPORT_C)

SHFILES = Makefile.SH

OFILES = $(DRIVE_O) $(PARSE_O) $(SUPPORT_O)

DEPENDSRC = $(DRIVE_C) lex.l yacc.y $(SUPPORT_C)

$(LIB): $(OFILES)
	ar cur $(LIB) $(OFILES)
	$(RANLIB) $(LIB)

#
# Uncomment the following rule if using Linda.
#
#raytrace.lo: raytrace.cl
#	$(LCC) $(CFLAGS) -c raytrace.cl

#
# End of configuration section
#
install:	rayshade
		mv rayshade $(BINDIR)/rayshade

clean:
	rm -f $(OFILES) core libshade.a

realclean:
	rm -f $(OFILES) core libshade.a y.tab.h
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
