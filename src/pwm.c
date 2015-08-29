/* pwm.c
 * Funzioni di controllo dei due clock PWM
   I vari usleep servono per attendere che le modifiche ai registri abbiano effetto
 */
#include <stdlib.h>
#include <unistd.h>
#include "internal.h"
#include "gpio.h"
#include "pwm.h"

double libgpio_pwm_freq; // Frequenza operativa

struct {
    int          reg_range; // registro range
    int          reg_data;  // registro dati
    unsigned int pres;      // risoluzione pulsazione
} libgpio_pwm[2] = {  // I due canali
    {PWM_RNG1, PWM_DAT1},
    {PWM_RNG2, PWM_DAT2}
};

void pwmSetup(int pwm, unsigned int resolution, int mode, int enable){
    // Per calcolare DATn dal dutycycle
    libgpio_pwm[pwm].pres      = resolution;
    // Ferma solo il PWM pwm e salva il contenuto degli altri in un registro
    unsigned int ctl = *(libgpio_pwm_mem + PWM_CTL) & ~(0xff << (8*pwm));
    *(libgpio_pwm_mem + PWM_CTL) = ctl;
    usleep(10);
    // Imposta la risoluzione
    *(libgpio_pwm_mem + libgpio_pwm[pwm].reg_range) = resolution;
    usleep(10);
    // Il dutycycle iniziale è 0%
    *(libgpio_pwm_mem + libgpio_pwm[pwm].reg_data)  = 0;
    usleep(10);
    // Imposta i modi
    if(mode == PWM_MODE_MARKSPACE) ctl |= 0x80 << (pwm*8);
    if(enable)                     ctl |= 0x01 << (pwm*8);
    // Imposta!
    *(libgpio_pwm_mem + PWM_CTL) = ctl;
}

void pwmSetDuty(int pwm, double dc){
    // Calcola DATn dal dutycycle e impostalo
    *(libgpio_pwm_mem + libgpio_pwm[pwm].reg_data) = (int)((dc/100.0)*libgpio_pwm[pwm].pres);
}
