# Makefile for Makefile.pdlibbuilder.
#

lib.name := windowing

common.sources = src/windowing.c

bartlett~.class.sources = src/bartlett~.c
blackman~.class.sources = src/blackman~.c
connes~.class.sources   = src/connes~.c
cosine~.class.sources   = src/cosine~.c
gaussian~.class.sources = src/gaussian~.c
hamming~.class.sources  = src/hamming~.c
hann~.class.sources     = src/hann~.c
kaiser~.class.sources   = src/kaiser~.c
lanczos~.class.sources  = src/lanczos~.c
welch~.class.sources    = src/welch~.c

datafiles = \
$(wildcard help/*-help.pd) \
help/dspSwitch~.pd \
LICENSE.txt \
README.md \
README.txt \
src/mconf.h \
windowing-meta.pd


PDLIBBUILDER_DIR=pd-lib-builder/
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
