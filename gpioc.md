gpioc
=====
Un programma per manipolare facilmente i GPIO

Sinossi
-------
`gpioc [opzioni] sezione comando [argomenti del comando]`

Opzioni
-------
`-b a|b|a+|b+|2` Specifica su quale raspberry si è. Di default è b  
`-p` Usa la numerazione pin dei GPIO invece di quella BCM. Ad esempio usando questo switch gpio4, pin 7 dell'header P1, viene identificato proprio con 7 invece che con 4. Per le schede provviste di header P5 la numerazione di questi pin è 50+pin  
`-r` Resetta la cache. Alcuni dati inerenti al clock e al pwm sono conservati da libgpio in strutture interne. Senza questi dati alcune funzioni di gpioc non sarebbero disponibili, per questo il programma le salva in `/tmp/gpioc.cache`. Questo switch risulta utile in caso di modifica da parte di applicazioni esterne dell'hardware clock e pwm  
`-h` Mostra l'aiuto integrato  
`-V` Mostra la versione libgpio e gpioc  

Sezioni
-------
Le sezioni sono i tre principali controller presenti sulla raspberry.  
`pin` Per tutte le funzioni riguardanti i pin (setup/pull/set/get)  
`pwm` Fornisce l'accesso ai due controller PWM presenti sulla scheda  
`clock` Per l'accesso ai General Purpose Clock, al clock PCM e al clock PWM  

Comandi
-------
Ogni sezione ha il suo set di comandi  
Gli argomenti opzionali sono indicati tra `[]`  

# Comandi per `pin`
`setup <pin> <modalità>` Inizializza il pin. modalità è in, out o alt0-5  
`pull <pin> <pull>` Imposta la resistenza pullup/down di pin. pull è up, down o none  
`set <pin> <val>` Imposta pin a val  
`get <pin>` Ottieni il valore di pin  

# Comandi per `pwm`
`setup <pwm> <res> [mode]` Inizializza pwm con risoluzione res e modo [mode]. res è intero, 2048 massimo. mode è normal(default) o markspace  
`stop <pwm>` Ferma il pwm  
`set <pwm> <dc>` Imposta il dutycycle di pwm a dc  

# Comandi per `clock`
Per tutte le opzioni che lo richiedono clock è gp0, gp1, gp2, pcm o pwm  
freq supporta i suffissi k e m (x1000, x1000000), case insensitive  
`selectsrc <freq> <mash>` Seleziona la sorgente migliore per generare questa frequenza. Non modifica l'hardware e spesso altre scelte sono possibili, questo comando funge da "guida"  
`setup <clock> <freq> <src> <mash>` Inizializza clock alla frequenza freq con sorgente src e filtro mash  
`setupd <clock> <div> <src> <mash>` Inizializza clock con il divisore div dalla sorgente src e filtro mash  
`set <clock> <freq>` Imposta la frequenza di clock a freq  
`setd <clock> <div>` Imposta il divisore di clock a div  
`stop <clock>` Ferma il clock  
