CC=gcc
CFLAGS=-g -lportaudio

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

menc: menc.c
