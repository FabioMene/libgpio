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

/* clock.c
   fornisce l'accesso ai 3 general purpose clock presenti sulla rpi, più il clock del PWM
   I vari usleep servono per attendere che le modifiche ai registri abbiano effetto
*/
#include <unistd.h>
#include "internal.h"
#include "gpio.h"
#include "clock.h"

// Queste sono le mappature da CLOCK_SRC_* al numero effettivo BCM
int libgpio_clock_sources[] = {
    6, // PLLD
    1, // OSC
    7, // HDMI
    5  // PLLC - Questo cambia a seconda dell'overclock, non dovrebbe essere considerato affidabile senza accurati calcoli
};

// Frequenze delle sorgenti
double libgpio_clock_sources_freq[] = {
    500e6,  // PLLD
    19.2e6, // OSC
    216e6,  // HDMI
    1000e6  // PLLC
};

// I mash hanno limite minimo per il divisoro intero
int libgpio_mash_min[] = {
    1,
    2,
    3,
    5
};

// Struttura per mappare i clock ai registri BCM
struct {
    int reg_ctl; // reg controllo
    int reg_div; // reg divisore / tolleranza
    int source;  // sorgente clock, per il calcolo del divisore dalla frequenza
} libgpio_clock[] = {
    {GPCLK0_CTL, GPCLK0_DIV}, // GPCLK0
    {GPCLK1_CTL, GPCLK1_DIV}, // GPCLK1
    {GPCLK2_CTL, GPCLK2_DIV}, // GPCLK2
    {PCMCLK_CTL, PCMCLK_DIV}, // PCM Audio
    {PWMCLK_CTL, PWMCLK_DIV}  // PWM
};

#define CLOCK_CTL libgpio_clock[clock].reg_ctl // macro tattiche per evitare di riscrivere troppo 
#define CLOCK_DIV libgpio_clock[clock].reg_div

int clockSelectSource(double frequency, int mash){
    int s;
    for(s=0;s < CLOCK_SRC_TOT;s++){
        // Calcola il divisore con questa sorgente
        int i = libgpio_clock_sources_freq[s] / frequency;
        // Se è nei limiti ritorna questa sorgente
        if(i >= libgpio_mash_min[mash] && i <= 4095) return s;
    }
    // Se niente può generare questa frequenza ritorna errore
    return -1;
}

void clockSetup(int clock, double frequency, int source, int mash){
    // Per clockSetFreq(int, double);
    libgpio_clock[clock].source = source;
    // Calcola le due parti del divisore, intera e frazionale
    double div = libgpio_clock_sources_freq[source] / frequency;
    int i = div;
    int f = (div-i) * 4096;
    // Se la sorgente non può generare questa frequenza ritorna, dovrebbe essere verificato con clockSelectSource
    if(i < libgpio_mash_min[mash] || i > 4095) return;
    // Ferma il clock
    *(libgpio_clock_mem + CLOCK_CTL) = 0x5a000020;
    // e attendi finché è occupato
    while((*(libgpio_clock_mem + CLOCK_CTL)) & 0x00000080) usleep(1);
    // Setta il divisore
    *(libgpio_clock_mem + CLOCK_DIV) = 0x5a000000 | (i << 12) | f;
    usleep(10);
    // Nell'ordine: abilita clock | setta il mash | setta la sorgente
    *(libgpio_clock_mem + CLOCK_CTL) = 0x5a000010 | (mash << 9) | libgpio_clock_sources[source];
}

void clockSetupDiv(int clock, int div, int source, int mash){
    // Per clockSetFreq(int, double);
    libgpio_clock[clock].source = source;
    // Ferma il clock
    *(libgpio_clock_mem + CLOCK_CTL) = 0x5a000020;
    // e attendi finché è occupato
    while((*(libgpio_clock_mem + CLOCK_CTL)) & 0x00000080) usleep(1);
    *(libgpio_clock_mem + CLOCK_DIV) = 0x5a000000 | div;
    // Setta il divisore
    usleep(10);
    // Nell'ordine: abilita clock | setta il mash | setta la sorgente
    *(libgpio_clock_mem + CLOCK_CTL) = 0x5a000010 | (mash << 9) | libgpio_clock_sources[source];
}

void clockSetFreq(int clock, double freq){
    // Calcola le due parti del divisore. Le variabli sono "register" per cercare di velocizzare la funzione,
    // in modo da avere solo 4 accessi alla memoria (2 per div e 2 per impostarlo)
    register double div = libgpio_clock_sources_freq[libgpio_clock[clock].source] / freq;
    register int i = div;
    register int f = (div-i) * 4096;
    // Imposta il nuovo divisore
    *(libgpio_clock_mem + CLOCK_DIV) = 0x5a000000 | (i << 12) | f;
}

void clockSetDiv(int clock, int div){
    // Setta il divisore. Punto.
    *(libgpio_clock_mem + CLOCK_DIV) = 0x5a000000 | div;
}

void clockStop(int clock){
    // Ferma il clock
    *(libgpio_clock_mem + CLOCK_CTL) = 0x5a000020;
    // e attendi finché non è eseguito
    while((*(libgpio_clock_mem + CLOCK_CTL)) & 0x00000080) usleep(1);
}



