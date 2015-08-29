# libgpio
Una libreria minimale per l'accesso alle periferiche GPIO/PWM/GPCLOCK della Raspberry Pi

### Come funziona
Questa libreria si appoggia solo sulle mappe di memoria degli indirizzi virtuali delle periferiche,
0x20000000 per le prime RPi e 0x3f000000 per la RPi 2.  
Non occupa nessun canale DMA, il che rallenta di poco l'accesso all'hardware ma per la maggior parte degli usi è accettabile

### Come si compila / installa
libgpio non ha dipendenze se non gcc e si compila semplicemente con `make` 

Per installare la libreria dopo averla compilata eseguire `sudo make install`

### Come include
Basta includere `libgpio/gpio.h` e, a seconda del bisogno, `libgpio/pwm.h` e `libgpio/clock.h` e compilare aggiungendo la libreria libgpio (`-lgpio`)

### Come si usa
Fare riferimento agli header per la documentazione  
`gpio.h`: Inizializzazione della libreria e accesso ai GPIO  
`pwm.h`: Controller PWM. L'inizializzazione dei controller richiede di inizializzare il clock PWMCLK tramite clockSetup, da `clock.h`  
`clock.h`: General Purpose clock, PCM Audio e PWM clock
