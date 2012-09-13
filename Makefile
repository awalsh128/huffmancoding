SHELL=/bin/bash

EXE = hc_compress

CC = gcc
CFLAGS = -g -Wall -pedantic --std=gnu99
CSOURCES = main.c node.c minqueue.c bitio.c huffmancodes.c
CHEADERS = node.h minqueue.h bitio.c huffmancodes.h
COBJECTS = ${CSOURCES:.c=.o} 
SCCFILES = ${CSOURCES} ${CHEADERS} \
           Makefile README
DEPSFILE = Makefile.deps
LINT = splint
MEMCHECK = valgrind --leak-check=full --track-origins=yes
NODEPS = backup spotless clean lint memcheck

all: ${EXE}

${EXE}: ${COBJECTS}
	${CC} ${CFLAGS} -lm -o $@ ${COBJECTS}

deps: ${CSOURCES}
	${CC} -E -MM ${CSOURCES} > ${DEPSFILE}

${DEPSFILE}: 
	@ touch ${DEPSFILE}
	${MAKE} --no-print-directory deps

backup:
	tar -cvf ${EXE}.tar ${SCCFILES}

check: all lint memcheck

spotless: clean
	rm -f ${EXE}

clean:
	rm -fr ${DEPSFILE} ${COBJECTS}

lint:
	${LINT} ${CSOURCES}

memcheck:
	${MEMCHECK} ./${EXE}

run:
	./${EXE}

commitscc:
	git push origin master

ifeq "${filter ${NODEPS}, ${MAKECMDGOALS}}" ""
include ${DEPSFILE}
endif

