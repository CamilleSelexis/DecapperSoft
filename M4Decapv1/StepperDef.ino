//Set the motor speed
void motor_low_speed(){
  //Set the speed to 1 rot/s
  ControllerZ.setVMAX(1,1);
  ControllerM.setVMAX(1,1);
  ControllerC.setVMAX(1,1);

  ControllerZ.setAMAX(AMAX_SLOW);
  ControllerM.setAMAX(AMAX_SLOW);
  ControllerC.setAMAX(AMAX_SLOW);
}
//Set the motor speed
void motor_high_speed(){
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

  unsigned long Zmove = ceil(ZGEAR*STEP_TURN*USTEPS*4.1); //in steps
  unsigned long Cmove = ceil(CGEAR*CTRANS*STEP_TURN*USTEPS*0.8); //in steps
  unsigned long screwSpeedZ = ceil(Zmove/(SCREW_TIME/USTEPS)); //take 10 sec to unscrew the cap
  unsigned long screwSpeedC = ceil(Cmove/(SCREW_TIME/USTEPS));

  
  ControllerZ.setVMAX(screwSpeedZ,0);
  ControllerC.setVMAX(screwSpeedC,0);
  
  ControllerZ.setAMAX(AMAX_DEFAULT);
  ControllerC.setAMAX(AMAX_DEFAULT);
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
