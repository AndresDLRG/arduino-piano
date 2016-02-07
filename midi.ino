#include "midi.h"

/*
 * Este arreglo reduce mucho el código ya que se pone todo en ciclos
 * el índice de PINES_ENTRADA debe concidir con el índice de PINES_NOTAS.
 * Cada nota con su índice corresponde al pin con el mismo índice
 */
// pines a utilizar
const uint8_t PINES_ENTRADA[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
// que nota reproduce cada pin
const uint8_t PINES_NOTAS[] = {NOTE_DO_4, NOTE_DO_4_, NOTE_RE_4, NOTE_RE_4_, 
                               NOTE_MI_4, NOTE_FA_4, NOTE_FA_4, NOTE_SOL_4,
                               NOTE_SOL_4_, NOTE_LA_4, NOTE_LA_4, NOTE_SI_4};
/*                               
 * El tamaño de los arreglos de arriba para facilitarlo
 * debería ser 12 pero le puse 9 porque solo tenia 9 pushbutton
 * así los ultimos 3 no me los contará
 */
const uint8_t tamano = 9;                            

/* 
 * Variables para controlar las teclas.
 * Se utilizan bits para controlar sus estados siendo 
 * asi una variable tipo uint16_t puede controlar hasta 16 teclas
 * y una de tipo uint64_t podría con 64 teclas
 * 
 */
// controla el estado de los botones en la ultima iteración
uint16_t pasado = 0;
// controla el estado de los botones en la iteración actual
uint16_t actual = 0;

void setup() {
  // establezco los pines de entrada
  for (uint8_t i = 2; i < 14; i++) {
    pinMode(i, INPUT);
  }
  // inicio comunicacion serial con una baudio que deberá ser el mismo 
  // utilizado para leer esta señal serial
  Serial.begin(19200);
}

void loop() {

  // itero todos mis pines de entrada, si el botón está presionado
  // el bit que lo representa se vuelve 1, 0 en caso contrario
  for (uint8_t i = 0; i < tamano; i++) {
    if (digitalRead(PINES_ENTRADA[i]) == HIGH) {
      bitWrite(actual, 15 - i, 1);
    } else {
      bitWrite(actual, 15 - i, 0);
    }
  }

  // vuelvo a iterar para los pines que tengo
  for (uint8_t i = 0; i < tamano; i++) {
    uint8_t miBit = bitRead(actual, 15 - i);
    // si el bit que se utiliza para ese pun sufrió un cambio con su estado
    // es decir era 0 el loop pasado y se volvió 1 o viceversa
    // así se evita enviar comandos de más
    if (bitRead(pasado, 15 - i) != miBit) {
      // si el bit es 1 mando que inicie su nota respectiva
      // si es 0 mando que termine la nota
      if(miBit == 1) {
        MIDImessage(MIDI_NOTE_ON, PINES_NOTAS[i], 100);
      } else {
        MIDImessage(MIDI_NOTE_OFF, PINES_NOTAS[i], 100);
      }
    }
  }

  // actualizo mis variables
  pasado = actual;

  // evita el rebote de los botones y el sobreprocesamiento de datos
  delay(50);

}

/*
 * Manda un mensaje serial en formato MIDI
 * @param command Un comando MIDO
 * @param MIDInote La nota para el comando anterior
 * @param MIDIvelocity (0~127) establece el volumen de la nota
 */
void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  Serial.write(command);
  Serial.write(MIDInote);
  Serial.write(MIDIvelocity);
}

