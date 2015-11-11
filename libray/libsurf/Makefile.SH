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
echo "Extracting libsurf/Makefile (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >Makefile <<!GROK!THIS!
OPTIMIZE = $optimize
CCFLAGS = $ccflags $large
RANLIB = $ranlib
CC = $cc
MKDEP = $mkdep

!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>Makefile <<'!NO!SUBS!'
LIB = ../libray.a
INCLUDE = -I.. -I../..
CFLAGS = $(INCLUDE) $(CCFLAGS) $(OPTIMIZE)
SHELL = /bin/sh

CFILES = atmosphere.c fog.c fogdeck.c mist.c surface.c surfshade.c
OFILES = $(CFILES:.c=.o)

$(LIB): $(OFILES)
	ar cur $(LIB) $(OFILES)
	$(RANLIB) $(LIB)

clean:
	rm -f $(OFILES)

depend:
	(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	 $(MKDEP) $(CFILES) | sed 's/: \.\//: /; /\/usr\/include/d' \
	) >Makefile.new
	cp Makefile Makefile.bak
	cp Makefile.new Makefile
	rm -f Makefile.new

# DO NOT DELETE THIS LINE
!NO!SUBS!
chmod 755 Makefile
$eunicefix Makefile
