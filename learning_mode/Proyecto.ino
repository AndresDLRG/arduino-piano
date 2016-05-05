
/* 
  @author horaciogarza
*/

uint8_t pinButtons[8] = {1,2,3,4,5,6,7,8};
uint8_t LEDS[8] = {9,10,11,12,13,14,15,16};

int nobodyGotIt = 999;
int steps[8] = {8,4,2,1,5,3,7,6};
int stepNumber;
boolean finished = false;

void setup() {

  for(int i = 0 ; i < sizeof(pinButtons) ; i++){
    pinMode(pinButtons[i], INPUT);

  }

  for(int i = 0 ; i < sizeof(LEDS) ; i++){
    pinMode(LEDS[i], OUTPUT);
  }
  
  stepNumber = 0;

}


void loop() {

  digitalWrite(LEDS[stepNumber], 1);
  uint8_t whoGotTheP = getPressedButtonID();
  
  letTheLEDWakeUp(whoGotTheP);
  if(finished){
    learnFinishedAnimation();
  }
  
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

  if(LEDS[pint] == steps[stepNumber]){

    digitalWrite(LEDS[stepNumber], 0);
    if(stepNumber == sizeof(steps)){
      finished = true
      
    }else{
      stepNumber++;
      digitalWrite(LEDS[stepNumber], 1);
      
    }
    
    
  }else{
    
    digitalWrite(LEDS[stepNumber], 0);
    delay(250);
    digitalWrite(LEDS[stepNumber], 1);
    delay(250);
    digitalWrite(LEDS[stepNumber], 0);
    delay(250);
    digitalWrite(LEDS[stepNumber], 1);
    delay(100);
  }

}

void learnFinishedAnimation(){
  for(int i = 0 ; i < (sizeof(LEDS) - 1) ; i++){
    digitalWrite(LEDS[i], 1);
    delay(200);
    digitalWrite(LEDS[i], 0);
  }
}



