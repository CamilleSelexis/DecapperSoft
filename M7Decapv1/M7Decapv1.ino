#include <SPI.h>
#include "TMC4361A_Register.h"
#include "Arduino.h"
#include "PwmOut.h"
#include "pinDefinitions.h"
#include "PinNames.h"
#define _TIMERINTERRUPT_LOGLEVEL_ 0
#include "Portenta_H7_TimerInterrupt.h"
#include "TMC4361A.h"
#include <Portenta_Ethernet.h>
#include <Ethernet.h>
#include <math.h>
#include <stdio.h>
#include "RPC.h"
#include "camera.h"
#include "himax.h"

#define DECAP_ID  1
//Better to connect it to drv_enn of the TMC2660 to power off the mosfet and reduce heat generation
/*Unconnected pin on the portenta board
 * A0-A6
 * D2-D3
 * D7
 * D11-D12 used by vision shield camera
 * A5-A6 used by vision shield
 * D1 used for a timer?
 */
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

#define F_CPU       200000000
uint32_t ExtClk;

#define RELAY_ON digitalWrite(RELAY_PIN,HIGH);
#define RELAY_OFF digitalWrite(RELAY_PIN,LOW);

#define LEDG_ON  digitalWrite(LEDG,LOW);
#define LEDG_OFF digitalWrite(LEDG,HIGH);
#define LEDR_ON  digitalWrite(LEDR,LOW);
#define LEDR_OFF digitalWrite(LEDR,HIGH);
#define LEDB_ON  digitalWrite(LEDB,LOW);
#define LEDB_OFF digitalWrite(LEDB,HIGH);

#define USTEPS  256 //number of usteps by full steps
#define STEP_TURN 200 //number of full steps in a single turn of the axis

#define RELMOVE_LIMIT 10000000 //Max value rel move accepts
#define CS_LIMIT 32 //max value setCurrentScaling accepts
#define VMAX_LIMIT 1700000 //max value setVMAX accepts
//State variables ------------------------------------------------------------------------
bool HS = false; //If the decapper needs to be ignored

bool isInit = false;          //Is the device init
bool *isInit_pntr = &isInit;      //Goes true after the init routine

bool capHeld = false;           //Goes true after a decap and false after a recap
bool *capHeld_pntr = &capHeld;

volatile bool M4work = true;          //Cette variable est vrai lorsque le M4 effectue une tache
volatile bool *M4work_pntr = &M4work;     //N'est pas utilisé pour le moment mais pourrait être utile

long task_start_time = 0; //variable to store the time needed to perform a given task
//Values received from the M4 core

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

bool Zstate = false;
bool Mstate = false;
bool Cstate = false;
//Camera related variables --------------------------------------------------------------
const int imgH = 240; //Y dimension
const int imgW = 320; //X dimension
HM01B0 himax;
Camera cam(himax);
#define IMAGE_MODE CAMERA_GRAYSCALE
#define RESOLUTION CAMERA_R320x240
FrameBuffer FB(imgH,imgW,1);

uint8_t fb[imgW*imgH];        //Buffer for the image capture
uint8_t *Pfb = fb; 

const int cropx[2] = {230,250};    //Size of the cropped image in the width
const int cropy[2] = {100,200};   //Size of the cropped image in the height
const int ly = cropy[1]-cropy[0]; //Length of the cropped dimmensions
const int lx = cropx[1]-cropx[0];

const long calibration = 1000*USTEPS;    //Rotation offset - 9267 is the distance between the bumps
const int caliProp = 28; //Factor for the calibration based on the dist from the center

long uSToTurnC = ceil(STEP_TURN*51.89*1.25*USTEPS);
//Light depending parameters for the image detection (will probably need a tweek for each environement)
//Try to change thres and n to have the minimum amount of markers detected while never having 0 of them.
const uint8_t thres = 8;         //Threshold of image detection 2->10
const uint8_t n = 2;             //Size of the moving average avg done on 2n+1

//Ethernet related variables ----------------------------------------------------
byte mac[] = {0xDE, 0xA1, DECAP_ID, 0x73, 0x24, 0x12};  //Mac adress


