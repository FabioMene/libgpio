/*
    libgpio - a simple library to access Raspberry Pi's hardware
    Copyright (C) 2015  Fabio Meneghetti

    This file is part of libgpio

    libgpio is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    libgpio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with libgpio; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/* gpio.c
 * Funzioni di setup e accesso ai gpio
 */
#include <unistd.h>
#include "internal.h"
#include "gpio.h"

void gpioSetup(int pin, int mode){
    // Ottieni il numero BCM dal numero fornito
    pin = libgpio_gpio_map[pin];
    // Imposta il pin in modalità input AKA reset 
    *(libgpio_gpio_mem+(pin/10)) &= ~(7<<((pin%10)*3));
    // Imposta la modalità del pin vera e propria
    if(mode != GPIO_MODE_INPUT) *(libgpio_gpio_mem+(pin/10)) |= mode << ((pin%10)*3);
}

void gpioPull(int pin, int pull){
    // I vari usleep servono per attendere che le modifiche ai registri abbiano effetto
    // Ottieni il numero BCM dal numero fornito
    pin = libgpio_gpio_map[pin];
    // Imposta la modalità Pull up/down da impostare
    GPIO_PUD  = pull & 3;
    usleep(500);
    // Imposta il pin a cui applicare la modalità 
    GPIO_PUDC(pin) = 1 << (pin & 0x1f);
    usleep(500);
    // Resetta tutto, le modifiche - ovviamente - restano
    GPIO_PUD  = 0;
    GPIO_PUDC(pin) = 0;
}

void gpioSet(int pin, int v){
    // Ottieni il numero BCM dal numero fornito
    pin = libgpio_gpio_map[pin];
    // Set/Clear cono in due registri separati quindi...
    if(v) GPIO_SET(pin) = 1 << (pin & 0x1f);
    else  GPIO_CLR(pin) = 1 << (pin & 0x1f);
}

int gpioGet(int pin){
    // Ottieni il numero BCM dal numero fornito
    pin = libgpio_gpio_map[pin];
    return (GPIO_GET(pin) & (1 << (pin & 0x1f)))?1:0;
}

