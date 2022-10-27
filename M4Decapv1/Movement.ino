//this function is called when a motor is running and returns true when each motor is done, return false if the movement timeouts (30 sec)
bool motor_running(){
  digitalWrite(LEDR,LOW);
  long time_start = millis();
  long time_update = time_start;
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
  RPC.print("Move took ");RPC.print(millis()-time_start);RPC.println(" ms");
  return true;
}

void moveZ(){
  ControllerZ.setTargetRelative(51200);
  if(!motor_running())
    RPC.println("Failed the move");
}

void moveM(){
  ControllerM.setTargetRelative(51200);
  if(!motor_running())
    RPC.println("Failed the move");
}

void moveC(){
  ControllerC.setTargetRelative(51200);
  if(!motor_running())
    RPC.println("Failed the move");
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
