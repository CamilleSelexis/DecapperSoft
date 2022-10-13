//this function is called when a motor is running and returns true when each motor is done, return false if the movement timeouts (30 sec)
bool motor_running(){
  digitalWrite(LEDR,LOW);
  long time_start = millis();
  while(!(ControllerZ.isTargetReached() && ControllerM.isTargetReached() && ControllerC.isTargetReached())){
    if(millis()-time_start > TIMEOUT_MVMT){
      Serial.println("Something is wrong and I can feel it");
      ControllerZ.setTargetRelative(0);
      ControllerM.setTargetRelative(0);
      ControllerC.setTargetRelative(0);
      digitalWrite(LEDR,HIGH);
      return false;
    }
  }
  digitalWrite(LEDR,HIGH);
  return true;
}
