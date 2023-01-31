void init_RPC(){
  RPC.begin();
  RPC.bind("Decap",launchDecap);
  RPC.bind("Recap",launchRecap);
  RPC.bind("Initialize",launchInitialize);
  RPC.bind("ZrelMove",launchZrelMove);
  RPC.bind("MrelMove",launchMrelMove);
  RPC.bind("CrelMove",launchCrelMove);
  RPC.bind("ZsetVMAX",launchZsetVMAX);
  RPC.bind("MsetVMAX",launchMsetVMAX);
  RPC.bind("CsetVMAX",launchCsetVMAX);
  RPC.bind("ZsetCurrentScaling",launchZsetCurrentScaling);
  RPC.bind("MsetCurrentScaling",launchMsetCurrentScaling);
  RPC.bind("CsetCurrentScaling",launchCsetCurrentScaling);
  RPC.bind("stopAllMoves",launchstopAllMoves);
  RPC.bind("initControllers",launchinitControllers);
  RPC.bind("resumeMoves",launchResume);
  RPC.bind("abortMoves",launchAbort);
}

void getCurrentPosition(){
  float turn = ControllerZ.getEncoderTurn();
  float angle = ControllerZ.getEncoderAngle();
  long actualZ = floor((turn*360+angle)*USTEPS/1.8); //1.8 deg by full steps

  ControllerZ.setCurrentPos(actualZ);

  turn = ControllerM.getEncoderTurn();
  angle = ControllerM.getEncoderAngle();
  long actualM = floor((turn*360+angle)*USTEPS/1.8);

  ControllerM.setCurrentPos(actualM);
  
  turn = ControllerC.getEncoderTurn();
  angle = ControllerC.getEncoderAngle();
  long actualC = floor((turn*360+angle)*USTEPS/1.8);

  ControllerC.setCurrentPos(actualC);
}

void updateValues(){
  ZPos = ControllerZ.getCurrentPos();
  ZTarget = ControllerZ.getCurrentTarget();
  ZPosEnc = ControllerZ.getEncoderPos();
  ZAngleEnc = ControllerZ.getEncoderAngle();
  ZDevEnc = ControllerZ.getEncoderDev();
  //ZTurnEnc = ControllerZ.getEncoderTurn();
  delay(20);
  MPos = ControllerM.getCurrentPos();
  MTarget = ControllerM.getCurrentTarget();
  MPosEnc = ControllerM.getEncoderPos();
  MAngleEnc = ControllerM.getEncoderAngle();
  MDevEnc = ControllerM.getEncoderDev();
  //MTurnEnc = ControllerM.getEncoderTurn();
  delay(20);
  CPos = ControllerC.getCurrentPos();
  CTarget = ControllerC.getCurrentTarget();
  CPosEnc = ControllerC.getEncoderPos();
  CAngleEnc = ControllerC.getEncoderAngle();
  CDevEnc = ControllerC.getEncoderDev();
  //CTurnEnc = ControllerC.getEncoderTurn();
  delay(20);
  //if(!RPC.call("currentMotorPositionRPC",ZPos,MPos,CPos,ZTarget,MTarget,CTarget).as<bool>())
  if(!RPC.call("currentMotorPositionRPC",ZPos,MPos,CPos,ZTarget,MTarget,CTarget,
                                        ZPosEnc,MPosEnc,CPosEnc,ZAngleEnc,MAngleEnc,CAngleEnc,
                                        ZTurnEnc,MTurnEnc,CTurnEnc,
                                        ZDevEnc,MDevEnc,CDevEnc).as<bool>())
    RPC.println("Error sending motor position");
}

void initControllers(){
  pin_init();
  ControllerZ.begin();
  ControllerM.begin();
  ControllerC.begin();
}

bool abortMoves() {
  checkpoints = 0;
  recapFlag = false;
  decapFlag = false;
  stopRoutine = false;
  *Pstate = 3;
}
