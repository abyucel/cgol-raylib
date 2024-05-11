.POSIX:

include config.mk

SRC = main.c
OBJ = ${SRC:.c=.o}

all: options ${BIN}

options:
	@echo "build options:"
	@echo "CFLAGS  = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC      = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

${BIN}: ${OBJ}
	${CC} -o ${BIN} ${OBJ} ${LDFLAGS}

clean:
	rm -f ${BIN} ${OBJ}

.PHONY: all options clean
