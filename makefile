.RECIPEPREFIX +=
CC = gcc
CFLAGS = -Wall -lm -lrt -lwiringPi

PROG = bin/*
OBJS = obj/*

default:
    mkdir -p bin obj
    $(CC) $(CFLAGS) -c src/CoapServer.c -o obj/CoapServer
    $(CC) $(CFLAGS) obj/CoapServer -o bin/Server

run:
    sudo ./bin/Server

clean:
    rm $(PROG) $(OBJS)
