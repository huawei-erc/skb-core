#!/bin/bash

gdbus-codegen \
    --interface-prefix org.freedesktop \
    --generate-c-code Skb \
    org.freedesktop.Skb.xml \
&& \
gcc -std=c99 -o service-skb \
    `pkg-config --cflags --libs gio-2.0 gio-unix-2.0` -I/usr/local/eclipse/include/x86_64_linux \
    -L/usr/local/eclipse/lib/x86_64_linux -I$PWD -lreadline -leclipse\
    service-skb.c Skb.h Skb.c skb_main.c skb_service.c skb_functions.c