uint8_t ip_addr[4] = {192,168,1,100+DECAP_ID}; //101,102,103,104
String StringIP = String(ip_addr[0]) + "." + String(ip_addr[1]) + "." + String(ip_addr[2]) + "." + String(ip_addr[3]);
IPAddress ip(ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]); //
EthernetServer server = EthernetServer(80);  // (port 80 is default for HTTP) 52 is the number of the lab
#define TIMEOUT_ETH  2000 //timeout after 2 s
void setup() {
  //set Pins
  pinMode(CLK16_PIN,OUTPUT);
  LEDG_ON;
  //pinMode(EN_PIN,OUTPUT);
  //digitalWrite(EN_PIN,HIGH);
  //set up Timer8 for clk generation
  mbed::PwmOut* pwm = new mbed::PwmOut(digitalPinToPinName(CLK16_PIN)); //D0 TIM8 CH3N
  digitalPinToPwm(CLK16_PIN) = pwm;
  TIM8->PSC = 0;
  TIM8->ARR = 10; //Freq = F_CPU/(ARR*(PSC+1)) -> 200/10 = 20 MHz
  TIM8->CCR3 = 5; //Used to define the duty cycle D1 HIGH when CNT<CCR1 & D1 LOW when CNT>CCR1
  ExtClk = F_CPU/10;
  //Timer 1 CH1 D1 is used by the camera
  //init serial port
  Serial.begin(115200);
  //while(!Serial);
  Serial.println("Setup start");
  RPC.begin();
  RPC.bind("M4TaskCompleted",M4TaskCompleted);
  RPC.bind("decapDone",decapDone);
  RPC.bind("recapDone",recapDone);
  RPC.bind("initDone",initDone);
  RPC.bind("ZCurrentPos",ZCurrentPos);
  RPC.bind("currentMotorPositionRPC",currentMotorPositionRPC);
  
  //Init the camera
  if(cam.begin(RESOLUTION, IMAGE_MODE, 15)){
    Serial.println("Cam initialised");//initialise the camera
  }
  else{
    Serial.println("Cam failed to initialize");
  }
  cam.setStandby(true);                //Put it in standby mode
  
  //Init the Ethernet communications
  LEDB_ON;
  
  Serial.println("Ethernet Coms starting...");
  Ethernet.begin(mac,ip);  //Start the Ethernet coms
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
  // Start the server
  server.begin();           //"server" is the name of the object for comunication through ethernet
  Serial.print("Ethernet server connected. Server is at ");
  //Serial.println(Ethernet.localIP());         //Gives the local IP through serial com
  Serial.println(StringIP);
  
  LEDG_OFF;
  Serial.println("Setup done");
}

void loop() {
  LEDB_ON;
  delay(250);
  EthernetClient client = server.available();
  EthernetClient* client_pntr = &client;
  if(client){ //A client tries to connect 
    unsigned long time_start = millis();
    String currentLine = "";
    while(client.connected()) { //Loop while the client is connected
      int state = getStatus();
      if(client.available()){
        time_start = millis(); //reset time_start
        currentLine = ""; //reset currentLine
        //Read the first Line
        char c = client.read();
        while(!(c== '\n' || c == ' ')){
          currentLine += c;
          c = client.read();
        }
        
        if(currentLine.endsWith("home")){
          homePage(client_pntr);
        }
        else if(currentLine.endsWith("reset")){
          answerHttp(client_pntr,currentLine);
          resetFunc();
        }
        else if(currentLine.endsWith("decapperStatus")){
          statusHttp(client_pntr,currentLine);
        }
        else if(currentLine.endsWith("initialize")){
          if(state == 4){//decapper is currenlty busy
            answerHttpNo(client_pntr,currentLine,state);
          }
          else{
            answerHttp(client_pntr,currentLine);
            refAllHome();
          }
        }
        else if(currentLine.endsWith("decap")){
          if(state != 1){ //If in any other state than ready
            answerHttpNo(client_pntr,currentLine,state);
          }
          else{
            answerHttp(client_pntr,currentLine);
            Decap();
          }
        }
        else if(currentLine.endsWith("recap")){
          if(state != 2){
            answerHttpNo(client_pntr,currentLine,state);
          }
          else{
            answerHttp(client_pntr,currentLine);
            Recap();
          }
        }
        else if(currentLine.endsWith("relMove")){ //relative move
          answerHttp(client_pntr,currentLine);
          if(!relMove(currentLine))
            Serial.println("Error calling relMove");
        }

        else if(currentLine.endsWith("setVMAX")){
          answerHttp(client_pntr,currentLine);
          if(!setVMAX(currentLine))
            Serial.println("Error setting VMAX");
        }
        else if(currentLine.endsWith("setCurrentScaling")){
          answerHttp(client_pntr,currentLine);
          if(!setCurrentScaling(currentLine))
            Serial.println("Error setting Current Scaling");
        }
        else if(currentLine.endsWith("initControllers")){
          answerHttp(client_pntr,currentLine);
          initControllers();
        }
        else if(currentLine.endsWith("capture")){
          answerHttpCapture(client_pntr,currentLine);
          Serial.println("------------------------");
          finalPos();
          //detectEdges();
          //printCapture(); //print on Serial Monitor
          Serial.println("------------------------");
          //printCrop();
          Serial.println("-------------------------");
        }
      }//if(client.available())
      else if(millis()-time_start > TIMEOUT_ETH){
        Serial.println("Client timed out");
        client.stop();
      }
      
    }//if(client.connected())
    client.stop();
    
  }//if(client)
  
  LEDB_OFF;
  delay(250);
  
  checkEncoderStatus();
  //If the M4 processor is currently working, we read the RPC every 200 ms to check for uncomming messages
  //if(*M4work_pntr){
  String buffer = "";
  while (RPC.available()) {
    buffer += (char)RPC.read(); // Fill the buffer with characters
  }
  if (buffer.length() > 0) {
    Serial.print(buffer);
  }
}
