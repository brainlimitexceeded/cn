CC	= gcc
M1_FLAGS = -O2 -Wall         -pedantic -std=gnu99
CFLAGS	=  -O2 -Wall -Werror -pedantic -std=gnu99
LIBS	= -lm -lpthread -no-pie

PROGRAM	= httpcmd

$(PROGRAM): main.c http.a
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

http.a: http.c
	$(CC) -c $(CFLAGS) $(LIBS) -o $@ $<

httpm1.a: http.c
	$(CC) -c $(M1_FLAGS) $(LIBS) -o $@ $<

.PHONY: m1
m1: main.c httpm1.a
	$(CC) -c $(M1_FLAGS) $(LIBS) -o $(PROGRAM) $^
