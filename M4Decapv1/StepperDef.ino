//Set the motor speed
void setLowSpeed(){
  //Set the speed to 1 rot/s
  ControllerZ.setVMAX(VMAX_SLOW,0);
  ControllerM.setVMAX(VMAX_SLOW,0);
  ControllerC.setVMAX(VMAX_SLOW,0);

  ControllerZ.setAMAX(AMAX_SLOW);
  ControllerM.setAMAX(AMAX_SLOW);
  ControllerC.setAMAX(AMAX_SLOW);
}
//Set the motor speed
void setHighSpeed(){
  //Set the speed to 5 rot/s
  ControllerZ.setVMAX(5,1);
  ControllerM.setVMAX(5,1);
  ControllerC.setVMAX(5,1);

  ControllerZ.setAMAX(AMAX_DEFAULT);
  ControllerM.setAMAX(AMAX_DEFAULT);
  ControllerC.setAMAX(AMAX_DEFAULT);
}

//Speed used for screwing/unscrewing of the cap
void setScrewingSpeed() {

  float Zdist = 8; //Z moves up 8 mm to unscrew
  float CTurn = 0.7; //C makes 0.8 turn
  unsigned long Zmove = ceil(Zdist*ZGEAR*ZTRANS*STEP_TURN*USTEPS/ZSCREWSTEP); //in Usteps 1500000
  unsigned long Cmove = ceil(CGEAR*CTRANS*STEP_TURN*USTEPS*CTurn); //in Usteps 2611200
  unsigned long screwSpeedZ = ceil(Zmove/(SCREW_TIME)); //take 6 sec to unscrew the cap
  unsigned long screwSpeedC = ceil(Cmove/(SCREW_TIME));
  
  RPC.print("Screwing speed set to : ");RPC.println(screwSpeedZ);
  RPC.print("Screwing speed set to : ");RPC.println(screwSpeedC);
  ControllerZ.setVMAX(screwSpeedZ<<8,0); //first 8 bits are for decimal
  delay(20);
  ControllerC.setVMAX(screwSpeedC<<8,0);
  delay(20);
}
//Set motors speed to default
void setDefaultSpeed() {
  ControllerZ.setVMAX(VMAX_DEFAULT,0);
  ControllerC.setVMAX(VMAX_DEFAULT,0);
  ControllerM.setVMAX(VMAX_DEFAULT,0);
  
  ControllerZ.setAMAX(AMAX_DEFAULT);
  ControllerC.setAMAX(AMAX_DEFAULT);
  ControllerM.setAMAX(AMAX_DEFAULT);
}
