ifeq (,$(wildcard ../config.mak))
$(error "../config.mak is not present, run configure !")
endif
include ../config.mak

DAISYDUCK = daisyduck

APPS_CPPFLAGS = $(CFG_CPPFLAGS) $(CPPFLAGS)
APPS_LDFLAGS = $(CFG_LDFLAGS) $(LDFLAGS)

SRCS = \
	Config.cxx \
	ConfigAccess.cxx \
	ConfigCustom.cxx \
	DaisyDuck.cxx \
	Input.cxx \
	OnlineBook.cxx \
	main.cxx \

SRCS_QRC = \
	qrc_images.cxx \

H_MOC = \
	ConfigAccess.h \
	ConfigCustom.h \
	DaisyDuck.h \
	Input.h \
	OnlineBook.h \

SRCS_MOC = \
	ConfigAccess_moc.cxx \
	ConfigCustom_moc.cxx \
	DaisyDuck_moc.cxx \
	Input_moc.cxx \
	OnlineBook_moc.cxx \

UI = \
	dialogAbout.ui \
	dialogConfigAccess.ui \
	dialogConfigCustom.ui \
	dialogInput.ui \
	dialogOnlineBook.ui \
	winMain.ui \

MOCS = $(H_MOC:.h=_moc.cxx)
UIS  = $(UI:.ui=.h)
OBJS = $(SRCS:.cxx=.o) $(SRCS_MOC:.cxx=.o) $(SRCS_QRC:.cxx=.o)

MANS = $(DAISYDUCK).1

DESKTOP = $(DAISYDUCK).desktop
ICON = ../images/$(DAISYDUCK).png

EXTRADIST = \
	common.h \
	Config.h \
	$(UI) \
	$(H_MOC) \
	$(MANS) \
	$(DESKTOP) \

.SUFFIXES: .ui .h _moc.cxx .cxx .o

all: apps

.cxx.o:
	$(CXX) -c $(OPTFLAGS) $(CXXFLAGS) $(APPS_CPPFLAGS) -o $@ $<

.h_moc.cxx:
	$(MOCQT5) -o $@ $<

.ui.h:
	$(UICQT5) -o $@ $<

$(DAISYDUCK): $(OBJS)
	$(CXX) $(OBJS) $(EXTRALIBS) $(APPS_LDFLAGS) -o $(DAISYDUCK)

apps-dep: $(UIS) $(SRCS_MOC)
	$(CXX) -MM $(CXXFLAGS) $(APPS_CPPFLAGS) $(SRCS) $(SRCS_QRC) 1>.depend

apps-all: $(DAISYDUCK)

apps: apps-dep
	$(MAKE) apps-all

clean:
	rm -f *.o
	rm -f $(UIS)
	rm -f *_moc.cxx
	rm -f $(DAISYDUCK)
	rm -f .depend

install: install-apps install-man install-data

install-apps: apps
	$(INSTALL) -d $(bindir)
	$(INSTALL) -c -m 755 $(DAISYDUCK) $(bindir)

install-man: $(MANS)
	for m in $(MANS); do \
	  section=`echo $$m | sed -e 's/^.*\\.//'`; \
	  $(INSTALL) -d $(mandir)/man$$section; \
	  $(INSTALL) -m 644 $$m $(mandir)/man$$section; \
	done

install-data: $(DESKTOP) $(ICON)
	$(INSTALL) -d $(sharedir)/pixmaps
	$(INSTALL) -d $(sharedir)/applications
	$(INSTALL) -c -m 644 $(ICON) $(sharedir)/pixmaps
	$(INSTALL) -c -m 644 $(DESKTOP) $(sharedir)/applications

uninstall: uninstall-apps uninstall-man uninstall-data

uninstall-apps:
	rm -f $(bindir)/$(DAISYDUCK)

uninstall-man:
	for m in $(MANS); do \
	  section=`echo $$m | sed -e 's/^.*\\.//'`; \
	  rm -f $(mandir)/man$$section/$$m; \
	done

uninstall-data:
	rm -f $(sharedir)/pixmaps/$(DAISYDUCK).png
	rm -f $(sharedir)/applications/$(DAISYDUCK).desktop

.PHONY: *clean *install* apps*

dist-all:
	cp $(EXTRADIST) $(SRCS) Makefile $(DIST)

.PHONY: dist dist-all

#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif
