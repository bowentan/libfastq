AM_LDFLAGS =
AM_CFLAGS = -g -I$(abs_top_srcdir)/include

lib_LTLIBRARIES = libfastq.la
libfastq_la_SOURCES = quality_system.c barcode.c fastq.c
libfastq_la_LDFLAGS = -version-info @LIBRARY_VERSION@ -release-info @RELEASE_VERSION@
libfastq_la_LIBADD = -lz
