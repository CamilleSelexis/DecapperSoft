void motor_low_speed(){
  //Set the speed to 1 rot/s
  ControllerZ.setVMAX(1,1);
  ControllerM.setVMAX(1,1);
  ControllerC.setVMAX(1,1);
}

void motor_high_speed(){
  //Set the speed to 5 rot/s
  ControllerZ.setVMAX(5,1);
  ControllerM.setVMAX(5,1);
  ControllerC.setVMAX(5,1);
}
