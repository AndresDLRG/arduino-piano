#include "midi.h"

struct Cancion {
  char nombre[25];
  int notas[25];
  int tiempos[25];
  int silencios[25];
  int respuestas[25];
  int tamano;
};

#define MODO_LIBRE         0
#define MODO_APRENDIZAJE   1
#define MODO_ESPERA        2

#define PIN_ANT_OCTAVA    29
#define PIN_SIG_OCTAVA    28

#define PIN_DO            53
#define PIN_DO_           52
#define PIN_RE            51
#define PIN_RE_           50
#define PIN_MI            49
#define PIN_FA            48
#define PIN_FA_           47
#define PIN_SOL           46
#define PIN_SOL_          45
#define PIN_LA            44
#define PIN_LA_           43
#define PIN_SI            42

#define PIN_A             39
#define PIN_B             38
#define PIN_C             37
#define PIN_D             36
#define PIN_E             35
#define PIN_F             34
#define PIN_G             33
#define PIN_PUNTO         32

                          //abcdefg(punto)
#define DISPLAY_0         0b11111100
#define DISPLAY_1         0b01100000
#define DISPLAY_2         0b11011010
#define DISPLAY_3         0b11110010
#define DISPLAY_4         0b01100110
#define DISPLAY_5         0b10110110
#define DISPLAY_6         0b10111110
#define DISPLAY_7         0b11100000
#define DISPLAY_8         0b11111110
#define DISPLAY_9         0b11110110


/*
 * Este arreglo reduce mucho el código ya que se pone todo en ciclos
 * el índice de PINES_ENTRADA debe concidir con el índice de PINES_NOTAS.
 * Cada nota con su índice corresponde al pin con el mismo índice
 */
// pines a utilizar
// do, do#, re, re#, mi, fa, fa#, sol, sol#, la, la#, si
const uint8_t PINES_ENTRADA[] = {PIN_DO, PIN_DO_, PIN_RE, PIN_RE_,
                                 PIN_MI, PIN_FA, PIN_FA_, PIN_SOL,
                                 PIN_SOL_, PIN_LA, PIN_LA_, PIN_SI};
// que nota reproduce cada pin
const uint8_t PINES_NOTAS[] = {NOTE_DO_0, NOTE_DO_0_, NOTE_RE_0, NOTE_RE_0_,
                               NOTE_MI_0, NOTE_FA_0, NOTE_FA_0_, NOTE_SOL_0,
                               NOTE_SOL_0_, NOTE_LA_0, NOTE_LA_0_, NOTE_SI_0};

const uint8_t PINES_DISPLAY_SEGMENTOS[] = {
  PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G, PIN_PUNTO
};
/*
 * El tamaño de los arreglos de arriba para facilitarlo
 * debería ser 12 pero le puse 9 porque solo tenia 9 pushbutton
 * así los ultimos 3 no me los contará
 */
const uint8_t tamano = 12;

uint8_t octavaActual = 0;

/*
 * Variables para controlar las teclas.
 * Se utilizan bits para controlar sus estados siendo
 * asi una variable tipo uint16_t puede controlar hasta 16 teclas
 * y una de tipo uint64_t podría con 64 teclas
 *
 */
// controla el estado de los botones en la ultima iteración
uint16_t teclasPasado = 0;
// controla el estado de los botones en la iteración actual
uint16_t teclasActual = 0;

/*
 * Esto es para los botones de cambiar octavas
*/
uint8_t botonesPasado = 0;
uint8_t botonesActual = 0;

#include "canciones.h"
void reproducirCancion(Cancion);

void setup() {
  // establezco los pines de entrada
  for (uint8_t i = 0; i < tamano ; i++) {
    pinMode(PINES_ENTRADA[i], OUTPUT);
  }
  for (uint8_t i = 0; i < 8 ; i++) {
    pinMode(PINES_DISPLAY_SEGMENTOS[i], OUTPUT);
  }
  pinMode(PIN_SIG_OCTAVA, INPUT);
  pinMode(PIN_ANT_OCTAVA, INPUT);

  actualizarDisplaySegmentos(octavaActual);
  // inicio comunicacion serial con una baudio que deberá ser el mismo
  // utilizado para leer esta señal serial
  Serial.begin(19200);

  aprendizaje(&cancion2);

}

void loop() {
  delay(50);

}

/*
 * Manda un mensaje serial en formato MIDI
 * @param command Un comando MIDO
 * @param MIDInote La nota para el comando anterior
 * @param MIDIvelocity (0~127) establece el volumen de la nota
 */
void MIDImessage(uint8_t command, uint8_t MIDInote, uint8_t MIDIvelocity) {
  Serial.write(command);
  Serial.write(MIDInote);
  Serial.write(MIDIvelocity);
}

void MIDInoteOn(uint8_t MIDInote, uint8_t MIDIvelocity) {
  if (MIDInote  < 127)
    MIDImessage(MIDI_NOTE_ON, MIDInote, MIDIvelocity);
}

void MIDInoteOff(uint8_t MIDInote, uint8_t MIDIvelocity) {
  if (MIDInote  < 127)
    MIDImessage(MIDI_NOTE_OFF, MIDInote, MIDIvelocity);
}

/*
 * Aumenta en 1 la cotava actual
 */
void aumentarOctava() {
  if(octavaActual < 9) {
    octavaActual++;
  } else {
    octavaActual = 0;
  }
}

/*
 * Recue en 1 la octava actual
 */
void reducirOctava() {
  if(octavaActual > 0) {
    octavaActual--;
  } else {
    octavaActual = 9;
  }
}

/**
 * Cambia a una octava especifica
 */
void cambiarOctava(uint8_t octava) {
  octavaActual = octava;
}

/**
 * Actualiza el display de 7 segmentos con un numero del 0 al 9
 */
void actualizarDisplaySegmentos(uint8_t num) {
  switch (num) {
    case 0:
            cambiarDisplaySegmentos(DISPLAY_0);
            break;
    case 1:
            cambiarDisplaySegmentos(DISPLAY_1);
            break;
    case 2:
            cambiarDisplaySegmentos(DISPLAY_2);
            break;
    case 3:
            cambiarDisplaySegmentos(DISPLAY_3);
            break;
    case 4:
            cambiarDisplaySegmentos(DISPLAY_4);
            break;
    case 5:
            cambiarDisplaySegmentos(DISPLAY_5);
            break;
    case 6:
            cambiarDisplaySegmentos(DISPLAY_6);
            break;
    case 7:
            cambiarDisplaySegmentos(DISPLAY_7);
            break;
    case 8:
            cambiarDisplaySegmentos(DISPLAY_8);
            break;
    case 9:
            cambiarDisplaySegmentos(DISPLAY_9);
            break;
  }
}

/**
 * Cambia las luces especificas de cada segmento del led de 7 segmentos
 */
void cambiarDisplaySegmentos(uint8_t num) {
    for(uint8_t i = 0; i < 8; i++) {
      digitalWrite(PINES_DISPLAY_SEGMENTOS[i], bitRead(num, 7 - i));
    }
}

/**
 * Reproduce una cancion sin interacción humana
 */
void reproducirCancion(struct Cancion *cancion) {
  for (uint8_t i = 0; i < cancion->tamano; i++) {
    MIDInoteOn(cancion->notas[i], 100);
    delay(cancion->tiempos[i]);
    MIDInoteOff(cancion->notas[i], 100);
    delay(100);
  }
}

/**
 * Se encarga del modo libre
 */
void libre() {
  controlarOctavas();

  // itero todos mis pines de entrada, si el botón está presionado
  // el bit que lo representa se vuelve 1, 0 en caso contrario
  for (uint8_t i = 0; i < tamano; i++) {
    if (digitalRead(PINES_ENTRADA[i]) == HIGH) {
      bitWrite(teclasActual, 15 - i, 1);
    } else {
      bitWrite(teclasActual, 15 - i, 0);
    }
  }

  // vuelvo a iterar para los pines que tengo
  for (uint8_t i = 0; i < tamano; i++) {
    uint8_t miBit = bitRead(teclasActual, 15 - i);
    // si el bit que se utiliza para ese pun sufrió un cambio con su estado
    // es decir era 0 el loop pasado y se volvió 1 o viceversa
    // así se evita enviar comandos de más
    if (bitRead(teclasPasado, 15 - i) != miBit) {
      // si el bit es 1 mando que inicie su nota respectiva
      // si es 0 mando que termine la nota
      if(miBit == 1) {
        MIDInoteOn(PINES_NOTAS[i] + (octavaActual * tamano), 100);
      } else {
        MIDInoteOff(PINES_NOTAS[i] + (octavaActual * tamano), 100);
      }
    }
  }

  // Actualizo mis variables
  teclasPasado = teclasActual;

  // evita el rebote de los botones y el sobreprocesamiento de datos
  delay(50);
}

