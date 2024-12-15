
PREFIX := /usr/local

INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

TARGETS := program2 program1

all: $(strip $(TARGETS))

CFLAGS  := -std=c11 -Og -I$(INCDIR)
LDFLAGS := -L$(LIBDIR) -Wl,-rpath,$(LIBDIR)

program2: program2.o
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -fPIE $^ -lssl -lcrypto

program1: program1.o dbl_and_add.o
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -fPIE $^ -lgmp

%.o:: %.c
	$(CC) -o $@ $(CFLAGS) -fPIC -DPIC -c $<

clean:
	- @rm -vf *.o $(TARGETS)

# not files to be created
.PHONY: clean all

# disable default implicit rules
.SUFFIXES:
