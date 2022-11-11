/*
 * M4Decapv1 by Camille Aussems
 * Selexis Geneva 26.08.2022
 * To run on an arduino portenta H7 with vision shield
 * Drive up to 3 driver/controller TMC4361A - TMC2660
 * Upload on M4 core
 * Based on version 3.3 of core-mbed
 */
#include <SPI.h>
#include "TMC4361A_Register.h"
#include "Arduino.h"
#include "TMC4361A.h"
#include "RPC.h"

#define EN_PIN  D14
#define STALL_PIN D5

#define TGT1      D4
#define TGT2      D5
#define TGT3      D6
#define RST1      D4
#define RST2      D5
#define RST3      D6
#define CS1       D11
#define CS2       D12
#define CS3       D13
#define MOSI_PIN  D8
#define MISO_PIN  D10
#define SCK_PIN   D9
#define CLK16_PIN D1

#define TIMEOUT_MVMT 30000 //30 sec
#define TIMEOUT 10000 //10 sec
#define TIME_UPDATE 500 //500 ms -> we want to update position and target at this rate
#define SCREW_TIME 6 //6 sec to screw/unscrew the cap

#define USTEPS  256 //number of usteps by full steps
#define STEP_TURN 200 //number of full steps in a single turn of the axis

#define ZGEAR   5
#define CGEAR   51
const float CTRANS =  1.25;
#define MGEAR   51
#define F_CPU       200000000
uint32_t ExtClk;

int Cpos = 0;
int *PCpos = &Cpos;
int state = 0; //State used by the finite state machine
int* Pstate = &state;

//Z positions
uint32_t standbyZ = 1000*USTEPS;
uint32_t Znear = 2000*USTEPS;
uint32_t capHeight = 4000*USTEPS;
uint32_t capDecapZ = 5000*USTEPS;
uint32_t screwStartZ = 4000*USTEPS;

//M positions
uint32_t standbyM = 2500*USTEPS;
uint32_t Mopen = 2000*USTEPS;
uint32_t capRelease = 2000*USTEPS;
uint32_t capHold = 1000*USTEPS;

//C positions
uint32_t standbyC = 0*USTEPS;
uint32_t capDecapC = 2000*USTEPS;

//Current motor values
long ZPos = 0;
long ZTarget = 0;
long ZPosEnc = 0;
long ZAngleEnc = 0;
long MPos = 0;
long MTarget = 0;
long MPosEnc = 0;
long MAngleEnc = 0;
long CPos = 0;
long CTarget = 0;
long CPosEnc = 0;
long CAngleEnc = 0;

long Zvalue = 0;
long *pZvalue = &Zvalue;
long Mvalue = 0;
long *pMvalue = &Mvalue;
long Cvalue = 0;
long *pCvalue = &Cvalue;
//Declare the 3 Drivers
TMC4361A ControllerZ(CS1,TGT1);
TMC4361A ControllerM(CS2,TGT2);
TMC4361A ControllerC(CS3,TGT3);

void setup() {
  //Initialize pins
  //delay(2000);
  pin_init();
  digitalWrite(LEDR,LOW);
  //Initialize the RPC
  init_RPC();
  //set up Timer1 is generated by the M7 core
  //Initialize the drivers
  ControllerZ.begin();
  ControllerM.begin(); //Encoder is plugged
  ControllerC.begin();
  
  RPC.println("M4 setup done");
  if(RPC.call("M4TaskCompleted").as<bool>())
    RPC.println("Good day to you sir");
  //getCurrentPosition();
  digitalWrite(LEDR,HIGH);
  delay(1000);
  //pin_init();
}

void loop() {
  //Wait for call from the M7 core to do something
  switch(state){
    case 0: //standby state
      delay(250);
      updateValues(); //Nothing to update anyway
      //test_motors();
      break;
      
    case 1: //Decap
      decap();
      if(!RPC.call("decapDone").as<bool>())
        RPC.println("Error sending task completed");
      state = 0; //return to default state
      //send decap done to M7
      break;
      
    case 2: //Recap
      recap();
      if(!RPC.call("recapDone").as<bool>())
        RPC.println("Error sending task completed");
      state = 0; //return to default state
      //send recap done to M7
      break;
      
    case 3: //Init_drivers
    RPC.println("Axis Z");
      init_driver(ControllerZ);
      RPC.println("Axis M");
      init_driver(ControllerM);
      RPC.println("Axis C");
      init_driver(ControllerC);
      
      if(!RPC.call("initDone").as<bool>())
          RPC.println("Error sending task completed");
      state = 0; //return to default state
      break;
      
    case 4: //Write_parameters
      break;
    case 5: //read parameters
      break;
    case 11: //manual Z relMove
      RPC.print("M4: Moving Axis Z by ");RPC.println(Zvalue);
      ZrelMove(Zvalue);
      if(!RPC.call("M4TaskCompleted").as<bool>())
          RPC.println("Error sending task completed");
      state = 0; //return to default state
      break;
    case 12: //Manual M relMove
      RPC.print("M4: Moving Axis M");;RPC.println(Mvalue);
      MrelMove(Mvalue);
      if(!RPC.call("M4TaskCompleted").as<bool>())
          RPC.println("Error sending task completed");
      state = 0; //return to default state
      break;
    case 13: //Manual C relMove
      RPC.print("M4: Moving Axis C");;RPC.println(Cvalue);
      CrelMove(Cvalue);
      if(!RPC.call("M4TaskCompleted").as<bool>())
          RPC.println("Error sending task completed");
      state = 0; //return to default state
      break;
    case 21: //Interrupt by tgt reached Z
      RPC.println("Z reached its targets");
      state = 0;
      break;
    case 22: //Interrupt by tgt reached M
      RPC.println("M reached its targets");
      state = 0;
      break;
    case 23: //Interrupt by tgt reached C
      RPC.println("C reached its targets");
      state = 0;
      break;
    case 31: //Send Z current position to the M7
      RPC.println(ControllerZ.getCurrentPos());
      state = 0;
      break;
    case 32: //Send M current position to the M7
      RPC.println(ControllerM.getCurrentPos());  
      state = 0;
      break;
    case 33: //Send C current position to the M7
      RPC.println(ControllerC.getCurrentPos());
      state = 0;
      break;
    default:
      RPC.println("Wrong state");
      break;
  }
}