/**
 * Se encarga del modo de aprendizaje
 */
void aprendizaje(struct Cancion *cancion) {
  uint8_t nivelMax = cancion->tamano;
  uint8_t nivelMaxActual = 1;
  uint8_t nivelActual = 1;
  uint8_t flag = 0;
  
  while (flag == 0) {

    simonDice(cancion, nivelMaxActual);
    while(nivelMaxActual <= nivelMax) {
      controlarOctavas();

      // itero todos mis pines de entrada, si el botón está presionado
      // el bit que lo representa se vuelve 1, 0 en caso contrario
      for (uint8_t i = 0; i < tamano; i++) {
        if (digitalRead(PINES_ENTRADA[i]) == HIGH) {
          bitWrite(teclasActual, 15 - i, 1);
        } else {
          bitWrite(teclasActual, 15 - i, 0);
        }
      }

      // vuelvo a iterar para los pines que tengo
      for (uint8_t i = 0; i < tamano; i++) {
        uint8_t miBit = bitRead(teclasActual, 15 - i);
        // si el bit que se utiliza para ese pun sufrió un cambio con su estado
        // es decir era 0 el loop pasado y se volvió 1 o viceversa
        // así se evita enviar comandos de más
        if (bitRead(teclasPasado, 15 - i) != miBit) {
          // si el bit es 1 mando que inicie su nota respectiva
          // si es 0 mando que termine la nota
          if(miBit == 1) {
            MIDInoteOn(PINES_NOTAS[i] + (octavaActual * tamano), 100);
          } else {
            MIDInoteOff(PINES_NOTAS[i] + (octavaActual * tamano), 100);
            // detectar si presiono el boton correcto
            
            if(PINES_ENTRADA[i] == cancion->respuestas[nivelActual - 1]) {
              nivelActual++;
              //Serial.println("le atiné");
              if (nivelActual > nivelMaxActual){
                //Serial.println("subiendo dificultad");
                nivelMaxActual++;
                nivelActual = 1;
                delay(1000);
                simonDice(cancion, nivelMaxActual);
                break;
              }
            } else {
              // si se equivocó
              //Serial.println(PINES_ENTRADA[i]);
              delay(500);
              simonDice(cancion, nivelMaxActual);
              break;
            }
          }
        }
      }
      // Actualizo mis variables
      teclasPasado = teclasActual;
      delay(50);
    }
    flag = 1;
  }
  // aqui ya paso todo el simon dice   
}  

void simonDice(struct Cancion *cancion, uint8_t nivel) {
  for (uint8_t i = 0; i < nivel; i++) {
    MIDInoteOn(cancion->notas[i], 100);
    digitalWrite(cancion->respuestas[i], HIGH);
    delay(cancion->tiempos[i]);
    MIDInoteOff(cancion->notas[i], 100);
    digitalWrite(cancion->respuestas[i], LOW);
    delay(cancion->silencios[i]);
  }
}

/**
 * Controla el cambio de octavas y la actualización del led de 7 segmentos
 */
void controlarOctavas() {
  if (digitalRead(PIN_SIG_OCTAVA) == HIGH) {
    bitWrite(botonesActual, 7, 1);
  } else {
    bitWrite(botonesActual, 7, 0);
  }
  if (digitalRead(PIN_ANT_OCTAVA) == HIGH) {
    bitWrite(botonesActual, 6, 1);
  } else {
    bitWrite(botonesActual, 6, 0);
  }

  if (bitRead(botonesPasado, 7) == 0 && bitRead(botonesActual, 7) == 1) {
    aumentarOctava();
    actualizarDisplaySegmentos(octavaActual);
  }
  if (bitRead(botonesPasado, 6) == 0 && bitRead(botonesActual, 6) == 1) {
    reducirOctava();
    actualizarDisplaySegmentos(octavaActual);
  }

   botonesPasado = botonesActual;
}


