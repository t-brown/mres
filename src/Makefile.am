#
# Makefile for kres
#

bin_PROGRAMS = kres
EXTRA_DIST   = kres.1

AUTOMAKE_OPTIONS = nostdinc
AM_CFLAGS        =  -I.                 \
                    $(HDF5_CFLAGS)      \
                    $(HDF5_CPPFLAGS)
localedir        = $(datadir)/locale
DEFS             = -DLOCALEDIR=\"$(localedir)\" @DEFS@

kres_LDADD       = $(LIBINTL)           \
                   $(HDF5_LDFLAGS)      \
                   $(HDF5_LIBS)

kres_SOURCES     = atts.h               \
                  args.h      args.c    \
                  main.c                \
                  events.h    events.c  \
                  mem.h       mem.c     \
                  projects.h  projects.c



