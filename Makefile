CC=gcc
CFLAGS=-lportaudio 

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

menc: menc.c
