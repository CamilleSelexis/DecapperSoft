
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
  *M4work_pntr = RPC.call("MoveZ").as<bool>();
}
void moveM(){
  task_start_time = millis();
  Serial.println("Moving M axis");
  *M4work_pntr = RPC.call("MoveM").as<bool>();
}
void moveC(){
  task_start_time = millis();
  Serial.println("Moving C axis");
  *M4work_pntr = RPC.call("MoveC").as<bool>();
}

void updateM4() {
  ZPos = RPC.call("ZPos").as<long>();
  ZTarget = RPC.call("ZTarget").as<long>();
  MPos = RPC.call("MPos").as<long>();
  MTarget = RPC.call("MTarget").as<long>();
  CPos = RPC.call("CPos").as<long>();
  CTarget = RPC.call("CTarget").as<long>();
}

void updateEncoder() {
  ZPosEnc = RPC.call("ZPosEnc").as<long>();
  MPosEnc = RPC.call("MPosEnc").as<long>();
  CPosEnc = RPC.call("CPosEnc").as<long>();
}
