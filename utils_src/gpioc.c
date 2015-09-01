/*
    gpioc - a simple program to control Raspberry Pi's hardware, using libgpio
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <libgpio/gpio.h>
#include <libgpio/pwm.h>
#include <libgpio/clock.h>

gpio_board_e board  = GPIO_BOARD_PI_B;
int          usebcm = 1;

#define GPIOC_VERSION 0x000100 // 1.0

char* argv0;

void usage(char* f, ...){
    printf("Uso: %s [opzioni] sezione comando [argomenti]\n"
           "Wrapper per libgpio\n"
           "Opzioni:\n"
           "Per le opzioni che richiedono un valore sono indicati quelli di\n"
           "default tra []\n"
           "    -b  <a|b|a+|b+|2>    Seleziona la raspberry in uso [b]\n"
           "    -p                   Utilizza la numerazione pin invece che quella BCM\n"
           "    -r                   Resetta la cache.\n"
           "                         La cache viene usata per mantenere alcuni dati inerenti\n"
           "                         ai controller PWM inizializzati e ai clock\n"
           "    -h                   Mostra questo aiuto ed esce\n"
           "    -V                   Mostra la versione ed esce\n"
           "Sezione \xc3\xa8:\n"
           "    pin     Inizializza/imposta/ottieni il valore di un gpio/pin\n"
           "    pwm     Gestisci i controller PWM\n"
           "    clock   Gestisci i clock (GP1/2/3, PCM, PWM)\n"
           "Gli argomenti opzinali sono indicati tra [] e il loro valori\n"
           "predefiniti tra [] nella descrizione\n"
           "Comandi per 'pin'\n"
           "    setup <pin> <modalità>               Inizializza il <pin>\n"
           "                                         <modalità> è in, out o alt0-5\n"
           "    pull <pin> <pull>                    Imposta la resistenza pullup/down di <pin>\n"
           "                                         <pull> è up, down o none\n"
           "    set <pin> <val>                      Imposta <pin> a <val>\n"
           "    get <pin>                            Ottieni il valore di <pin>\n"
           "Comandi per 'pwm'\n"
           "    setup <pwm> <res> [mode]             Inizializza <pwm> con risoluzione <res> e modo [mode]\n"
           "                                         <res> è intero, 1024 massimo\n"
           "                                         <mode> è normal o markspace [normal]\n"
           "    stop <pwm>                           Ferma il pwm <pwm>\n"
           "    set <pwm> <dc>                       Imposta il dutycycle di <pwm> a <dc>\n"
           "Comandi per 'clock'\n"
           "Per tutte le opzioni che lo richiedono <clock> è gp0, gp1, gp2, pcm o pwm\n"
           "<freq> supporta i suffissi k e m (x1000, x1000000), case insensitive\n"
           "    selectsrc <freq> <mash>              Seleziona la sorgente migliore per generare questa frequenza\n"
           "    setup <clock> <freq> <src> <mash>    Inizializza <clock> alla frequenza <freq> con sorgente\n"
           "                                         <src> e filtro <mash>\n"
           "    setupd <clock> <div> <src> <mash>    Inizializza <clock> con il divisore <div> dalla sorgente\n"
           "                                         <src> e filtro <mash>\n"
           "    set <clock> <freq>                   Imposta la frequenza di <clock> a <freq>\n"
           "    setd <clock> <div>                   Imposta il divisore di <clock> a <div>\n"
           "    stop <clock>                         Ferma il clock\n"
           , argv0);
    gpioEnd();
    if(f != NULL){
        va_list l;
        va_start(l, f);
        vfprintf(stderr, f, l);
        va_end(l);
        exit(1);
    }
    exit(0);
}

#define CACHE_FILE "/tmp/gpioc.cache"
// PWM - [2] pwm0, [5] pwm1
extern unsigned int libgpio_pwm[6];
// Clock - [2] gp0, [5] gp1, [8] gp2, [11] pcm, [14] pwm
extern int libgpio_clock[15];

void read_cache(){
    int fd = open(CACHE_FILE, O_RDONLY);
    if(fd >= 0){
        read(fd, libgpio_pwm+2,    sizeof(unsigned int));
        read(fd, libgpio_pwm+5,    sizeof(unsigned int));
        read(fd, libgpio_clock+2,  sizeof(int));
        read(fd, libgpio_clock+5,  sizeof(int));
        read(fd, libgpio_clock+8,  sizeof(int));
        read(fd, libgpio_clock+11, sizeof(int));
        read(fd, libgpio_clock+14, sizeof(int));
        close(fd);
    }
}

void write_cache(){
    int fd = open(CACHE_FILE, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if(fd >= 0){
        write(fd, libgpio_pwm+2,    sizeof(unsigned int));
        write(fd, libgpio_pwm+5,    sizeof(unsigned int));
        write(fd, libgpio_clock+2,  sizeof(int));
        write(fd, libgpio_clock+5,  sizeof(int));
        write(fd, libgpio_clock+8,  sizeof(int));
        write(fd, libgpio_clock+11, sizeof(int));
        write(fd, libgpio_clock+14, sizeof(int));
        close(fd);
    } else printf("Attenzione: Impossibile scrivere la cache\n");
}

#define SECT(s) if(strcmp(section, s) == 0)
#define CMD(c) if(strcmp(cmd, c) == 0)
#define REQUIRE(n) if(argc < n){usage("Pochi argomenti per il comando %s::%s\n", section, cmd);}
#define CMD404() usage("Comando %s::%s non riconosciuto\n", section, cmd)
#define END() do{write_cache();gpioEnd();return 0;}while(0)
#define INT(a) ({int __x = strtol(a, &endptr, 10);if(endptr == a){usage("%s non è un intero\n", a);}__x;})
#define DUB(a) ({double __x = strtod(a, &endptr);if(endptr == a){usage("%s non è un decimale\n", a);}__x;})

double getFreq(char* v){
    char* endptr;
    double f = DUB(v);
    if     (strcasecmp(endptr, "k") == 0) f *= 1000.0;
    else if(strcasecmp(endptr, "m") == 0) f *= 1000000.0;
    else if(*endptr != 0) usage("Suffisso %s non riconosciuto\n", endptr);
    return f;
}

int getClock(char* n){
    if     (strcmp(n, "gp0") == 0) return CLOCK_GP0;
    else if(strcmp(n, "gp1") == 0) return CLOCK_GP1;
    else if(strcmp(n, "gp2") == 0) return CLOCK_GP2;
    else if(strcmp(n, "pcm") == 0) return CLOCK_PCM;
    else if(strcmp(n, "pwm") == 0) return CLOCK_PWM;
    usage("%s non è un clock valido\n", n);
    return 0; // -Wall
}

int getClockSrc(char* n){
    if     (strcmp(n, "plld") == 0) return CLOCK_SRC_PLLD;
    else if(strcmp(n, "osc")  == 0) return CLOCK_SRC_OSC;
    else if(strcmp(n, "hdmi") == 0) return CLOCK_SRC_HDMI;
    else if(strcmp(n, "pllc") == 0) return CLOCK_SRC_PLLC;
    usage("%s non è una sorgente valida\n", n);
    return 0; // sempre -Wall
}

char* clock_src_names[] = {
    "N/A",
    "plld",
    "osc",
    "hdmi",
    "pllc"
};

int main(int argc, char* argv[]){
    argv0 = argv[0];
    int opt;
    while((opt = getopt(argc, argv, "b:prhV")) != -1){
        switch(opt){
            case 'b':
                if(strcmp(optarg, "a") == 0)       board = GPIO_BOARD_PI_A;
                else if(strcmp(optarg, "b") == 0)  board = GPIO_BOARD_PI_B;
                else if(strcmp(optarg, "a+") == 0) board = GPIO_BOARD_PI_A_PLUS;
                else if(strcmp(optarg, "b+") == 0) board = GPIO_BOARD_PI_B_PLUS;
                else if(strcmp(optarg, "2") == 0)  board = GPIO_BOARD_PI_2_B;
                else usage("La board '%s' non \xc3\xa8 valida\n", optarg);
                break;
            case 'p':
                usebcm = 0;
                break;
            case 'r':
                remove(CACHE_FILE);
                return 0;
            case 'h':
                usage(NULL);
            case 'V':
                printf("gpioc %x.%x, libgpio %x.%x\n"
                       "Copyright \xc2\xa9 2015 Fabio Meneghetti\n"
                       "Licenza GPLv2+: GNU GPL versione 2 o successive <http://gnu.org/licenses/gpl.html>\n"
                       "Questo è software libero: è possibile modificarlo e ridistribuirlo.\n"
                       "Non c'è ALCUNA GARANZIA, nei limiti permessi dalla legge.\n",
                       GPIOC_VERSION >> 8, GPIOC_VERSION & 0xff, LIBGPIO_VERSION >> 8, LIBGPIO_VERSION & 0xff);
                return 0;
            default:
                usage("Opzione '%c' sconosciuta\n", opt);
        }
    }
    if(gpioInit(board, usebcm) < 0){
        printf("Impossibile accedere alla memoria\n");
        return 1;
    }
    read_cache();
    if(argc-optind < 2) usage("Non ci sono argomenti!\n");
    char* section = *(argv+optind);
    char* cmd     = *(argv+optind+1);
    char** args = argv+optind+2;
    argc        = argc-optind-2;
    char* endptr; // per INT
    setuid(0); setgid(0); // Se setuid
    SECT("pin"){
        CMD("setup"){
            REQUIRE(2);
            int pin = INT(args[0]);
            int mod;
            if     (strcmp(args[1], "in")   == 0) mod = GPIO_MODE_INPUT;
            else if(strcmp(args[1], "out")  == 0) mod = GPIO_MODE_OUTPUT;
            else if(strcmp(args[1], "alt0") == 0) mod = GPIO_MODE_ALT0;
            else if(strcmp(args[1], "alt1") == 0) mod = GPIO_MODE_ALT1;
            else if(strcmp(args[1], "alt2") == 0) mod = GPIO_MODE_ALT2;
            else if(strcmp(args[1], "alt3") == 0) mod = GPIO_MODE_ALT3;
            else if(strcmp(args[1], "alt4") == 0) mod = GPIO_MODE_ALT4;
            else if(strcmp(args[1], "alt5") == 0) mod = GPIO_MODE_ALT5;
            else usage("La modalità %s non è valida\n", args[1]);
            gpioSetup(pin, mod);
            END();
        }
        CMD("pull"){
            REQUIRE(2);
            int pin = INT(args[0]);
            int pull;
            if     (strcmp(args[1], "none") == 0) pull = GPIO_PULL_NONE;
            else if(strcmp(args[1], "up")   == 0) pull = GPIO_PULL_UP;
            else if(strcmp(args[1], "down") == 0) pull = GPIO_PULL_DOWN;
            else usage("La modalità %s non è valida\n", args[1]);
            gpioPull(pin, pull);
            END();
        }
        CMD("set"){
            REQUIRE(2);
            int pin = INT(args[0]);
            int val = INT(args[1]);
            gpioSet(pin, val);
            END();
        }
        CMD("get"){
            REQUIRE(1);
            int pin = INT(args[0]);
            printf("%d\n", gpioGet(pin));
            END();
        }
        CMD404();
    }
    SECT("pwm"){
        CMD("setup"){
            REQUIRE(2);
            int pwm  = INT(args[0]);
            int res  = INT(args[1]);
            int mode = PWM_MODE_NORMAL;
            if(argc > 2){
                if     (strcmp(args[2], "normal")    == 0) mode = PWM_MODE_NORMAL;
                else if(strcmp(args[2], "markspace") == 0) mode = PWM_MODE_MARKSPACE;
                else usage("La modalità %s non è valida\n", args[2]);
            }
            pwmSetup(pwm, res, mode, 1);
            END();
        }
        CMD("stop"){
            REQUIRE(1);
            int pwm  = INT(args[0]);
            pwmSetup(pwm, 0, 0, 0);
            END();
        }
        CMD("set"){
            REQUIRE(2);
            int pwm   = INT(args[0]);
            double dc = DUB(args[1]);
            pwmSetDuty(pwm, dc);
            END();
        }
        CMD404();
    }
    SECT("clock"){
        CMD("selectsrc"){
            REQUIRE(2);
            double freq = getFreq(args[0]);
            int mash    = INT(args[1]);
            printf("%s\n", clock_src_names[clockSelectSource(freq, mash)+1]);
            END();
        }
        CMD("setup"){
            REQUIRE(4);
            int    clock = getClock(args[0]);
            double freq  = getFreq(args[1]);
            int    src   = getClockSrc(args[2]);
            int    mash  = INT(args[3]);
            clockSetup(clock, freq, src, mash);
            END();
        }
        CMD("setupd"){
            REQUIRE(4);
            int clock = getClock(args[0]);
            int div   = INT(args[1]);
            int src   = getClockSrc(args[2]);
            int mash  = INT(args[3]);
            clockSetupDiv(clock, div, src, mash);
            END();
        }
        CMD("set"){
            REQUIRE(2);
            int    clock = getClock(args[0]);
            double freq  = getFreq(args[1]);
            clockSetFreq(clock, freq);
            END();
        }
        CMD("setd"){
            REQUIRE(2);
            int clock = getClock(args[0]);
            int div   = INT(args[1]);
            clockSetDiv(clock, div);
            END();
        }
        CMD("stop"){
            REQUIRE(1);
            int clock = getClock(args[0]);
            clockStop(clock);
            END();
        }
        CMD404();
    }
    usage("Sezione %s non riconosciuta\n", section);
    return 0; // Così il compilatore è felice
}
