//Contains functions setting target positions for the 3 motors

//this function is called when a motor is running and returns true when each motor is done, return false if the movement timeouts (30 sec)
bool motor_running(long timeout){
  LEDR_ON;
  long time_start = millis();
  long time_update = time_start;
  //updateValues();
  //if(ZPos != ZTarget)    RPC.println("Z is running");
  //if(MPos != MTarget)    RPC.println("M is running");
  //if(CPos != CTarget)    RPC.println("C is running");
  while(!(ControllerZ.isTargetReached() && ControllerM.isTargetReached() && ControllerC.isTargetReached())){
    if(millis()-time_update > timeout){

      updateValues();
      time_update = millis();
    }
    delay(50); //Read every 50 ms
    //Check timeout condition
    if(millis()-time_start > timeout){
      RPC.println("TimeOut during movement");
      ControllerZ.setTargetRelative(0);
      ControllerM.setTargetRelative(0);
      ControllerC.setTargetRelative(0);

      //Check on which axis the error happened
      if(ControllerZ.isEncoderFail()){
        //RPC.call("ZEncoderFail").as<bool>();
        RPC.println("Encoder Z fail");
        LEDR_OFF;
        CLEAR_RUNNING;
        return false;
      }
      if(ControllerM.isEncoderFail()){ //Encoder fail on M -> grip on bumps
        //RPC.call("MEncoderFail").as<bool>();
        RPC.println("Encoder M fail");
        if(MTarget == capHold && !stopRoutine) { //Encoder fail happened when trying to grip
          RPC.println("Realign routine start");
          if( realignCap(timeout)){
            LEDR_OFF;
            CLEAR_RUNNING;
            return true;
          }
          return false;
        }
        else {
          LEDR_OFF;
          CLEAR_RUNNING;
          return false;
        }

      }
      if(ControllerC.isEncoderFail()){
        //RPC.call("CEncoderFail").as<bool>();
        LEDR_OFF;
        CLEAR_RUNNING;
        return false;
      }
    LEDR_OFF;
    CLEAR_RUNNING;
    return false;
    }
  }
  LEDR_OFF;
  CLEAR_RUNNING;
  return true;
}
bool realignCap(long timeout){
  long time_start = millis();
  
  setLowSpeed();
  //Déserre M -> 2500000
  ControllerM.setTarget(capNear);
  RPC.println("Current M target = ");RPC.println(ControllerM.getCurrentTarget());
  delay(50);
  while(!ControllerM.isTargetReached()){
    //Motor is running
    delay(50);
    if(millis()-time_start > timeout){
      RPC.println("Timeout in realign");
      return false; //Realign failed
    }
  }
  updateValues();
  RPC.print("In the for loop");
  for(int i=1;i<4;i++){ //Essaie de réaligner en 4 fois
    time_start = millis();
    ControllerC.setTargetRelative(ceil(uSToTurnC/60)); //CW pour pas que le bouchon soit entraîné
    ControllerM.setTarget(capHold);
    while(!(ControllerM.isTargetReached() && ControllerC.isTargetReached()) && abs(ControllerM.getEncoderDev())<20000){ //Doit vérifier cette valeure
      //Motor is running
      delay(50);
      if(millis()-time_start > timeout) return false; //Realign failed
    }
    RPC.println(ControllerM.getEncoderDev());
    //Si sortit du while: soit encoderDev>10000 -> doit réouvrir et retourner le C
    //                    soit TargetReached -> realignement réussit
    time_start = millis();
    if(abs(ControllerM.getEncoderDev())>20000){
      RPC.println("Retry realign");
      ControllerM.setTarget(capNear);
      while(!ControllerM.isTargetReached()){
        //Motor is running
        delay(50);
        if(millis()-time_start > timeout) return false; //Realign failed
      }
    }
    else{
      setDefaultSpeed();
      return true; //Realign success
    }
    updateValues();
  }
  setDefaultSpeed();
  return false; //Realign failed
}
//----------------------------------------------------------------------------------
//Decapping routine -----------------------------------------------------------
bool decap(int resumeVal){
  decapFlag = true;
  stopRoutine = false;
  DRIVER_ON;
  //pin_init(); //pin_init after the camera was used to be able to use pin D13 ?
  setDefaultSpeed();
  //if(!ApproachFlask()) return false;
  if(resumeVal <= 1)
    if(!AlignCap(*pCAlignValue) || stopRoutine) return false;
  if(resumeVal <= 2)
    if(!CloseClaws() || stopRoutine) return false;
  if(resumeVal <= 3)
    if(!Unscrew() || stopRoutine) return false;
  //if(!UnscrewCap() || stopRoutine) return false;
  if(resumeVal <= 4)
    if(!goToStandbyWithCap() || stopRoutine) return false;
  checkpoints = 10; //Process is finished
  DRIVER_OFF;
  
  stopRoutine = false;
  decapFlag = false;
  return true;
}

