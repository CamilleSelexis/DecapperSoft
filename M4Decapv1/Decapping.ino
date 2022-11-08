//Contains functions setting target positions for the 3 motors
//Open the claws and lower the arm
bool ApproachFlask(){
  RPC.println("Going closer to the flask");
  ControllerZ.setTarget(Znear);
  ControllerM.setTarget(Mopen);

  if(!motor_running()){//wait for the driver to finish their movements
    RPC.println("Failed ApproachFlask");
    return false;
  }

  return true;
}

//Align the claws with the line on the cap
bool AlignCap(int capPos){
  RPC.println("Aligning the claws with the cap");
  ControllerC.setTargetRelative(capPos);
  if(!motor_running){
    RPC.println("Failed AlignCap");
    return false;
  }
  return true;
}

//perform the unscrewing movement -> lower the arm, close the claws, turn CCW while going up
bool UnscrewCap(){
  RPC.println("Press the cap");
  ControllerZ.setTarget(capHeight);
  if(!motor_running){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  RPC.println("Hold the cap");
  ControllerM.setTarget(capHold);
  if(!motor_running){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  RPC.println("Unscrewing of the cap");
  setScrewingSpeed();
  ControllerZ.setTarget(capDecapZ);
  ControllerC.setTarget(capDecapC);
  if(!motor_running){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  setDefaultSpeed();
  
  return true;
}

//Go to the standby position
bool goToStandby() {
  ControllerZ.setTarget(standbyZ);
  ControllerC.setTarget(standbyC);
  ControllerM.setTarget(standbyM);

  if(!motor_running()){
    RPC.println("Failed goToStandby");
    return false;
  }

  return true;
}

//Perform screwing movement, lower the arm, turn CW while going down, open the claws
bool ScrewCap(){
  RPC.println("Press the cap");
  ControllerZ.setTarget(screwStartZ);
  if(!motor_running){
    RPC.println("Failed UnscrewCap");
    return false;
  }

  RPC.println("Screwing of the cap");
  setScrewingSpeed();
  ControllerZ.setTarget(capDecapZ);
  ControllerC.setTarget(capDecapC);
  if(!motor_running){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  RPC.println("Release the cap");
  ControllerM.setTarget(capRelease);
  if(!motor_running){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  setDefaultSpeed();
  return true;
}
bool decap(){
  pin_init(); //pin_init after the camera was used to be able to use pin D13 ?
  if(!ApproachFlask()) return false;
  if(!AlignCap(*PCpos)) return false;
  if(!UnscrewCap()) return false;
  if(!goToStandby()) return false;

  return true;
}

bool recap(){
  if(!ApproachFlask()) return false;
  if(!AlignCap(*PCpos)) return false;
  if(!ScrewCap()) return false;
  if(!goToStandby()) return false;
  
  return true;
}

//Check that the drivers are operating correctly and that the encoder return correct values
bool init_driver(TMC4361A controller){
  //Sequentially check the drivers
  for(int i = 1; i<5; i++){
    float init_angle = controller.getEncoderAngle();
    float init_turn = controller.getEncoderTurn();
    controller.setVMAX(200*256/(5-i),0);//About 1/5 turn per sec
    controller.setTargetRelative(50*256*i*(pow(-1,i)));//Turn for 1/4 turn
    while(!controller.isTargetReached()); //Wait for the motor to turn
    float angle_diff = abs(controller.getEncoderAngle()*controller.getEncoderTurn()-init_angle*init_turn);
    if((90*i)-10< angle_diff || angle_diff > (90*i)-10){
      RPC.println("More than 10° error");
      return false;
    }
  }
  RPC.println("Encoder Z is in range");
  return true;
}
