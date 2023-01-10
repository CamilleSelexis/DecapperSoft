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
//#define STALL_PIN D5
#define TGT1      D2
#define TGT2      D3
#define TGT3      D4
#define CS1       D5
#define CS2       D6
#define CS3       D7
#define MOSI_PIN  D8
#define MISO_PIN  D10
#define SCK_PIN   D9
#define CLK16_PIN D0
#define RELAY_PIN A0

#define TIMEOUT_MVMT 30000 //30 sec
#define TIMEOUT 10000 //10 sec
#define TIME_UPDATE 100 //100 ms -> we want to update position and target at this rate
#define SCREW_TIME 6 //6 sec to screw/unscrew the cap

#define USTEPS  256 //number of usteps by full steps
#define STEP_TURN 200 //number of full steps in a single turn of the axis

#define ZGEAR   5
#define CGEAR   51
const float CTRANS =  1.25;
#define MGEAR   51
#define F_CPU       200000000
uint32_t ExtClk;

long Cpos = 0;
long *PCpos = &Cpos;
int state = 0; //State used by the finite state machine
int* Pstate = &state;

bool Zrunning = false;
bool Mrunning = false;
bool Crunning = false;
#define CLEAR_RUNNING Zrunning = false;Mrunning=false;Crunning = false;
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
float ZAngleEnc = 0;
float ZTurnEnc = 0;
long ZDevEnc = 0;
long MPos = 0;
long MTarget = 0;
long MPosEnc = 0;
float MAngleEnc = 0;
float MTurnEnc = 0;
long MDevEnc = 0;
long CPos = 0;
long CTarget = 0;
long CPosEnc = 0;
float CAngleEnc = 0;
float CTurnEnc = 0;
long CDevEnc = 0;
//For long value transfer from RPC
long Zvalue = 0;
long *pZvalue = &Zvalue;
long Mvalue = 0;
long *pMvalue = &Mvalue;
long Cvalue = 0;
long *pCvalue = &Cvalue;
//For int value transfer from RPC
int iZvalue = 0;
int *piZvalue = &iZvalue;
int iMvalue = 0;
int *piMvalue = &iMvalue;
int iCvalue = 0;
int *piCvalue = &iCvalue;
//Declare the 3 Drivers
TMC4361A ControllerZ(CS1,TGT1);
TMC4361A ControllerM(CS2,TGT2);
TMC4361A ControllerC(CS3,TGT3);
TMC4361A *pControllerZ = &ControllerZ;
TMC4361A *pControllerM = &ControllerM;
TMC4361A *pControllerC = &ControllerC;
void setup() {
  //Initialize pins
  pin_init();
  digitalWrite(LEDR,LOW);
  //Initialize the RPC
  init_RPC();
  //set up Timer1 is generated by the M7 core
  //Initialize the drivers
  ControllerZ.begin();
  ControllerM.begin();
  ControllerC.begin();
  RPC.println("M4 setup done");
  RPC.call("M4TaskCompleted").as<bool>();
  //getCurrentPosition();
  digitalWrite(LEDR,HIGH);
  delay(1000);
  //ControllerC.setTargetRelative(512000);
}

void loop() {
  //Wait for call from the M7 core to do something
  bool Zstate = 0;
  bool Mstate = 0;
  bool Cstate = 0;
  uint32_t valueEnc = 0;
  switch(state){
    case 0: //standby state
      delay(250);
      updateValues(); //Nothing to update anyway
      //RPC.println(ControllerZ.isTargetReached());
      //RPC.println(ControllerC.isTargetReached());
     /*valueEnc = ControllerM.getEncoderRaw();
      RPC.print("Encoder Turn : ");RPC.print((valueEnc & 0xfffe0000)>>17,HEX);
      RPC.print(" Angle : ");RPC.println(valueEnc & 0x1ffff);
      //RPC.println(ControllerM.getEncoderRaw(),HEX);
      //test_motors();*/
      break;
      
    case 1: //Decap
      RPC.print("Launching Decap with Cpos : ");RPC.println(*PCpos);
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
      Zstate = init_driver(pControllerZ);
      RPC.println("Axis M");
      Mstate = init_driver(pControllerM);
      RPC.println("Axis C");
      Cstate = init_driver(pControllerC);
      
      if(!RPC.call("initDone",Zstate,Mstate,Cstate).as<bool>())
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
      RPC.print("M4: M relMove");;RPC.println(Mvalue);
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
    case 41:
      RPC.print("M4 : setting Z VMAX");RPC.println(Zvalue);
      ZsetVMAX(Zvalue);
      if(!RPC.call("M4TaskCompleted").as<bool>())
        RPC.println("Error sending task completed");
      state = 0; //return to default state
      Zvalue = 0; //Clear Zvalue
      break;
    case 42:
      RPC.print("M4 : setting M VMAX");RPC.println(Mvalue);
      MsetVMAX(Mvalue);
      if(!RPC.call("M4TaskCompleted").as<bool>())
        RPC.println("Error sending task completed");
      state = 0; //return to default state
      Mvalue = 0;
    case 43:
      RPC.print("M4 : setting C VMAX");RPC.println(Cvalue);
      CsetVMAX(Cvalue);
      if(!RPC.call("M4TaskCompleted").as<bool>())
        RPC.println("Error sending task completed");
      state = 0; //return to default state
      Cvalue = 0;
      break;
    case 51:
      RPC.print("M4 : setting Z CS");RPC.println(iZvalue);
      ZsetCurrentScaling(iZvalue);
      if(!RPC.call("M4TaskCompleted").as<bool>())
        RPC.println("Error sending task completed");
      state = 0; //return to default state
      iZvalue = 0;
      break;
    case 52:
      RPC.print("M4 : setting M CS");RPC.println(iMvalue);
      MsetCurrentScaling(iMvalue);
      if(!RPC.call("M4TaskCompleted").as<bool>())
        RPC.println("Error sending task completed");
      state = 0; //return to default state
      iMvalue = 0;
      break;
    case 53:
      RPC.print("M4 : setting C CS");RPC.println(iCvalue);
      CsetCurrentScaling(iCvalue);
      if(!RPC.call("M4TaskCompleted").as<bool>())
        RPC.println("Error sending task completed");
      state = 0; //return to default state
      iCvalue = 0;
      break;
    case 101:
      initControllers();
      break;
    default:
      RPC.println("Wrong state");
      break;
  }
}
