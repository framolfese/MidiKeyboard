CC=gcc
CCOPTS= --std=gnu99 -Wall -lm -lopenal
AR=ar

HEADERS=serialreader.h\
		../LibreriaCondivisa/libreria.h\


OBJS=serialreader.o\
	  ../LibreriaCondivisa/libreria.o\

LIBS=lib.a

BINS=main

#disastros_test

.phony: clean all


all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

lib.a: $(OBJS) $(HEADERS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)

main:	serialreader.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm -lopenal -pthread


clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