//Align the claws with the bumps on the cap & go closer to the cap
bool AlignCap(long capPos){
  //RPC.print("Aligning claws & going closer");RPC.println(capPos);
  checkpoints = 1;
  ControllerZ.setTarget(capHeight);
  ControllerM.setTarget(Mopen);
  ControllerC.setTarget(capPos);
  if(!motor_running(15000)){
    RPC.println("Failed AlignCap");
    return false;
  }
  return true;
}
bool CloseClaws(){
  checkpoints = 2;
  ControllerM.setTarget(capHold);
  if(!motor_running(10000)){ //Reduced timeout for closing, it should then do the realign procedure faster
    RPC.println("Failed UnscrewCap");
    return false;
  }
  return true;
}
bool Unscrew(){
  checkpoints = 3;
  //RPC.println("Unscrewing of the cap");
  setScrewingSpeed();
  ControllerZ.setTarget(ZScrewingPos);
  ControllerC.setTargetRelative(CUnscrew);
  if(!motor_running(15000)){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  setDefaultSpeed();
  
  return true;
}
bool goToStandbyWithCap() {
  checkpoints = 4;
  ControllerZ.setTarget(standbyZAfterDecap);
    if(!motor_running(15000)){
    RPC.println("Failed goToStandby");
    return false;
  }

  return true;
}
//-------------------------------------------------------------------------------------------
//Recapping Routine -------------------------------------------------------------------------
bool recap(int resumeVal){
  recapFlag = true;
  DRIVER_ON;
  stopRoutine = false;
  setDefaultSpeed();
  checkpoints = 1;
  //if(!ScrewCap() || stopRoutine) return false;
  if(resumeVal <= 1)
    if(!goToRecap() || stopRoutine) return false;
  if(resumeVal <= 2)
    if(!Screw() || stopRoutine) return false;
  if(resumeVal <= 3)
    if(!OpenClaws() || stopRoutine) return false;
  if(resumeVal <= 4)
    if(!goToStandby() || stopRoutine) return false;
  checkpoints = 10; //Process is finished
  
  DRIVER_OFF;
  stopRoutine = false;
  recapFlag = false;
  
  return true;
}

bool goToRecap(){
  checkpoints = 1;
  ControllerZ.setTarget(ZScrewingPos);
  if(!motor_running(15000)){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  return true;
}

bool Screw(){
  checkpoints = 2;
  //RPC.println("Screwing of the cap");
  setScrewingSpeed();
  ControllerZ.setTarget(capHeight);
  ControllerC.setTargetRelative(CScrew);
  if(!motor_running(15000)){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  return true;
}
bool OpenClaws(){
  checkpoints = 3;
  setDefaultSpeed();
  //RPC.println("Release the cap");
  ControllerM.setTarget(capRelease);
  if(!motor_running(15000)){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  return true;
}
//Go to the standby position
bool goToStandby() {
  checkpoints = 4;
  ControllerZ.setTarget(standbyZ);
  ControllerM.setTarget(standbyM);
  //ControllerC.setTarget(standbyC);
  if(!motor_running(20000)){
    RPC.println("Failed goToStandby");
    return false;
  }
  return true;
}
//-----------------------------------------------------------------
//Initialize Driver & Controller Routine---------------------------
bool goToInitPos(){
  setDefaultSpeed();
  setLowAccel();
  ControllerM.setTarget(standbyM);
  if(!motor_running(20000)){
    RPC.println("Failed goToStandby");
    return false;}
  ControllerZ.setTarget(standbyZ);
  ControllerC.setTarget(standbyC);
  if(!motor_running(20000)){
    RPC.println("Failed goToStandby");
    return false;}
  return true;
}

bool init_driver(TMC4361A *pController) {
  long initPos = pController->getEncoderPos();
  long time_start = millis();
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
    while(!pController->isTargetReached()){
    if(millis()-time_start > TIMEOUT_MVMT){ return false;} //Wait for the motor to turn
    }
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
//--------------------------------------------------------------------------------------
//Realtive Moves------------------------------------------------------------------------
bool ZrelMove(long value){
  RPC.print("Z will move by ");RPC.println(value);
  ControllerZ.setTargetRelative(value);
  if(!motor_running(60000)){
    RPC.println("Failed the relative move on Z");
    return false;
  }
  return true;
}
bool MrelMove(long value){
  RPC.print("M will move by ");RPC.println(value);
  ControllerM.setTargetRelative(value);
  if(!motor_running(60000)){
    RPC.println("Failed the relative move on M");
    return false;
  }
  return true;
}
bool CrelMove(long value){
  RPC.print("C will move by ");RPC.println(value);
  ControllerC.setTargetRelative(value);
  if(!motor_running(60000)){
    RPC.println("Failed the relative move on C");
    return false;
  }
  return true;
}
//-------------------------------------------------------------------------------
bool resumeMoves(){
  //Movement was interrupted during decap
  if(decapFlag){
    switch(checkpoints){
      case 1: //Interrupt during approach phase
        ControllerZ.setCurrentPos(ZPos-ZDevEnc); //Current true position
        //ControllerZ.setTarget(capHeight);
        ControllerM.setCurrentPos(MPos-MDevEnc); //Current true position
        //ControllerM.setTarget(Mopen);
        ControllerC.setCurrentPos(CPos-CDevEnc); //Current true position
        //ControllerC.setTarget(*pCAlignValue);
        CmdResult = decap(1); //Do everything
        RPC.call("decapDone",CmdResult).as<bool>();
        return true;
      break;
      case 2: //Interrupt during tightening phase
        ControllerM.setCurrentPos(MPos-MDevEnc);
        //ControllerM.setTarget(capHold);
        CmdResult = decap(2); //Do everything
        RPC.call("decapDone",CmdResult).as<bool>();
        return true;
      break;
      case 3: //Interrupt during unscrewing phase
        ControllerZ.setCurrentPos(ZPos-ZDevEnc);
        //ControllerZ.setTarget(ZScrewingPos);
        ControllerC.setCurrentPos(CPos-CDevEnc);
        ControllerC.setTarget(CPos);
        CmdResult = decap(3); //Do everything
        RPC.call("decapDone",CmdResult).as<bool>();
        return true;
      break;
      case 4: //Interrupt during the up phase
        ControllerZ.setCurrentPos(ZPos-ZDevEnc);
        CmdResult = decap(4); //Do everything
        RPC.call("decapDone",CmdResult).as<bool>();
        return true;
      break;
      case 10: //Decap was done
        CmdResult = decap(5); //Do everything
        RPC.call("decapDone",CmdResult).as<bool>(); //Launch decap without any moves only to properly set all variables
        return true;
      break;
    }
  }
  //Movement was interrupted during recap
  else if(recapFlag){
    switch(checkpoints){
      case 1: //Interrupt during approach phase
        ControllerZ.setCurrentPos(ZPos-ZDevEnc); //Current true position
        ControllerZ.setTarget(ZScrewingPos);
        CmdResult = recap(1); //Do everything
        RPC.call("recapDone",CmdResult).as<bool>();
        return true;
      break;
      case 2: //Interrupt during screwing phase
        ControllerZ.setCurrentPos(ZPos-ZDevEnc);
        //ControllerZ.setTarget(capHeight);
        ControllerC.setCurrentPos(CPos-CDevEnc);
        ControllerC.setTarget(CPos);
        CmdResult = recap(2); //Do everything
        RPC.call("recapDone",CmdResult).as<bool>();
        return true;
      break;
      case 3: //Interrupt during opening of the claws
        ControllerM.setCurrentPos(MPos-MDevEnc);
        CmdResult = recap(3); //Do everything
        RPC.call("recapDone",CmdResult).as<bool>();
        return true;
      break;
      case 4: //Interrupt during the up phase
        ControllerZ.setCurrentPos(ZPos-ZDevEnc);
        CmdResult = recap(4); //Do everything
        RPC.call("recapDone",CmdResult).as<bool>();
        return true;
      break;
      case 10: //Recap was done
        CmdResult = recap(5); //Do everything
        RPC.call("recapDone",CmdResult).as<bool>(); //Launch recap without any moves only to properly set all variables
        return true;
      break;
    }    
  }
  return true;
}

//Obsolete functions------------------------------------------------------------------------------
//Open the claws and lower the arm
bool ApproachFlask(){
  //RPC.println("Going closer to the flask");
  ControllerZ.setTarget(Znear);
  ControllerM.setTarget(Mopen);
  
  if(!motor_running(15000)){//wait for the driver to finish their movements
    RPC.println("Failed ApproachFlask");
    return false;
  }

  return true;
}
//perform the unscrewing movement -> lower the arm, close the claws, turn CCW while going up
bool UnscrewCap(){
  //RPC.println("Press the cap");
  //RPC.println("Hold the cap");
  checkpoints = 2;
  ControllerM.setTarget(capHold);
  if(!motor_running(15000)){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  checkpoints = 3;
  //RPC.println("Unscrewing of the cap");
  setScrewingSpeed();
  ControllerZ.setTarget(ZScrewingPos);
  ControllerC.setTargetRelative(CUnscrew);
  if(!motor_running(15000)){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  setDefaultSpeed();
  
  return true;
}

//Perform screwing movement, lower the arm, turn CW while going down, open the claws
bool ScrewCap(){
  //RPC.println("Going to screw height");
  checkpoints = 1;
  ControllerZ.setTarget(ZScrewingPos);
  if(!motor_running(15000)){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  checkpoints = 2;
  //RPC.println("Screwing of the cap");
  setScrewingSpeed();
  ControllerZ.setTarget(capHeight);
  ControllerC.setTargetRelative(CScrew);
  if(!motor_running(15000)){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  checkpoints = 3;
  setDefaultSpeed();
  //RPC.println("Release the cap");
  ControllerM.setTarget(capRelease);
  if(!motor_running(15000)){
    RPC.println("Failed UnscrewCap");
    return false;
  }
  checkpoints = 4;
  /*
  ControllerZ.setTarget(Znear);
  if(!motor_running()){
    RPC.println("Failed UnscrewCap");
    return false;
  }*/
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
