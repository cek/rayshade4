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
echo "Extracting etc/rsconvert/Makefile (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >Makefile <<!GROK!THIS!
OPTIMIZE = $optimize
CCFLAGS = $ccflags $large
CC = $cc
MKDEP = $mkdep
YACC = $yacc
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>Makefile <<'!NO!SUBS!'
LIBRAYDIR = ../../libray
INCLUDE = -I$(LIBRAYDIR) -I../../
CFLAGS = $(CCFLAGS) $(INCLUDE) $(OPTIMIZE)
LIBS =
YFLAGS = -d

CFILES = main.c yacc.c lex.c
OBJS = $(CFILES:.c=.o)
DEPENDSRC = main.c yacc.y lex.l

rsconvert: $(OBJS)
	$(CC) $(CFLAGS) -o rsconvert $(OBJS) $(LIBS)

depend:
	(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	 $(MKDEP) $(DEPENDSRC) | sed 's/: \.\//: /; /\/usr\/include/d' \
	) >Makefile.new
	cp Makefile Makefile.bak
	cp Makefile.new Makefile
	rm -f Makefile.new

clean:
	/bin/rm -f y.tab.h $(OBJS) lex.c

# DO NOT DELETE THIS LINE
!NO!SUBS!
chmod 755 Makefile
$eunicefix Makefile
