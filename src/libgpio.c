/* libgpio.c
 * Funzioni di inizializzazione/deinizializzazione
 */
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "internal.h"
#include "gpio.h"

int libgpio_mem_fd;

#define ERR(p, c) if(p == MAP_FAILED){p=NULL;gpioEnd();return c;} // Per semplificare la vita

int gpioInit(gpio_board_e board, int usebcm){
    // Apri la memoria!
    libgpio_mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(libgpio_mem_fd < 0) return GPIOE_MEM_ACCESS;
    // La base periferiche cambia tra Rpi A, B, A+, B+ e Rpi 2
    unsigned int rpi_base = (board == GPIO_BOARD_PI_2_B)?RPI_2_BASE:RPI_1_BASE;
    // GPIO Map
    libgpio_gpio_mem  = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, libgpio_mem_fd, rpi_base + GPIO_OFFSET);
    ERR(libgpio_gpio_mem,  GPIOE_GPIO_MAP);
    // CLOCK Map
    libgpio_clock_mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, libgpio_mem_fd, rpi_base + CLOCK_OFFSET);
    ERR(libgpio_clock_mem, GPIOE_CLOCK_MAP);
    // PWM Map
    libgpio_pwm_mem   = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, libgpio_mem_fd, rpi_base + PWM_OFFSET);
    ERR(libgpio_pwm_mem,   GPIOE_PWM_MAP);
    // Mappa pin GPIO
    if(usebcm) libgpio_gpio_map = libgpio_bcm_map;
    else {
        if     (board == GPIO_BOARD_PI_A) libgpio_gpio_map = libgpio_model_a_map;
        else if(board == GPIO_BOARD_PI_B) libgpio_gpio_map = libgpio_model_b_map;
        else                              libgpio_gpio_map = libgpio_model_ab_plus_b2_map;
    }
    return 0;
}

#undef ERR

void gpioEnd(){
    // Rimuovi le mappature
    if(libgpio_gpio_mem)  munmap((void*)libgpio_gpio_mem,  4096);
    if(libgpio_clock_mem) munmap((void*)libgpio_clock_mem, 4096);
    if(libgpio_pwm_mem)   munmap((void*)libgpio_pwm_mem,   4096);
    libgpio_gpio_mem  = NULL;
    libgpio_clock_mem = NULL;
    libgpio_pwm_mem   = NULL;
    close(libgpio_mem_fd);
}

