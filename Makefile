################################################################################
#
# Infinite Memory Engine - IME native wrapper
#
# Copyright (c) 2018, DataDirect Networks.
#
################################################################################

LIB_NAME=libim_client.so
SRC_FILE=ime_client.c
COMMON_CFLAGS=-std=c99 -shared -fpic -Wall

all: lio

clean:
	rm -f *.so

nolio:
	gcc ${COMMON_CFLAGS} ${SRC_FILE} -o ${LIB_NAME}

lio:
	gcc ${COMMON_CFLAGS} -lrt -DUSE_LIO ${SRC_FILE} -o ${LIB_NAME}

