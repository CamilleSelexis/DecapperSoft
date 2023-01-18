//Contains functions setting target positions for the 3 motors

//this function is called when a motor is running and returns true when each motor is done, return false if the movement timeouts (30 sec)
bool motor_running(){
  digitalWrite(LEDR,LOW);
  long time_start = millis();
  long time_update = time_start;
  //updateValues();
  //if(ZPos != ZTarget)    RPC.println("Z is running");
  //if(MPos != MTarget)    RPC.println("M is running");
  //if(CPos != CTarget)    RPC.println("C is running");
  while(!(ControllerZ.isTargetReached() && ControllerM.isTargetReached() && ControllerC.isTargetReached())){
      
    if(millis()-time_start > TIMEOUT_MVMT){
      RPC.println("Something is wrong and I can feel it");
      ControllerZ.setTargetRelative(0);
      ControllerM.setTargetRelative(0);
      ControllerC.setTargetRelative(0);
      digitalWrite(LEDR,HIGH);
      return false;
    }
    if(millis()-time_update > TIME_UPDATE){

      updateValues();
      time_update = millis();
    }
  }
  digitalWrite(LEDR,HIGH);
  //RPC.print("Move took ");RPC.print(millis()-time_start);RPC.println(" ms");
  CLEAR_RUNNING;
  return true;
}
//Open the claws and lower the arm
bool ApproachFlask(){
  //RPC.println("Going closer to the flask");
  ControllerZ.setTarget(Znear);
  ControllerM.setTarget(Mopen);
  
  if(!motor_running()){//wait for the driver to finish their movements
    RPC.println("Failed ApproachFlask");
    return false;
  }

  return true;
}

//Align the claws with the bumps on the cap & go closer to the cap
bool AlignCap(long capPos){
  //RPC.print("Aligning claws & going closer");RPC.println(capPos);
  ControllerZ.setTarget(capHeight);
  ControllerM.setTarget(Mopen);
  ControllerC.setTarget(capPos);
  if(!motor_running()){
    RPC.println("Failed AlignCap");
    return false;
  }
  return true;
}

