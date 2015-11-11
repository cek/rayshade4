#
# Makefile for libray
# C. Kolb 1/91
#
# $Id: Makefile,v 4.0.1.1 91/10/10 18:33:41 cek Exp Locker: cek $
#
MAKE = make -$(MAKEFLAGS)
STUFF = libcommon libobj libimage liblight libsurf libtext
SHELL = /bin/sh

default:
	for i in $(STUFF); do \
		(cd $$i && $(MAKE)); \
	done

#
# Have to keep clean/depend separete so
# Configure will work
#

depend:
	for i in $(STUFF); do \
		(cd $$i && $(MAKE) depend); \
	done

clean:
	for i in $(STUFF); do \
		(cd $$i && $(MAKE) clean); \
	done
	rm -f libray.a
