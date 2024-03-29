/*
 * Arduino Portenta script written for the M4 core
 * Programmed by Camille Aussems - Selexis Geneva 17.01.2023
 * V1.0
 * 
 * The M4 core is the one in charge of the parametrisation of the Trinamic controllers
 * It acts as a slave to the M7 core and relay the different movement to the controllers
 * Based on version 3.3 of core-mbed
 */
#include <SPI.h>
#include "TMC4361A_Register.h"
#include "Arduino.h"
#include "TMC4361A.h"
#include "RPC.h"

#define DECAP_ID 2
//Defines the 0 position in encoder absolute position for each axis
//Z -> 6000000 = 129mm from base
//M -> 1000000 = Mors open by 6 mm
//C ->0 =  one of the mors aligned on the rail
//Decapper 1
#if DECAP_ID == 1
  #define Z_ZERO  3349898579
  #define M_ZERO  3352848773 // 502164 -3000000 = 0
  #define C_ZERO  2429052
#endif
//Decapper 2
#if DECAP_ID == 2
// 3354193706  3355753764 - //310554  1935450 // 60626 1685450Encoder  3355434317
  #define Z_ZERO  3354241560  //3353994826 - 237440 = 3’353’757’386
  #define M_ZERO  3353121560
  #define C_ZERO  3324343084  

#endif
//Decapper 3
#if DECAP_ID == 3
  #define Z_ZERO  3347621535
  #define M_ZERO  34595755   //2900000 = 3357511005 0 = 3357511005 -2900000 = 3354611005
  #define C_ZERO  3222505772  

#endif
//Decapper 4
#if DECAP_ID == 4
  #define Z_ZERO  3340374280
  #define M_ZERO  3351159189
  #define C_ZERO  3352498375  

#endif
#define DRIVER_ON pinMode(EN_PIN,OUTPUT);digitalWrite(EN_PIN,LOW);
#define DRIVER_OFF pinMode(EN_PIN,INPUT);
#define LEDR_ON digitalWrite(LEDR,LOW);
#define LEDR_OFF digitalWrite(LEDR,HIGH);
#define EN_PIN  D14
//#define STALL_PIN D5
#define TGT_Z      D4
#define TGT_M      D3
#define TGT_C      D2
#define CS_Z       D7
#define CS_M       D6
#define CS_C       D5
#define MOSI_PIN  D8
#define MISO_PIN  D10
#define SCK_PIN   D9
#define CLK16_PIN D0
#define RELAY_PIN A0

#define TIMEOUT_MVMT 15000 //15 sec
#define TIMEOUT 10000 //10 sec
#define TIME_UPDATE 100 //100 ms -> we want to update position and target at this rate

#define USTEPS  256 //number of usteps by full steps
#define STEP_TURN 200 //number of full steps in a single turn of the axis

#define ZGEAR   3.71  // Gear ratio of the gearbox
#define ZTRANS  2     //Gear ratio between the 2 gears in the cap
#define ZSCREWSTEP  2 //mm by turn

#define CGEAR   51.89
#define CTRANS  1.25

#define MGEAR   51.89
#define F_CPU       200000000
uint32_t ExtClk;

long CAlignValue = 0;
long *pCAlignValue = &CAlignValue;
int state = 0; //State used by the finite state machine
int* Pstate = &state;

bool Zrunning = false;
bool Mrunning = false;
bool Crunning = false;
bool stopRoutine = false;
bool CmdResult = false;
bool decapFlag = false; //Set to true during decap
bool recapFlag = false; //Set to true during recap
int checkpoints = 0; //This value is incremented before each move so that we have a trace of where we are in the process
#define CLEAR_RUNNING Zrunning = false;Mrunning=false;Crunning = false;

//Z positions
uint32_t standbyZ = 6000000; //above the bottle so that precise can take and bring new bottles
uint32_t standbyZAfterDecap = 6000000; // Minimal clearance so that the precise can remove the basket without its cap
uint32_t Znear = 9000000; //Just above the cap
uint32_t capHeight = 11800000; //Cap pressed

//M positions
uint32_t standbyM = 1000000;
uint32_t Mopen = 1000000;
uint32_t capRelease = 1000000; //Release the cap = open
uint32_t capHold = 2900000; //Gripped on the cap without little spikes 2900000
uint32_t capNear = 1800000;
long uSToTurnC = ceil(STEP_TURN*CGEAR*CTRANS*USTEPS);
//C positions
uint32_t standbyC = 0;

//Screw parameters
#define SCREW_TIME 4 //4 sec to screw/unscrew the cap
float capThread = 8; //mm/turn was 6
float unscrewRot = 0.7;//turn -> rotation necessary to unscrew the cap 0.8->0.86
//float scaling = 1.1; //Variable used to proportionnaly increase both Z and C movements amplitude
uint32_t ZUnscrew = ceil(capThread*unscrewRot*ZGEAR*ZTRANS*STEP_TURN*USTEPS/ZSCREWSTEP); //Z relative movement to unscrew/screw -> 1000000 steps
uint32_t ZScrewingPos = capHeight-ZUnscrew; //position corresponding to end of unscrew movement/start of screw
long CUnscrew = -ceil(unscrewRot*CGEAR*CTRANS*STEP_TURN*USTEPS); //C relative movement to unscrew/screw -> 4500000 steps
long CScrew = ceil(unscrewRot*CGEAR*CTRANS*STEP_TURN*USTEPS); //Screw is 1% more than unscrew


