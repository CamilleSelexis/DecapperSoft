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


  unsigned long screwSpeedZ = ceil(ZUnscrew/(SCREW_TIME)); //take 6 sec to unscrew the cap -> 912000 steps
  unsigned long screwSpeedC = ceil(CUnscrew/(SCREW_TIME));
  
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
void setLowAccel(){
  ControllerZ.setAMAX(AMAX_SLOW);
  ControllerM.setAMAX(AMAX_SLOW);
  ControllerC.setAMAX(AMAX_SLOW);
}
