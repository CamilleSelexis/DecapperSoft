
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
bool relMove(String currentLine){
  long value = 0;
  int valSize = currentLine.length()-8; //Gives the number of digit of the value
  char axis = currentLine[valSize]; //Gives which axis is concerned
  Serial.println(axis);
  if(!(axis == 'Z' || axis == 'M' || axis == 'C')) return false;
  for(int i =2 ; i<valSize;i++){ //First char is \ second give the sign
    value = value*10 + currentLine[i]-48;
  }
  if(currentLine[1] == 45)  value = value*(-1);
  else if(currentLine[1] != 43){ value = 0; return false;}
  if(abs(value)>1000000){
    Serial.println("Value out of range");
    value = 0;
    return false;
  }
  Serial.print("Computed value = ");Serial.println(value);
  switch(axis){
    case 'Z':
      if(!RPC.call("ZrelMove",value).as<bool>())
        Serial.println("Error calling ZrelMove");
      break;
    case 'M':
      if(!RPC.call("MrelMove",value).as<bool>())
        Serial.println("Error calling ZrelMove");
      break;
    case 'C':
      if(!RPC.call("CrelMove",value).as<bool>())
        Serial.println("Error calling ZrelMove");
      break;
    default :
      Serial.println("Unknown axis");
      break;
      return false;
  }
  return true;
}
void updateM4() {
  //Only ask M4 for an update if he is available
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
