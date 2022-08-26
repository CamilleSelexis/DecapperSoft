void init_RPC(){
  RPC.begin();
  RPC.bind("Ref",Refgo);
  RPC.bind("Get_flask",Get_flaskgo);
  RPC.bind("Align",Aligngo);
  RPC.bind("Unscrew",Unscrewgo);
  RPC.bind("reScrew",reScrewgo);
  RPC.bind("Bringback",Bringbackgo);
  RPC.bind("untigh_up",untigh_upgo);
  RPC.bind("isMVT",isMVT);
  RPC.bind("GetDown",Getdowngo);
  RPC.bind("Backto1",backto1);
  RPC.bind("RefM",goRefM);
  RPC.bind("GoToStandby",goToStandby);
  RPC.bind("Decap",goDecap);
  RPC.bind("Recap",goRecap);
  RPC.bind("setParams",goSetParams);
  RPC.bind("MoveZ",goMoveZ);
  RPC.bind("MoveM",goMoveM);
  RPC.bind("MoveC",goMoveC);
  RPC.bind("readM4Params",readM4Params);
}

void decap(){
  
}

void recap(){
  
}

//Check that the drivers are operating correctly and that the encoder return correct values
bool init_drivers(TMC4361A controller){
  //Sequentially check the drivers
  for(int i = 1; i<5; i++){
    float init_angle = controller.getEncoderAngle();
    float init_turn = controller.getEncoderTurn();
    controller.setVMAX(200*256/(5-i),0);//About 1/5 turn per sec
    controller.setTargetRelative(50*256*i);//Turn for 1/4 turn
    while(!controller.isTargetReached()) //Wait for the motor to turn
    float angle_diff = abs(controller.getEncoderAngle()*controller.getEncoderTurn()-init_angle*init_turn);
    if(80< angle_diff || angle_diff > 100){
      RPC.println("More than 10° error");
      return false;
    }
  }
  RPC.println("Encoder Z is in range");
  return true;
}
