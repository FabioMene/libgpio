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

#ifndef LIBGPIO_GPIO_H
#define LIBGPIO_GPIO_H 1

#ifndef LIBGPIO_EXTERN
#define LIBGPIO_EXTERN extern
#endif

#define LIBGPIO_VERSION 0x000100 // 1.0

/** Board in uso
 *  @see gpioInit
 */
typedef enum {
  GPIO_BOARD_PI_A = 0,  // A,  26 pin
  GPIO_BOARD_PI_B,      // B,  26 pin
  GPIO_BOARD_PI_A_PLUS, // A+, 40 pin
  GPIO_BOARD_PI_B_PLUS, // B+, 40 pin
  GPIO_BOARD_PI_2_B     // Rpi 2
} gpio_board_e;

/** @see gpioInit
 */
#define GPIOE_MEM_ACCESS -1  // Impossibile accedere a /dev/mem
#define GPIOE_GPIO_MAP   -2  // Impossibile mappare i registri GPIO
#define GPIOE_CLOCK_MAP  -3  // Impossibile mappare i registri clock
#define GPIOE_PWM_MAP    -4  // Impossibile mappare i registri PWM

/** gpioInit - inizializza le mappe di memoria dei gpio
 *  @var in board    Il modello di raspberry da usare
 *  @var in usebcm   Se diverso da zero usa la numerazione BCM per i GPIO, altrimenti usa la numerazione dei pin (in base a board) 
 *  @return          0 se l'operazione ha successo, altrimenti GPIOE_* 
 */
LIBGPIO_EXTERN int gpioInit(gpio_board_e board, int usebcm);




/** gpioEnd - rimuovi le mappe di memoria
 */
LIBGPIO_EXTERN void gpioEnd(void);



/** Modi per i gpio
 *  @see gpioSetup
 */
#define GPIO_MODE_INPUT  0
#define GPIO_MODE_OUTPUT 1
#define GPIO_MODE_ALT0   4
#define GPIO_MODE_ALT1   5
#define GPIO_MODE_ALT2   6
#define GPIO_MODE_ALT3   7
#define GPIO_MODE_ALT4   3
#define GPIO_MODE_ALT5   2

/** gpioSetup - imposta la modalità di un pin
 *  @var in pin    il pin da impostare
 *  @var in mode   il modo del pin, GPIO_MODE_*
 */
LIBGPIO_EXTERN void gpioSetup(int pin, int mode);




/** Modalità pull
 *  @see gpioPull
 */
#define GPIO_PULL_NONE 0
#define GPIO_PULL_DOWN 1
#define GPIO_PULL_UP   2

/** gpioPull - imposta il tipo di pull o lo rimuove
 *  @var in pin    il pin da impostare
 *  @var in pull   il modo del pull, GPIO_PULL_*
 */
LIBGPIO_EXTERN void gpioPull(int pin, int pull);




/** gpioSet - imposta il livello digitale di un pin
 *  @var in pin    il pin da impostare
 *  @var in val    il livello digitale da impostare (0, != 0)
 */
LIBGPIO_EXTERN void gpioSet(int pin, int val);




/** gpioGet - ottieni il livello digitale di un pin
 *  @var in pin    il pin da impostare
 */
LIBGPIO_EXTERN int gpioGet(int pin);

#endif

