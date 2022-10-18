
void refAllHome(){ //Initialisation routine
  task_start_time = millis();
  Serial.println("The device will start its initialization");
  LEDR_ON;
  DRIVER_ON;      //Motor enable
  *M4work_pntr = RPC.call("InitDrivers").as<bool>();
}

void Decap(){
  task_start_time = millis();
  Serial.println("The machine will now start the decaping routine. Keep clear");
  
  long C_pos = finalPos();
  Serial.print("Camera capture returned the value : ");
  Serial.println(C_pos);
  
  LEDR_ON;
  DRIVER_ON;
  RELAY_ON;
  *M4work_pntr = true;
  RPC.call("Decap",(int)C_pos).as<int>();
}


void Recap(){
  task_start_time = millis();
  Serial.println("The machine will now start the recaping routine. Keep clear");
  LEDR_ON;
  DRIVER_ON;
  RELAY_ON;
  *M4work_pntr = RPC.call("Recap").as<bool>();
}

void moveZ(){
  task_start_time = millis();
  Serial.println("Moving Z axis");
  LEDR_ON;
  DRIVER_ON;
  RELAY_ON;
  *M4work_pntr = RPC.call("MoveZ").as<bool>();
}