//Current motor values
long ZPos = 0;
long ZTarget = 0;
uint32_t ZPosEnc = 0;
float ZAngleEnc = 0;
float ZTurnEnc = 0;
long ZDevEnc = 0;
long MPos = 0;
long MTarget = 0;
uint32_t MPosEnc = 0;
float MAngleEnc = 0;
float MTurnEnc = 0;
long MDevEnc = 0;
long CPos = 0;
long CTarget = 0;
uint32_t CPosEnc = 0;
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
TMC4361A ControllerZ(CS_Z,TGT_Z);
TMC4361A ControllerM(CS_M,TGT_M);
TMC4361A ControllerC(CS_C,TGT_C);
TMC4361A *pControllerZ = &ControllerZ;
TMC4361A *pControllerM = &ControllerM;
TMC4361A *pControllerC = &ControllerC;
void setup() {
  long timer_init = millis();
  //Initialize pins
  pin_init();
  digitalWrite(LEDR,LOW);
  //Initialize the RPC
  init_RPC();
  //set up Timer1 is generated by the M7 core
  //Initialize the controllers and drivers
  ControllerZ.begin();
  ControllerM.begin();
  ControllerM.setCurrentScale(12);
  ControllerC.begin();
  //init encoders
  DRIVER_ON;
  ControllerZ.init_CLPosital(Z_ZERO);
  ControllerM.init_CLPosital(M_ZERO);
  ControllerC.init_CLPosital(C_ZERO);
  DRIVER_OFF;
  RPC.println("M4 setup done");
  RPC.call("M4TaskCompleted").as<bool>();
  digitalWrite(LEDR,HIGH);
  RPC.println(millis()-timer_init);
  delay(1000);
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
      //RPC.print("Multiturn = ");RPC.print(MPosEnc>>17);RPC.print(" SingleTurn = ");RPC.println(MPosEnc & 0x1FFFF);
      break;
      
    case 1: //Decap
      RPC.print("Launching Decap with Cpos : ");RPC.println(*pCAlignValue);
      CmdResult = decap(0);
      if(!RPC.call("decapDone",CmdResult).as<bool>())
        RPC.println("Error sending task completed");
      DRIVER_OFF;
      state = 0; //return to default state
      //send decap done to M7
      break;
      
    case 2: //Recap
      RPC.print("Launching recap routine");
      CmdResult = recap(0);
      if(!RPC.call("recapDone",CmdResult).as<bool>())
        RPC.println("Error sending task completed");
      DRIVER_OFF;
      state = 0; //return to default state

      //send recap done to M7
      break;
      
    case 3: //Init_driver
      //If calibration failed, redo it
      DRIVER_ON;
      stopRoutine = false;
      if(ControllerZ.isEncoderFail()){
        ControllerZ.init_CLPosital(Z_ZERO);
        RPC.println(" Z Encoder fail");
      }
      delay(50);
      if(ControllerM.isEncoderFail()){
        ControllerM.init_CLPosital(M_ZERO);
        RPC.println(" M Encoder fail");
      }
      delay(50);
      if(ControllerC.isEncoderFail()){
        ControllerC.init_CLPosital(C_ZERO);
        RPC.println(" C Encoder fail");
      }
      delay(50);
      goToInitPos();
      setLowSpeed();
      RPC.println("Axis Z");
      Zstate = init_driver(pControllerZ);
      RPC.println("------------------");
      RPC.println("Axis M");
      Mstate = init_driver(pControllerM);
      RPC.println("-------------------");
      RPC.println("Axis C");
      Cstate = init_driver(pControllerC);
      RPC.println("-------------------");
      DRIVER_OFF;
      setDefaultSpeed();
      stopRoutine = false;
      delay(500); //To properly print all messages
      if(!RPC.call("initDone",Zstate,Mstate,Cstate).as<bool>())
          RPC.println("Error during init procedure");
      state = 0; //return to default state
      break;
      
    case 4: //Write_parameters
      break;
    case 5: //read parameters
      break;

    case 6: //resume Moves
      RPC.print("M4 resuming last move");
      resumeMoves();
      if(!RPC.call("M4TaskCompleted").as<bool>())
        RPC.println("Error sending task completed");
      state = 0;
      break;
    case 7: //abort Moves
      RPC.print("M4 abort");
      abortMoves();
      if(!RPC.call("M4TaskCompleted").as<bool>())
        RPC.println("Error sending task completed");
      state = 0;
      break;
      
    case 11: //manual Z relMove
      RPC.print("M4: Moving Axis Z by ");RPC.println(Zvalue);
      DRIVER_ON;
      ZrelMove(Zvalue);
      DRIVER_OFF;
      if(!RPC.call("M4TaskCompleted").as<bool>())
          RPC.println("Error sending task completed");
      state = 0; //return to default state
      break;
    case 12: //Manual M relMove
      RPC.print("M4: M relMove");;RPC.println(Mvalue);
      DRIVER_ON;
      MrelMove(Mvalue);
      DRIVER_OFF;
      if(!RPC.call("M4TaskCompleted").as<bool>())
          RPC.println("Error sending task completed");
      state = 0; //return to default state
      break;
    case 13: //Manual C relMove
      RPC.print("M4: Moving Axis C");;RPC.println(Cvalue);
      DRIVER_ON;
      CrelMove(Cvalue);
      DRIVER_OFF;
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
    RPC.println("Init Controllers");
      initControllers();
      state =0;
      break;
    default:
      RPC.println("Wrong state");
      break;
  }
}