//perform the unscrewing movement -> lower the arm, close the claws, turn CCW while going up
bool UnscrewCap(){
  //RPC.println("Press the cap");
  /*ControllerZ.setTarget(capHeight);
  if(!motor_running()){
    RPC.println("Failed UnscrewCap");
    return false;
  }*/
  //RPC.println("Hold the cap");
  ControllerM.setTarget(capHold);
  if(!motor_running()){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  //RPC.println("Unscrewing of the cap");
  setScrewingSpeed();
  ControllerZ.setTarget(ZScrewingPos);
  ControllerC.setTargetRelative(CUnscrew);
  if(!motor_running()){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  setDefaultSpeed();
  
  return true;
}

//Go to the standby position
bool goToStandby() {
  ControllerZ.setTarget(standbyZ);
  ControllerM.setTarget(standbyM);
  //ControllerC.setTarget(standbyC);

  if(!motor_running()){
    RPC.println("Failed goToStandby");
    return false;
  }

  return true;
}

bool goToInitPos(){
  ControllerM.setTarget(standbyM);
  if(!motor_running()){
    RPC.println("Failed goToStandby");
    return false;}
  ControllerZ.setTarget(standbyZ);
  ControllerC.setTarget(standbyC);
  if(!motor_running()){
    RPC.println("Failed goToStandby");
    return false;}
  return true;
}
bool goToStandbyWithCap() {
  ControllerZ.setTarget(standbyZ);
    if(!motor_running()){
    RPC.println("Failed goToStandby");
    return false;
  }

  return true;
}
//Perform screwing movement, lower the arm, turn CW while going down, open the claws
bool ScrewCap(){
  //RPC.println("Going to screw height");
  ControllerZ.setTarget(ZScrewingPos);
  if(!motor_running()){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  //RPC.println("Screwing of the cap");
  setScrewingSpeed();
  ControllerZ.setTarget(capHeight);
  ControllerC.setTargetRelative(CScrew);
  if(!motor_running()){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  setDefaultSpeed();
  //RPC.println("Release the cap");
  ControllerM.setTarget(capRelease);
  if(!motor_running()){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  /*
  ControllerZ.setTarget(Znear);
  if(!motor_running()){
    RPC.println("Failed UnscrewCap");
    return false;
  }*/
  return true;
}
bool decap(){
  //pin_init(); //pin_init after the camera was used to be able to use pin D13 ?
  setDefaultSpeed();
  //if(!ApproachFlask()) return false;
  if(!AlignCap(*pCPos)) return false;
  if(!UnscrewCap()) return false;
  if(!goToStandbyWithCap()) return false;
  
  return true;
}

bool recap(){
  setDefaultSpeed();
  if(!ScrewCap()) return false;
  if(!goToStandby()) return false;
  
  return true;
}
bool init_driver(TMC4361A *pController) {
  long initPos = pController->getEncoderPos();
  long targetPos = 0;
  long tolerance = 256; //1FS tolerance
  delay(50);
  for(int i = 1; i<=3; i++){
    switch(i){
      case 1 : pController->setTargetRelative(25*256); //+90 °
       targetPos = initPos + 25*256;
       break;
      case 2 : pController->setTargetRelative(50*256*(-1)); // -90°
        targetPos = initPos - 25*256;
        break;
      case 3 : pController->setTargetRelative(25*256); // 0°
        targetPos = initPos;
      break;
    }
    while(!pController->isTargetReached()); //Wait for the motor to turn
    //motor_running();
    long newPos = pController->getEncoderPos(); //Should be equal to 50*256
    RPC.print("Position difference = ");RPC.println(abs(newPos - targetPos));
    if(abs(newPos-targetPos) > tolerance){
      RPC.print("Encoder outisde of tolerance : " + String(newPos-targetPos));
      return false;
    }
  }
  RPC.println("Encoder is in tolerance");
  return true;
}

bool ZrelMove(long value){
  RPC.print("Z will move by ");RPC.println(value);
  ControllerZ.setTargetRelative(value);
  if(!motor_running()){
    RPC.println("Failed the relative move on Z");
    return false;
  }
  return true;
}
bool MrelMove(long value){
  RPC.print("M will move by ");RPC.println(value);
  ControllerM.setTargetRelative(value);
  if(!motor_running()){
    RPC.println("Failed the relative move on M");
    return false;
  }
  return true;
}
bool CrelMove(long value){
  RPC.print("C will move by ");RPC.println(value);
  ControllerC.setTargetRelative(value);
  if(!motor_running()){
    RPC.println("Failed the relative move on C");
    return false;
  }
  return true;
}
void test_motors(){
  if(ControllerZ.isTargetReached()){
      RPC.println("Z target reached so setting new target");
      ControllerZ.setTargetRelative(25600);}
  else{
        RPC.print("Z current position :");RPC.println(ControllerZ.getCurrentPos());
        RPC.print("Z current target :");RPC.println(ControllerZ.getCurrentTarget());
      }
  if(ControllerM.isTargetReached()){
      RPC.println("M target reached so setting new target");
      ControllerM.setTargetRelative(51200);}
  else{
      RPC.print("M current position :");RPC.println(ControllerM.getCurrentPos());
      RPC.print("M current target :");RPC.println(ControllerM.getCurrentTarget());
      }
    if(ControllerC.isTargetReached()){
      RPC.println("C target reached so setting new target");
      ControllerC.setTargetRelative(256000);}
    else{
      RPC.print("C current position :");RPC.println(ControllerC.getCurrentPos());
      RPC.print("C current target :");RPC.println(ControllerC.getCurrentTarget());
      }
}

bool encoderCheck(TMC4361A controller, float initAngle, float initTurn,long initPos){
  float angle = controller.getEncoderAngle();
  float turn = controller.getEncoderTurn();

  long motorPos = controller.getCurrentPos();
  long motorTarget = controller.getCurrentTarget();

  if((abs(angle-initAngle)+abs(turn-initTurn)*360)*200*256>(abs(motorPos-initPos)+512)) //3.2° tolerance
    return false;
  return true;
}
