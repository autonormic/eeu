# GNU make variant (the native FreeBSD makefile is Makefile).
CC ?= cc
PREFIX ?= /usr/local
DESTDIR ?=
UTF8_LOCALE ?= $(NLS_LOCALE)
NLS_LOCALE ?= ru_RU.UTF-8
NLS_MSG = nls/$(NLS_LOCALE)/eeu.msg
NLS_CAT = nls/$(NLS_LOCALE)/eeu
CPPFLAGS += -DHAS_NCURSES -DHAS_STDLIB -DHAS_UNISTD -DHAS_STDARG \
	-DHAS_SYS_WAIT
CFLAGS ?= -O2
CFLAGS += -std=gnu99 -Wall

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),FreeBSD)
LDLIBS += -ltinfow -lncursesw
else
LDLIBS += -lncursesw
endif

OBJS = ee.o ee_utf8.o

.PHONY: all test install clean
all: eeu reeu $(NLS_CAT)

$(NLS_CAT): $(NLS_MSG)
	mkdir -p $(dir $@)
	LC_ALL=$(UTF8_LOCALE) gencat $@ $<

eeu: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

reeu: eeu
	ln -sf eeu reeu

ee.o: ee.c ee_utf8.h ee_version.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c ee.c -o ee.o

ee_utf8.o: ee_utf8.c ee_utf8.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c ee_utf8.c -o ee_utf8.o

test: $(NLS_CAT)
	$(CC) $(CPPFLAGS) $(CFLAGS) -I. tests/test_ee_utf8.c ee_utf8.c -o tests/test_ee_utf8
	LC_ALL=$(UTF8_LOCALE) ./tests/test_ee_utf8
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_ee_nls.c -o tests/test_ee_nls
	NLSPATH="$(CURDIR)/nls/$(NLS_LOCALE)/%N" LC_ALL=$(UTF8_LOCALE) ./tests/test_ee_nls eeu

install: eeu $(NLS_CAT)
	install -d $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/man/man1
	install -d $(DESTDIR)$(PREFIX)/share/nls/$(NLS_LOCALE)
	install -m 755 eeu $(DESTDIR)$(PREFIX)/bin/eeu
	ln -sf eeu $(DESTDIR)$(PREFIX)/bin/reeu
	install -m 644 eeu.1 $(DESTDIR)$(PREFIX)/man/man1/eeu.1
	install -m 644 $(NLS_CAT) $(DESTDIR)$(PREFIX)/share/nls/$(NLS_LOCALE)/eeu

clean:
	rm -f $(OBJS) eeu reeu tests/test_ee_utf8 tests/test_ee_nls $(NLS_CAT)
