ifeq (,$(wildcard ../../config.mak))
$(error "../../config.mak is not present, run configure !")
endif
include ../../config.mak

EXTRADIST = \
	build.sh \
	config \
	vlc_10_skip-cache-gen.diff \

all:

dist-all:
	cp $(EXTRADIST) Makefile $(DIST)

.PHONY: clean dist-all
