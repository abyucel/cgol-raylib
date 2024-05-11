NAME = cgol-raylib
VERSION = 0.1.0
BIN = ${NAME}

INCS = -I.
LIBS = -lm -lraylib -lpthread -lGL -ldl -lrt -lX11
CFLAGS += -g -std=c99 -pedantic -Wall -Wextra ${INCS} -DVERSION=\"${VERSION}\"
LDFLAGS += -g ${LIBS}

CC ?= gcc
