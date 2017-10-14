#!/bin/bash
gcc -D_GNU_SOURCE -DWITH_POSIX -I /usr/local/include/coap/ coap_server.c -o coap_server -lcoap-1
cd slides && pandoc -t beamer -V theme:Rochester -o ../CoAP.pdf CoAPing.md

