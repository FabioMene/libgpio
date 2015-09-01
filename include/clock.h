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

#ifndef LIBGPIO_CLOCK_H
#define LIBGPIO_CLOCK_H 1

#ifndef LIBGPIO_GPIO_H
#error Includere libgpio/gpio.h prima di libgpio/clock.h e libgpio/pwm.h
#endif

/** I tre GPCLOCK + clock PCM + clock PWM
    @see clockSetup
    @see clockStop
*/
#define CLOCK_GP0 0
#define CLOCK_GP1 1 // I tre GPClock
#define CLOCK_GP2 2
#define CLOCK_PCM 3 // PCM Audio
#define CLOCK_PWM 4 // PWM // OSC e' la sorgente consigliata

/** Le possibili sorgenti del clock
    @see clockSetup
*/
#define CLOCK_SRC_PLLD 0 // 500000000  Hz / 500  MHz
#define CLOCK_SRC_OSC  1 // 19200000   Hz / 19.2 MHz
#define CLOCK_SRC_HDMI 2 // 216000000  Hz / 216  MHz
#define CLOCK_SRC_PLLC 3 // 1000000000 Hz / 1    GHz, cambia con l'overclock

#define CLOCK_SRC_TOT  4

/** clockSelectSource - seleziona automaticamente la sorgente migliore per questa frequenza / Questa e' una funzione helper
    @var in frequency    la frequenza da considerare
    @var in mash         il filtro mash da usare, da 0 a 3 (0 nella maggior parte dei casi va bene)
    @return       >=0    la sorgente da usare al posto di CLOCK_SRC_*
                   <0    la frequenza non può essere ottenuta con nessuna sorgente
*/
LIBGPIO_EXTERN int clockSelectSource(double frequency, int mash);




/** clockSetup - inizializza un GPCLOCK
  frequency  frequenza del clock, da 4.6875 kHz a 500 MHz inclusi
    @var in clock        il clock da inizializzare
    @var in frequency    la frequenza del clock
    @var in source       la sorgente del clock
    @var in mash         il filtro mash da usare, da 0 a 3 (0 nella maggior parte dei casi va bene)
*/
LIBGPIO_EXTERN void clockSetup(int clock, double frequency, int source, int mash);




/** clockSetupDiv - inizializza un GPCLOCK impostando direttamente il divisore della sorgente ((intero << 12) | (int)(frazionale * 4096))
  frequency  frequenza del clock, da 4.6875 kHz a 500 MHz inclusi
    @var in clock        il clock da inizializzare
    @var in frequency    la frequenza del clock
    @var in source       la sorgente del clock
    @var in mash         il filtro mash da usare, da 0 a 3 (0 nella maggior parte dei casi va bene)
*/
LIBGPIO_EXTERN void clockSetupDiv(int clock, int div, int source, int mash);




/** clockSetFreq - imposta la frequenza di un clock
    @var in clock        il clock da impostare
    @var in frequency    la frequenza
*/
LIBGPIO_EXTERN void clockSetFreq(int clock, double frequency);




/** clockSetDiv - imposta la frequenza di un clock
    @var in clock    il clock da impostare
    @var in div      il divisore della sorgente
*/
LIBGPIO_EXTERN void clockSetDiv(int clock, int div);




/** clockStop - ferma un clock
    @var in clock   il clock da fermare
*/
LIBGPIO_EXTERN void clockStop(int clock);

#endif

