
/* 
  @author horaciogarza
*/

uint8_t pinButtons[8] = {1,2,3,4,5,6,7,8};
uint8_t LEDS[8] = {9,10,11,12,13,14,15,16};

int nobodyGotIt = 999;
int steps[8] = {8,4,2,1,5,3,7,6};

void setup() {

  for(int i = 0 ; i < sizeof(pinButtons) ; i++){
    pinMode(pinButtons[i], INPUT);

  }

  for(int i = 0 ; i < sizeof(LEDS) ; i++){
    pinMode(LEDS[i], OUTPUT);
  }

}


void loop() {

  uint8_t whoGotTheP = getPressedButtonID();
  letTheLEDWakeUp(whoGotTheP);
  
}


int getPressedButtonID(){


  for(int i = 0 ; i < sizeof(pinButtons) ; i++){

    if(digitalRead(pinButtons[i]) == 1){
      return i;
    }
  }
  
  return nobodyGotIt;
    

}

void letTheLEDWakeUp(int pint){

  if(pint != nobodyGotIt){
    digitalWrite(LEDS[pint], !digitalRead(LEDS[pint]));
  }

}



