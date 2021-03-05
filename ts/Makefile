ifeq (,$(wildcard ../config.mak))
$(error "../config.mak is not present, run configure !")
endif
include ../config.mak

TS =	daisyduck_fr.ts \

QM = $(TS:.ts=.qm)

EXTRADIST = \
	$(TS) \

.SUFFIXES: .ts .qm

all: release

.ts.qm:
	$(LRELEASEQT5) $< -qm $@

release: $(QM)

update:
	$(LUPDATEQT5) ../src/ -extensions ui,cxx,h -locations relative -ts $(TS)

install: release
	$(INSTALL) -d $(sharedir)/daisyduck/ts
	$(INSTALL) -m 644 $(QM) $(sharedir)/daisyduck/ts

uninstall:
	rm -rf $(sharedir)/daisyduck

clean:
	rm -f *.qm

dist-all:
	cp $(EXTRADIST) Makefile $(DIST)

.PHONY: clean dist-all release update
