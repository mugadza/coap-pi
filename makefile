.RECIPEPREFIX +=
CC = gcc
CFLAGS = -Wall -lm -lrt -lwiringPi -lcoap-1 -lpthread -lfastjson  -DWITH_POSIX

PROG = bin/*
OBJS = obj/*

sensor:
    $(CC) $(CFLAGS) -c src/PIRMotionSensor.c -o obj/PIRMotionSensor
    $(CC) $(CFLAGS) obj/PIRMotionSensor -o bin/sensor

server:
    $(CC) $(CFLAGS) -c src/CoapServer.c -o obj/CoapServer
    $(CC) $(CFLAGS) obj/CoapServer -o bin/server

run_sensor:
    sudo ./bin/sensor

run_server:
    sudo ./bin/server

clean:
    rm $(PROG) $(OBJS)
