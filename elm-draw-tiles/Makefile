executable=elm-draw-tiles

sources= main.c \
         map.c \
         options.c \
         utility.c
objects=$(sources:.c=.o)

DEFINES=-DEXECUTABLE_NAME=\"$(executable)\"

CC=gcc
CFLAGS=-O2 -g
LDLIBS=

CFLAGS += -Wall -Wextra -Werror
CFLAGS += $(DEFINES)

CFLAGS += $(shell pkg-config --cflags cairo)
LDLIBS += $(shell pkg-config --libs cairo) -lz

all: $(executable)

$(executable): $(objects)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean

clean:
	rm -f $(objects) $(executable)

# gcc -MM *.c
main.o: main.c map.h options.h utility.h
map.o: map.c map.h utility.h
options.o: options.c options.h utility.h
utility.o: utility.c utility.h
