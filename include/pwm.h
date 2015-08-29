#ifndef LIBGPIO_PWM_H
#define LIBGPIO_PWM_H 1

#ifndef LIBGPIO_GPIO_H
#error Includere libgpio/gpio.h prima di libgpio/pwm.h e libgpio/clock.h
#endif

/*
Per impostare un PWM:
 - Impostare un pin nel modo alternativo PWMn_OUT (documentazione BCM) cong gpioSetup(p, m)
 - Impostare il clock con clockSetup(PWMCLK, f, CLOCK_SOURCE_OSC, 0)
 - Inizializzare il controller con pwmSetup(p, r, m, e)
 - Impostare il dutycycle con pwmSetDuty(d)

Per disabilitare un PWM:
 - pwmSetup(p, r, m, e) con e posto a 0

*/

/** I due pwm
    @see pwmSetup
*/
#define PWM0 0
#define PWM1 1

/** Modalità operative dei PWM
    @see pwmSetup
*/
#define PWM_MODE_NORMAL    0 // Modalità pwm
#define PWM_MODE_MARKSPACE 1 // Modalità mark:space

/** Non c'e' bisogno di commento. Veramente.
    @see pwmSetup
*/
#define PWM_DISABLE 0
#define PWM_ENABLE  1

/** pwmSetup - imposta la modalità operativa di un controller PWM
    @var in pwm          il controller pwm da impostare
    @var in resolution   la risoluzione delle pulsazioni pwm, 4096 max
    @var in mode         la modalità del pwm
    @var in enable       abilita/disabilita il controller
*/
LIBGPIO_EXTERN void pwmSetup(int pwm, unsigned int resolution, int mode, int enable);




/** pwmSetDuty - imposta il dutycycle di un controller PWM
    @var in pwm    il controller pwm da impostare
    @var in dc     il dutycycle, da 0.0 a 99.999999 (100)
*/
LIBGPIO_EXTERN void pwmSetDuty(int pwm, double dc);

#endif

