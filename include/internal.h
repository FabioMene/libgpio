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

#ifndef LIBGPIO_INTERNAL_H
#define LIBGPIO_INTERNAL_H 1

/* Questo header è usato solo per uso interno, contiene dati generali su tutti i componenti della libreria
*/

// Mappe dei registri gpio/clock/pwm 
extern volatile unsigned* libgpio_gpio_mem;
extern volatile unsigned* libgpio_clock_mem;
extern volatile unsigned* libgpio_pwm_mem;

// Mappe GPIO
extern unsigned char  libgpio_bcm_map[];
extern unsigned char  libgpio_model_a_map[];
extern unsigned char  libgpio_model_b_map[];
extern unsigned char  libgpio_model_ab_plus_b2_map[];
extern unsigned char* libgpio_gpio_map;

// GPIO Setup
#define RPI_1_BASE   0x20000000 // Raspberry model A/B Rev 1/2
#define RPI_2_BASE   0x3f000000 // Raspberry 2 model B

#define GPIO_OFFSET  0x200000   // GPIO    |
#define CLOCK_OFFSET 0x101000   // Clock   | offset da RPI_._BASE
#define PWM_OFFSET   0x20c000   // PWM     |

// Roba varia

#define GPIO_SET(p)  *(libgpio_gpio_mem+7+(p>>5))   // Attiva i GPIO
#define GPIO_CLR(p)  *(libgpio_gpio_mem+10+(p>>5))  // Disattiva i GPIO
#define GPIO_GET(p)  *(libgpio_gpio_mem+13+(p>>5))  // Valore dei GPIO
#define GPIO_PUD     *(libgpio_gpio_mem+37)         // Pull up/down controller
#define GPIO_PUDC(p) *(libgpio_gpio_mem+38+(p>>5))  // Pull up/down clock (non e' un clock, fottuta broadcom)

// PWM

#define PWM_CTL    0 // PWM Controller

#define PWM_RNG1   4 // Range 1
#define PWM_DAT1   5 // Data 1

#define PWM_RNG2   8 // Range 2
#define PWM_DAT2   9 // Data 2

// Clock

#define GPCLK0_CTL 28  // GP0
#define GPCLK0_DIV 29
#define GPCLK1_CTL 30  // GP1
#define GPCLK1_DIV 31
#define GPCLK2_CTL 32  // GP2
#define GPCLK2_DIV 33
#define PCMCLK_CTL 38  // PCM Audio
#define PCMCLK_DIV 39
#define PWMCLK_CTL 40  // PWM
#define PWMCLK_DIV 41


#endif

