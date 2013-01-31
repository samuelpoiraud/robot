## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = eve

eve_SOURCES = \
	main.c \
	IHM_support.c IHM_support.h \
	IHM_interface.c IHM_interface.h \
	IHM_callbacks.c IHM_callbacks.h

eve_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

