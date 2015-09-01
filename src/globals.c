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

/* globals.c - contiene le tabelle di associazione pin/gpio e le variabili globali
*/

#include <stdlib.h>

// Puntatori agli indirizzi virtuali delle periferiche

volatile unsigned* libgpio_gpio_mem  = NULL;
volatile unsigned* libgpio_clock_mem = NULL;
volatile unsigned* libgpio_pwm_mem   = NULL;

/* Tabelle GPIO */

// Mappatura BCM, poco da fare
unsigned char libgpio_bcm_map[] = { 
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
};

// Nelle prossime mappature si associa ogni pin dell'header p1 al gpio corrispondente, attenzione ai numeri dei pin
// Per l'header p5 il numero del pin diventa 50+pin (da 51 a 58 compresi)

// Mappature Rpi Model A (Rpi rev.1)
unsigned char libgpio_model_a_map[] = { 0, // Segnaposto
    -1, -1, // +3v3         +5
     0, -1, // gpio0/sda1   +5
     1, -1, // gpio1/scl1   GND
     4, 14, // gpio4        gpio14/txd0 (boot in alt0=uart tx)
    -1, 15, // GND          gpio15/rxd0 (boot in alt0=uart rx)
    17, 18, // gpio17       gpio18
    21, -1, // gpio21       GND
    22, 23, // gpio22       gpio23
    -1, 24, // +3v3         gpio24
    10, -1, // gpio10/mosi  GND
     9, 25, // gpio9/miso   gpio25
    11,  8, // gpio11/sclk  gpio8/spi0ce0#
    -1,  7  // GND          gpio7/spi0ce1#
};

// Mappature Rpi Model B (Rpi rev.2)
unsigned char libgpio_model_b_map[] = { 0, // Segnaposto
    // Header P1
    -1, -1, // +3v3         +5v
     2, -1, // gpio2/sda1   +5v
     3, -1, // gpio3/scl1   GND
     4, 14, // gpio4        gpio14/txd0 (boot in alt0=uart tx)
    -1, 15, // GND          gpio15/rxd0 (boot in alt0=uart rx)
    27, 18, // gpio27       gpio18
    21, -1, // gpio21       GND
    22, 23, // gpio22       gpio23
    -1, 24, // +3v3         gpio24
    10, -1, // gpio10/mosi  GND
     9, 25, // gpio9/miso   gpio25
    11,  8, // gpio11/sclk  gpio8/spi0ce0#
    -1,  7, // GND          gpio7/spi0ce1#
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Segnaposti da 27 a 50
    // Header P5
    -1, -1, // +5v          +3v3
    28, 29, // gpio28       gpio29
    30, 31, // gpio30       gpio31
    -1, -1, // GND          GND
};

// Mappature per Rpi Model A+, B+ e B2
unsigned char libgpio_model_ab_plus_b2_map[] = { 0, // Segnaposto
    // Header P1
    -1, -1, // +3v3         +5v
     2, -1, // gpio2/sda1   +5v
     3, -1, // gpio3/scl1   GND
     4, 14, // gpio4        gpio14/txd0 (boot in alt0=uart tx)
    -1, 15, // GND          gpio15/rxd0 (boot in alt0=uart rx)
    27, 18, // gpio27       gpio18
    21, -1, // gpio21       GND
    22, 23, // gpio22       gpio23
    -1, 24, // +3v3         gpio24
    10, -1, // gpio10/mosi  GND
     9, 25, // gpio9/miso   gpio25
    11,  8, // gpio11/sclk  gpio8/spi0ce0#
    -1,  7, // GND          gpio7/spi0ce1#
     0,  1, // gpio0/id_sd  gpio1/id_sc
     5, -1, // gpio5        GND
     6, 12, // gpio6        gpio12
    13, -1, // gpio13       GND
    19, 16, // gpio19/miso  gpio16/ce2# 
    26, 20, // gpio26       gpio20/mosi
    -1, 21, // GND          gpio21/sclk
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Segnaposti da 41 a 50
    // Header P5
    -1, -1, // +5v          +3v3
    28, 29, // gpio28       gpio29
    30, 31, // gpio30       gpio31
    -1, -1  // GND          GND
};

// La mappatura attualmente in uso
unsigned char* libgpio_gpio_map = NULL;

