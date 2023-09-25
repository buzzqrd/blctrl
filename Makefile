CC = gcc
CFLAGS = -lm

HEARDERS = blctrl.h

blctrl: blctrl.c $(HEADERS)
	cc blctrl.c -o blctrl $(CFLAGS)	

install:
	sudo cp blctrl /usr/bin/

uninstall:
	sudo rm -f /usr/bin/blctrl

clean:
	rm -rf blctrl

