
CC = arm-linux-gcc
EXINCLUDEDIR += ../../libs3cadc
DEBUGFLAGS += -g -DNQDEBUG
CFLAGS += -Wall -Werror $(DEBUGFLAGS)

objs := adcdaemonize.o adcnetwork.o main.o nqcommon.o

all: adcserver

adcserver: $(objs)
	$(CC) -L$(EXINCLUDEDIR) -ls3cadc  -o $@ $^

$(objs) : nqcommon.h
main.o	: main.c adcserverexit.h adcdaemonize.h adcnetwork.h
adcdaemonize.o : adcdaemonize.c adcdaemonize.h
adcnetwork.o : adcnetwork.c adcnetwork.h
nqcommon.o : nqcommon.h nqcommon.c

servertest: servertest.o nqcommon.o
	$(CC) -o $@ $^

servertest.o: servertest.c
	$(CC) -c $(CFLAGS) -I$(EXINCLUDEDIR) -o $@ $<

$(objs) : %.o : %.c
	$(CC) -c $(CFLAGS) -I$(EXINCLUDEDIR) -o $@ $<




.phony : all clean install

clean :
	rm -v *.o adcserver servertest

install:
	-echo 'You need to copy adcserver youself'

	
