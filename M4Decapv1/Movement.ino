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

void motor_running(){
  digitalWrite(LEDR,LOW);
  while(!(ControllerZ.isTargetReached() && ControllerM.isTargetReached() && ControllerC.isTargetReached()))
  digitalWrite(LEDR,HIGH);
}

void Init
