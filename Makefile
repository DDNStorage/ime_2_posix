################################################################################
#
# Infinite Memory Engine - IME native wrapper
#
# Copyright (c) 2018, DataDirect Networks.
#
################################################################################

LIB_NAME=libim_client
SRC_FILE=ime_client.c
HEADER_FILE=ime_native.h
COMMON_CFLAGS=-std=c99 -Wall -g -Iime_native/
EXAMPLES_DIR=examples

all: nolio

clean:
	rm -f *.so examples/write

nolio: ime_client.c
	gcc ${COMMON_CFLAGS} -c -fPIC ${SRC_FILE} -o ${LIB_NAME}.o
	gcc -shared ${LIB_NAME}.o -o ${LIB_NAME}.so

lio: ime_client.c
	gcc ${COMMON_CFLAGS} -c -fPIC -ltr -DUSE_LIO ${SRC_FILE} -o ${LIB_NAME}.o
	gcc -shared ${LIB_NAME}.o -o ${LIB_NAME}.so

install:
	chmod 644 ${LIB_NAME}.so
	mkdir -p ${DESTDIR}/opt/ddn/ime/lib/
	mkdir -p ${DESTDIR}/opt/ddn/ime/include
	install ${LIB_NAME}.so ${DESTDIR}/opt/ddn/ime/lib/
	install ime_native/${HEADER_FILE} ${DESTDIR}/opt/ddn/ime/include

examples: ${EXAMPLES_DIR}/write.c
	gcc ${COMMON_CFLAGS} ${EXAMPLES_DIR}/write.c -o ${EXAMPLES_DIR}/write -L. -Iinclude/ -lim_client

.PHONY: examples

rpm-check-env:
ifndef BUILD_PATH
BUILD_PATH=${HOME}/rpmbuild
endif

rpm: rpm-check-env
	mkdir -p ${BUILD_PATH}/{BUILD,RPMS,SRPMS,SPECS,SOURCES}
	git archive --format=tar.gz --prefix=ime-wrapper-master/ HEAD > ime-wrapper-master.tar.gz
	mv ime-wrapper-master.tar.gz ${BUILD_PATH}/SOURCES
	rpmbuild --define "_version master" -bb ime-wrapper.spec

