void goToStandby(){
  //Go to encoder turn = 0
  ControllerZ.setTargetRelative(ControllerZ.getEncoderTurn()*usteps*200);
  ControllerM.setTargetRelative(ControllerM.getEncoderTurn()*usteps*200);
  ControllerC.setTargetRelative(ControllerC.getEncoderTurn()*usteps*200);

  motor_running();

  ControllerZ.setCurrentPos(0);
  ControllerM.setCurrentPos(0);
  ControllerC.setCurrentPos(0);
}

bool motor_running(){
  digitalWrite(LEDR,LOW);
  long time_start = millis();
  while(!(ControllerZ.isTargetReached() && ControllerM.isTargetReached() && ControllerC.isTargetReached())){
    if(millis()-time_start > TIMEOUT_MVMT){
      Serial.println("Something is wrong and I can feel it");
      ControllerZ.setTargetRelative(0);
      ControllerM.setTargetRelative(0);
      ControllerC.settargetRelative(0);
      digitalWrite(LEDR,HIGH);
      return false;
    }
  }
  digitalWrite(LEDR,HIGH);
  return true;
}
