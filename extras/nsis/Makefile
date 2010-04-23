ifeq (,$(wildcard ../../config.mak))
$(error "../../config.mak is not present, run configure !")
endif
include ../../config.mak

SEL = bsr

NSI = daisyduck.nsi

EXTRADIST = \
	daisyduck.ico \
	license.txt \
	side.bmp \
	$(NSI) \

all: nsis

bsr:
	cp configs/bsr.daisyduck.conf daisyduck.conf

nsis: $(SEL)
	makensis $(NSI)

clean:
	rm -f daisyduck.conf
	rm -f daisyduck-win32-installer-*.exe

dist-all:
	cp $(EXTRADIST) Makefile $(DIST)

.PHONY: clean dist-all nsis
