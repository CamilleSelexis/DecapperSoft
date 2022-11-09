void init_RPC(){
  RPC.begin();
  RPC.bind("Decap",launchDecap);
  RPC.bind("Recap",launchRecap);
  RPC.bind("InitDrivers",launchInitDrivers);
  RPC.bind("MoveZ",launchMoveZ);
  RPC.bind("MoveM",launchMoveM);
  RPC.bind("MoveC",launchMoveC);
  RPC.bind("ZPos",launchZPos);
  RPC.bind("MPos",launchMPos);
  RPC.bind("CPos",launchCPos);
  RPC.bind("ZTarget",launchZTarget);
  RPC.bind("MTarget",launchMTarget);
  RPC.bind("CTarget",launchCTarget);
  RPC.bind("ZPosEnc",launchZPosEnc);
  RPC.bind("MPosEnc",launchMPosEnc);
  RPC.bind("CPosEnc",launchCPosEnc);
  /*RPC.bind("ZCurrentPos",ZCurrentPos);
  RPC.bind("MCurrentPos",MCurrentPos);
  RPC.bind("CCurrentPos",CCurrentPos);
  RPC.bind("ApproachFlask",ApproachFlask);
  RPC.bind("Get_flask",Get_flaskgo);
  RPC.bind("Align",Aligngo);
  RPC.bind("Unscrew",Unscrewgo);
  RPC.bind("reScrew",reScrewgo);
  RPC.bind("Bringback",Bringbackgo);
  RPC.bind("untigh_up",untigh_upgo);
  RPC.bind("isMVT",isMVT);
  RPC.bind("GetDown",Getdowngo);
  RPC.bind("GoToStandby",GoToStandby);
  RPC.bind("setParams",SetParams);
  RPC.bind("readM4Params",readM4Params);*/
}

void getCurrentPosition(){
  float turn = ControllerZ.getEncoderTurn();
  float angle = ControllerZ.getEncoderAngle();
  long actualZ = floor((turn*360+angle)*USTEPS/1.8); //1.8 deg by full steps

  ControllerZ.setCurrentPos(actualZ);

  turn = ControllerC.getEncoderTurn();
  angle = ControllerC.getEncoderAngle();
  long actualC = floor((turn*360+angle)*USTEPS/1.8);

  ControllerC.setCurrentPos(actualC);

  turn = ControllerM.getEncoderTurn();
  angle = ControllerM.getEncoderAngle();
  long actualM = floor((turn*360+angle)*USTEPS/1.8);

  ControllerM.setCurrentPos(actualM);
}

void updateValues(){
  ZPos = ControllerZ.getCurrentPos();
  ZTarget = ControllerZ.getCurrentTarget();
  ZPosEnc = ControllerZ.getEncoderPos();
  delay(20);
  MPos = ControllerM.getCurrentPos();
  MTarget = ControllerM.getCurrentTarget();
  MPosEnc = ControllerM.getEncoderPos();
  //RPC.print("M encoder position : ");RPC.print(MPosEnc); RPC.print("M angle : ");RPC.print(ControllerM.getEncoderAngle());RPC.print(" M turn : ");RPC.println(ControllerM.getEncoderTurn());
  delay(20);
  CPos = ControllerC.getCurrentPos();
  CTarget = ControllerC.getCurrentTarget();
  CPosEnc = ControllerC.getEncoderPos();
  delay(20);
  //if(!RPC.call("currentMotorPositionRPC",ZPos,MPos,CPos,ZTarget,MTarget,CTarget).as<bool>())
  if(!RPC.call("currentMotorPositionRPC",ZPos,MPos,CPos,ZTarget,MTarget,CTarget,ZPosEnc,MPosEnc,CPosEnc).as<bool>())
    RPC.println("Error sending motor position");
}
