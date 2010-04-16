ifeq (,$(wildcard ../config.mak))
$(error "../config.mak is not present, run configure !")
endif
include ../config.mak

EXTRADIST = \
	daisyduck.png \
	daisyduck_2.png \
	daisyduck_3.png \
	exit.png \
	images.qrc \
	online.png \
	open.png \
	player.png \
	player_nn.png \
	player_ns.png \
	player_pn.png \
	player_ps.png \
	settings.png \
	welcome.png \

all: qrc

../src/qrc_images.cxx: images.qrc
	$(RCCQT4) -name images images.qrc -o ../src/qrc_images.cxx

qrc: ../src/qrc_images.cxx

clean:
	rm -f ../src/qrc_images.cxx

dist-all:
	cp $(EXTRADIST) Makefile $(DIST)

.PHONY: clean dist-all qrc
