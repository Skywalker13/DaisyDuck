ifeq (,$(wildcard ../../config.mak))
$(error "../../config.mak is not present, run configure !")
endif
include ../../config.mak

EXTRADIST = \
	build.sh \
	build-qt.sh \
	config \
	prl2pc.sh \
	qt4_10_optimize-small.diff \
	vlc_10_skip-cache-gen.diff \

all:

dist-all:
	cp $(EXTRADIST) Makefile $(DIST)

.PHONY: clean dist-all
