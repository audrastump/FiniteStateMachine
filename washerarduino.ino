#include <Arduino.h>
#include <Wire.h>
//Fill in the values below with the pins that you chose to use
const int POT_PIN = A0;
const int BUTTON_PIN = 4;
const int HOT_PIN = 6;
const int DRY_PIN = 8;
const int COLD_PIN = 10;
const int LOCK_PIN = 13;
/*
 * Pin to press button = 4
 * Pin to turn on hot wash = 6 - red LED
 * Pin to turn on dry = 8 - yellow LED
 * Pin to turn on cold wash = 10 - green LED
 * pin to lock the door= 13
 * Pin to do the potentiometer = A0
 */
enum State{economy, deluxeWash, superDeluxeWash,idle};

State currentState = idle;
int isPushed = 0;
void setup() {
Serial.begin(9600);
  pinMode(HOT_PIN,OUTPUT);
  pinMode(COLD_PIN,OUTPUT);
  pinMode(DRY_PIN,OUTPUT);
  pinMode(LOCK_PIN,OUTPUT);
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  pinMode(POT_PIN,INPUT);
}

void loop() { 
  currentState = changeSettings(currentState);
  if (int (digitalRead(BUTTON_PIN))==0){
    isPushed++;
  }
}
State changeSettings(State currentState){
  switch (currentState)
  {
    case idle:
    {
    //setting all the pins to low
    Serial.println("idle state");
    delay(3000);
    digitalWrite(HOT_PIN,LOW);
    digitalWrite(LOCK_PIN,LOW);
    digitalWrite(COLD_PIN,LOW);
    digitalWrite(DRY_PIN,LOW);
    double sensorValue = analogRead(POT_PIN);
    //only runs if the button is pressed 
    //if (int (digitalRead(BUTTON_PIN))==0)
    if (isPushed>0)
    {
      if(sensorValue>=400&&sensorValue<700){
      Serial.println("delux");
      currentState = deluxeWash; 
      return currentState;
      break; 
    }
    else if (sensorValue<=30){
      Serial.println("economy");
      currentState = economy;
      return currentState;
      break;
     }
    else if (sensorValue>700){
      Serial.println("super deluxe");
      currentState = superDeluxeWash;
      return currentState;
      break;
    }
    else{
      Serial.println("idle");
      currentState = idle;
      return currentState;
      break;
    }

    }
    currentState = idle;
    return currentState;
    break;
    }
  case economy:
  {
    //gotta lock the door and then turn on cold wash
    digitalWrite(LOCK_PIN,HIGH);
    digitalWrite(COLD_PIN,HIGH);
    //will run for 5 seconds
    delay(5000);
    //turn off cold wash and turn on the dryer 
    digitalWrite(COLD_PIN,LOW);
    double sensorValue = analogRead(POT_PIN);
    //if voltage is not in economy mode anymore, we will dry for 7 seconds
     if(sensorValue>300){
      Serial.println("deluxe dry");
      digitalWrite(DRY_PIN,HIGH);
      delay(7000);
      digitalWrite(DRY_PIN,LOW);
      currentState = idle;
      return currentState;
      break;
     }
     else{
      //let dry for a few seconds
      Serial.println("economy dry");
      digitalWrite(DRY_PIN,HIGH);
      delay(2000);
      digitalWrite(DRY_PIN,LOW);
     }
    currentState = idle;
    return currentState;
    break;
  }
  case deluxeWash: 
  {
     Serial.println("deluxe wash");
    //lock and turn on hot for 7 seconds
    digitalWrite(LOCK_PIN,HIGH);
    digitalWrite(HOT_PIN,HIGH);
    delay(7000);
    //turn off the hot water 
    digitalWrite(HOT_PIN,LOW);
    //calculate what the voltage should be
    double sensorValue = analogRead(POT_PIN);
    if(sensorValue<300){
      digitalWrite(DRY_PIN,HIGH);
      delay(2000);
      digitalWrite(DRY_PIN,LOW);
      currentState = idle;
      return currentState;
      break;
    }
    //if voltage is not in economy mode, we will dry 7 seconds
    digitalWrite(DRY_PIN,HIGH);
    delay(7000);
    digitalWrite(DRY_PIN,LOW);
    currentState = idle;
    return currentState;
    break;
  }
  case superDeluxeWash:
  {
     Serial.println("super deluxe wash");
    //Washes dishes in hot water for 7 minutes, then in medium water (both hot and cold) for 7 minutes, then finally runs the dryer for 7 minutes.
    //hot wash for 7 minutes
    digitalWrite(LOCK_PIN,HIGH);
    digitalWrite(HOT_PIN,HIGH);
    delay(7000);
    //medium wash for 7 minutes
    digitalWrite(HOT_PIN,HIGH);
    digitalWrite(COLD_PIN,HIGH);
    delay(7000);
    digitalWrite(HOT_PIN,LOW);
    digitalWrite(COLD_PIN,LOW);
    double sensorValue = analogRead(POT_PIN);
    //if voltage is in economy mode, we will dry for 2 seconds and break
    if(sensorValue<300){
      digitalWrite(DRY_PIN,HIGH);
      delay(2000);
      digitalWrite(DRY_PIN,LOW);
      currentState = idle;
      return currentState;
      break;
    }
    //dry for 7 minutes otherwise
    digitalWrite(DRY_PIN,HIGH);
    delay(7000);
    currentState = idle;
    return currentState;
    break;
    
  }
  
  }
}
