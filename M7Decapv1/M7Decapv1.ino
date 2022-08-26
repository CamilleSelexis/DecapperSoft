#include <SPI.h>
#include "TMC4361A_Register.h"
#include "Arduino.h"
#include "PwmOut.h"
#include "pinDefinitions.h"
#include "PinNames.h"
#define _TIMERINTERRUPT_LOGLEVEL_ 0
#include "Portenta_H7_TimerInterrupt.h"
#include "TMC4361A.h"

#define EN_PIN  D14
#define STALL_PIN D5

#define TGT1      D4
#define TGT2      D5
#define TGT3      D6
#define CS1       D11
#define CS2       D12
#define CS3       D13
#define MOSI_PIN  D8
#define MISO_PIN  D10
#define SCK_PIN   D9
#define CLK16_PIN D1

#define F_CPU       200000000
uint32_t ExtClk;

TMC4361A ControllerZ(CS1,TGT1);
TMC4361A ControllerX(CS2,TGT2);
void setup() {
  //set Pins
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); //deactivate driver (LOW active)
  //pinMode(STALL_PIN,INPUT);
  
  pinMode(CS1, OUTPUT);
  digitalWrite(CS1, HIGH);
  pinMode(CS2, OUTPUT);
  digitalWrite(CS2, HIGH);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(CLK16_PIN,OUTPUT);

  //set up Timer1 for clk generation
  mbed::PwmOut* pwm = new mbed::PwmOut(digitalPinToPinName(CLK16_PIN));
  digitalPinToPwm(CLK16_PIN) = pwm;
  TIM1->PSC = 0;
  TIM1->ARR = 12; //Freq = F_CPU/(ARR*(PSC+1)) -> 200/12 = 16.7 MHz
  TIM1->CCR1 = 6; //Used to define the duty cycle D1 HIGH when CNT<CCR1 & D1 LOW when CNT>CCR1
  ExtClk = F_CPU/(12);
  //init serial port
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Setup start");
  ControllerZ.begin();
  ControllerX.begin();

  ControllerZ.setTarget(200*256*20);
  ControllerX.setTarget(200*256*5);
  digitalWrite(EN_PIN,LOW);
  Serial.println("Setup done");
}

void loop() {
  static uint32_t last_time = 0;
  static uint32_t last_time2 = 0;
  uint32_t ms = millis();
  uint32_t data;

  if((ms-last_time)> 2000){
    last_time = ms;
    Serial.println("------------ Z --------------");
    Serial.print("Xactual = ");Serial.print(ControllerZ.readRegister(TMC4361A_XACTUAL));
    Serial.print(" Xtarget = ");Serial.println(ControllerZ.readRegister(TMC4361A_X_TARGET));
    Serial.print("Encoder angle = ");Serial.println(ControllerZ.getEncoderAngle());
    Serial.print("Encoder turn = ");Serial.println(ControllerZ.getEncoderTurn());
    Serial.println("----------------------");
    Serial.println("------------ X --------------");
    Serial.print("Xactual = ");Serial.print(ControllerX.readRegister(TMC4361A_XACTUAL));
    Serial.print(" Xtarget = ");Serial.println(ControllerX.readRegister(TMC4361A_X_TARGET));
    Serial.print("Encoder angle = ");Serial.println(ControllerX.getEncoderAngle());
    Serial.print("Encoder turn = ");Serial.println(ControllerX.getEncoderTurn());
    Serial.println("----------------------");

    if(ControllerZ.isTargetReached()){
      ControllerZ.setTargetRelative(200*256*20);//20 turns
    }
    if(ControllerX.isTargetReached()){
      ControllerX.setTargetRelative(200*256*5);
    }
  }
}
