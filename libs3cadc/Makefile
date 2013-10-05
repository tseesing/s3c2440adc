
DEBUG = -g -DNQ_DEBUG
EXTRA_CFLAGS = -I. -Wall $(DEBUG)
DESTDIR = /usr/lib

TOOLSDIR = /opt/arm/4.4.3/bin
CC = $(TOOLSDIR)/arm-linux-gcc 
AR = $(TOOLSDIR)/arm-linux-ar

all: libs3cadc.a libs3cadc.so

s3cadc.o : s3cadc.c 
	$(CC) $(EXTRA_CFLAGS) -c -o $@ $^

nqcommon.o : cnqcommon.c
	$(CC) $(EXTRA_CFLAGS) -c -o $@ $^

libs3cadc.a : s3cadc.o
	$(AR) -cvq -o $@ $<

libs3cadc.o : s3cadc.c  
	$(CC) $(EXTRA_CFLAGS) -fPIC -c -o $@ $<

libnqcommon.o : nqcommon.c
	$(CC) $(EXTRA_CFLAGS) -fPIC -c -o $@ $<

libs3cadc.so:  libs3cadc.o libnqcommon.o
	$(CC) -shared -Wl,-soname=libs3cadc.so.1 -o libs3cadc.so.1.0 $<
	ln -sv libs3cadc.so.1.0 libs3cadc.so.1
	ln -sv libs3cadc.so.1	$@

install:
	install -Dm755 libs3cadc.so.1.0 $(DESTDIR)/libs3cadc.so.1.0
	install -Dm666 libs3cadc.so.1 	$(DESTDIR)/libs3cadc.so.1
	install -Dm755 libs3cadc.so 	$(DESTDIR)/libs3cadc.so
	install -Dm644 s3cadc.h  		$(DESTDIR)/../include/s3cadc.h

uninstall: 
	-rm -v $(DESTDIR)/libs3cadc.so.1.0 $(DESTDIR)/libs3cadc.so.1 $(DESTDIR)/libs3cadc.so $(DESTDIR)/libs3cadc.a

.phony : clean

clean:
	-rm -v *.o *.so *.a
	-rm -v lib*.so.[0-9]*

	
test: test.c
	$(CC) $(EXTRA_CFLAGS) -o $@ $<
