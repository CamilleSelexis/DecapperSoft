
void refAllHome(){ //Initialisation routine
  task_start_time = millis();
  Serial.println("The device will start its initialization");
  LEDR_ON;
  *M4work_pntr = RPC.call("Initialize").as<bool>();
}

void Decap(){
  task_start_time = millis();
  Serial.println("The machine will now start the decaping routine. Keep clear");
  
  long C_pos = finalPos();
  Serial.print("Camera capture returned the value : ");
  Serial.println(C_pos);
  
  LEDR_ON;
  RELAY_ON;
  *M4work_pntr = true;
  RPC.call("Decap",C_pos).as<int>();
}


void Recap(){
  task_start_time = millis();
  Serial.println("The machine will now start the recaping routine. Keep clear");
  LEDR_ON;
  RELAY_ON;
  *M4work_pntr = RPC.call("Recap").as<bool>();
}
bool relMove(String currentLine){
  long value = 0;
  int valSize = currentLine.length()-8; //Gives the number of digit of the value
  char axis = currentLine[valSize]; //Gives which axis is concerned
  if(!(axis == 'Z' || axis == 'M' || axis == 'C')) return false;
  for(int i =2 ; i<valSize;i++){ //First char is \ second give the sign
    value = value*10 + currentLine[i]-48;
  }
  if(currentLine[1] == 45)  value = value*(-1);
  else if(currentLine[1] != 43){ value = 0; return false;}
  if(abs(value)>RELMOVE_LIMIT){
    Serial.println("Value out of range");
    value = 0;
    return false;
  }
  switch(axis){
    case 'Z':
      Serial.print("Calling ZrelMove with value : ");Serial.println(value);
      ZposBefore = ZPosEnc;
      LEDR_ON;
      if(!RPC.call("ZrelMove",value).as<bool>())
        Serial.println("Error calling ZrelMove");
      break;
    case 'M':
    MposBefore = MPosEnc;
      LEDR_ON;
      Serial.print("Calling MrelMove with value : ");Serial.println(value);
      if(!RPC.call("MrelMove",value).as<bool>())
        Serial.println("Error calling ZrelMove");
      break;
    case 'C':
    CposBefore = CPosEnc;
      LEDR_ON;
      Serial.print("Calling CrelMove with value : ");Serial.println(value);
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
bool setVMAX(String currentLine){
  long value= 0;
  int valSize = currentLine.length()-8;
  char axis = currentLine[valSize];
  if(!(axis == 'Z' || axis == 'M' || axis == 'C')) return false;
  for(int i=1; i<valSize; i++){
    value = value*10 + currentLine[i]-48; // 48 is ASCII for 0
  }
  if(value > 170000){ //Max = 200RPM = 3.33 RPS -> 200*256*200/60
    Serial.println("Value out of range");
    value = 0;
    return false;
  }

  switch(axis){
    case 'Z':
    Serial.print("Calling ZsetVMAX with value : ");Serial.println(value);
      if(!RPC.call("ZsetVMAX",value).as<bool>())
        Serial.println("Error calling ZsetVMAX");
      break;
    case 'M':
    Serial.print("Calling MsetVMAX with value : ");Serial.println(value);
      if(!RPC.call("MsetVMAX",value).as<bool>())
        Serial.println("Error calling MsetVMAX");
      break;
    case 'C':
    Serial.print("Calling CsetVMAX with value : ");Serial.println(value);
      if(!RPC.call("CsetVMAX",value).as<bool>())
        Serial.println("Error calling CsetVMAX");
      break;
    default :
      Serial.println("Unknown axis");
      break;
      return false;
  }
  return true;
}

bool setCurrentScaling(String currentLine){
  int value= 0;
  int valSize = currentLine.length()-18;
  char axis = currentLine[valSize];
  if(!(axis == 'Z' || axis == 'M' || axis == 'C')) return false;
  for(int i=1; i<valSize; i++){
    value = value*10 + currentLine[i]-48; // 48 is ASCII for 0
  }
  if(value > 32){
    Serial.println("Value out of range");
    value = 0;
    return false;
  }

  switch(axis){
    case 'Z':
    Serial.print("Calling ZsetCurrentScaling with value : ");Serial.println(value);
      if(!RPC.call("ZsetCurrentScaling",value).as<bool>())
        Serial.println("Error calling ZsetVMAX");
      break;
    case 'M':
    Serial.print("Calling MsetCurrentScaling with value : ");Serial.println(value);
      if(!RPC.call("MsetCurrentScaling",value).as<bool>())
        Serial.println("Error calling MsetVMAX");
      break;
    case 'C':
    Serial.print("Calling CsetCurrentScaling with value : ");Serial.println(value);
      if(!RPC.call("CsetCurrentScaling",value).as<bool>())
        Serial.println("Error calling CsetVMAX");
      break;
    default :
      Serial.println("Unknown axis");
      break;
      return false;
  }
  return true;
}

void initControllers(){
  RPC.call("initControllers").as<bool>();
}
