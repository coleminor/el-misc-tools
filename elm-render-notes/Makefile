executable=elm-render-notes

sources= draw.c \
				 image.c \
				 main.c \
				 note.c	\
				 options.c \
				 places.c \
				 utility.c
objects=$(sources:.c=.o)

DEFINES=-DEXECUTABLE_NAME=\"$(executable)\" \
	-DVERSION_MAJOR=0 -DVERSION_MINOR=4

CC=gcc
CFLAGS=-O2 -g
LDLIBS=

CFLAGS += -Wall -Wextra -Werror
CFLAGS += $(DEFINES)

CFLAGS += $(shell pkg-config --cflags pangocairo)
LDLIBS += $(shell pkg-config --libs pangocairo)

all: $(executable)

$(executable): $(objects)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean

clean:
	rm -f $(objects) $(executable)

# gcc -MM *.c
draw.o: draw.c draw.h note.h options.h utility.h
image.o: image.c image.h options.h
main.o: main.c image.h note.h options.h places.h utility.h
note.o: note.c note.h draw.h options.h places.h utility.h
options.o: options.c options.h utility.h
places.o: places.c places.h utility.h
utility.o: utility.c utility.h