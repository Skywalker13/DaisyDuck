ifeq (,$(wildcard ../config.mak))
$(error "../config.mak is not present, run configure !")
endif
include ../config.mak

EXTRADIST = \
	controls.svg \
	daisyduck.svg \

all:

dist-all:
	cp $(EXTRADIST) Makefile $(DIST)

.PHONY: clean dist-all
