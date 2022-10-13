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
#define CS1       D11
#define CS2       D12
#define CS3       D13
#define MOSI_PIN  D8
#define MISO_PIN  D10
#define SCK_PIN   D9
#define CLK16_PIN D1


#define TIMEOUT_MVMT 30000 //30 sec
#define TIMEOUT 10000 //10 sec

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
//Declare the 3 Drivers
TMC4361A ControllerZ(CS1,TGT1);
TMC4361A ControllerM(CS2,TGT2);
TMC4361A ControllerC(CS3,TGT3);

void setup() {
  //Initialize pins
  pin_init();

  //Initialize the RPC
  init_RPC();
  //set up Timer1 is generated by the M7 core
  //Initialize the drivers
  ControllerZ.begin();
  ControllerM.begin();
  ControllerC.begin();
  
  RPC.println("M4 setup done");

  getCurrentPosition();
}

void loop() {
  //Wait for call from the M7 core to do something
  switch(state){
    case 0: //standby state
      delay(100);
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
      init_driver(ControllerZ);
      init_driver(ControllerM);
      init_driver(ControllerC);
      if(!RPC.call("initDone").as<bool>())
          RPC.println("Error sending task completed");
      state = 0; //return to default state
      break;
      
    case 4: //Write_parameters
      break;
    case 5: //read parameters
      break;
    case 11: //manual Z move
      break;
    case 12: //Manual M move
      break;
    case 13: //Manual C move
      break;
    case 21: //Interrupt by tgt reached
      RPC.println("Z,M or C reached their targets");
      break;
    default:
      RPC.println("Wrong state");
      break;
  }
}
