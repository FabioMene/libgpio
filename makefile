# Makefile minimalissimo

FLAGS   = -DLIBGPIO_EXTERN= -Wall -O3 -Iinclude

all:
	rm -rf build
	mkdir build
	gcc $(FLAGS) -c src/libgpio.c -o build/libgpio.o 
	gcc $(FLAGS) -c src/gpio.c -o build/gpio.o 
	gcc $(FLAGS) -c src/pwm.c -o build/pwm.o 
	gcc $(FLAGS) -c src/clock.c -o build/clock.o 
	gcc $(FLAGS) -c src/globals.c -o build/globals.o
	ar cvru libgpio.a build/*.o

install:
	@rm -rf /usr/include/libgpio
	@rm -f /usr/lib/libgpio.*
	mkdir /usr/include/libgpio
	chmod 0755 /usr/include/libgpio
	install -m 644 include/gpio.h /usr/include/libgpio
	install -m 644 include/pwm.h /usr/include/libgpio
	install -m 644 include/clock.h /usr/include/libgpio
	install -m 644 libgpio.a /usr/lib
