# Colorimetro

Questo progetto implementa un colorimetro utilizzando una scheda di sviluppo con diverse periferiche. Il programma offre tre funzioni principali: avvio della scansione colorimetrica, visualizzazione del numero di rilevamenti del colore rosso e reset dei colori salvati.

## Funzioni Principali

### Funzione 1 - Avvio della Scansione Colorimetrica

Quando si sceglie la funzione 1, il programma:
1. Genera un beep (PWM con duty cycle al 50% e frequenza di 10kHz sullo speaker) per indicare l'inizio della scansione.
2. Inizia a misurare i valori RGB letti dal sensore. Sul display LCD della scheda appare "R: xxyy" (dove xxyy rappresenta il valore di Red misurato, ad esempio R: 255). Analogamente per Green e Blue.
3. Cliccando il pulsante BTNC viene generato un interrupt (External Interrupt INT4) che interrompe la scansione e salva quante volte è stato rilevato il rosso nella memoria flash disponibile.

### Funzione 2 - Visualizza il Numero di Volte che è Stato Rilevato il Colore Rosso

Quando si sceglie la funzione 2, il programma:
1. Visualizza sul terminale il numero di volte che è stato misurato un target rosso (salvato nella memoria flash della scheda).
2. Il LED RGB della scheda si accende di rosso e lampeggia (toggle time di 0.5s) per il numero di volte che il rosso è stato rilevato.

### Funzione 3 - Reset Colori Salvati

Quando si sceglie di resettare i colori salvati, il programma:
1. Cancella la memoria flash della scheda.

## Periferiche Principali

- **UART**: pin RF12 (UART4TX) e RF13 (UART4RX)
- **OUTPUT COMPARE**: OC1, pin RB14
- **PMP**: per LCD
- **TIMER**: Timer3 per PWM e Timer2 per funzioni di Delay
- **I2C**: per il sensore TCS34725
- **SPI**: per memoria Flash
- **GPIO**: per BTNC e LED RGB

## Sensore TCS34725

Il sensore TCS34725 è utilizzato per misurare i valori RGB. Il sensore comunica con la scheda tramite l'interfaccia I2C. Le funzioni principali per interagire con il sensore includono:

- **CLM_Init()**: Inizializza il sensore e configura i registri necessari.
- **CLM_GetID()**: Ottiene l'ID del sensore.
- **CLM_GetColorData(unsigned int *colors)**: Legge i dati di colore dal sensore e li normalizza.

## Utilizzo

1. Clona il repository:
   ```sh
   git clone https://github.com/GionaValse/Colorimetro.git
2. Apri il progetto in MPLAB X IDE.
3. Compila e carica il programma sulla scheda di sviluppo.
4. Utilizza il terminale per interagire con il programma e selezionare le funzioni desiderate.
