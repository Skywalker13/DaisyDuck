ifeq (,$(wildcard config.mak))
$(error "config.mak is not present, run configure !")
endif
include config.mak

DISTFILE = daisyduck-$(VERSION).tar.bz2

EXTRADIST = \
	configure \
	COPYING \
	README \
	TODO \

SUBDIRS = \
	DOCS \
	extras \
	extras/nsis \
	extras/nsis/configs \
	extras/nsis/languages \
	images \
	src \
	ts \

all: daisyduck translations docs

config.mak: configure
	@echo "############################################################"
	@echo "####### Please run ./configure again - it's changed! #######"
	@echo "############################################################"

images:
	$(MAKE) -C images

daisyduck: images
	$(MAKE) -C src

docs:
	$(MAKE) -C DOCS

docs-clean:
	$(MAKE) -C DOCS clean

clean:
	$(MAKE) -C images clean
	$(MAKE) -C ts clean
	$(MAKE) -C src clean

distclean: clean docs-clean
	rm -f config.log
	rm -f config.mak
	rm -f $(DISTFILE)

install: install-daisyduck install-translations

install-daisyduck: daisyduck
	$(MAKE) -C src install

install-translations: translations
	$(MAKE) -C ts install

uninstall: uninstall-daisyduck uninstall-translations

uninstall-daisyduck:
	$(MAKE) -C src uninstall

uninstall-translations:
	$(MAKE) -C ts uninstall

translations:
	$(MAKE) -C ts

update-translations: clean
	$(MAKE) -C ts update

win32-installer:
	$(MAKE) -C extras/nsis clean
	$(MAKE) -C extras/nsis $(INSTALLER_CFG) nsis

.PHONY: *clean *install* docs daisyduck images *translations

dist:
	-$(RM) $(DISTFILE)
	dist=$(shell pwd)/daisyduck-$(VERSION) && \
	for subdir in . $(SUBDIRS); do \
	  mkdir -p "$$dist/$$subdir"; \
	  $(MAKE) -C $$subdir dist-all DIST="$$dist/$$subdir"; \
	done && \
	tar cjf $(DISTFILE) daisyduck-$(VERSION)
	-$(RM) -rf daisyduck-$(VERSION)

dist-all:
	cp $(EXTRADIST) Makefile $(DIST)

.PHONY: dist dist-all
