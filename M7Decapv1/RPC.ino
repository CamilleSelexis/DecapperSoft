bool M4TaskCompleted(){
  *M4work_pntr = false;
  Serial.println("M4 finished its task and is now ready");
  return true;
}

//Properly set the flags once the init is done
bool initDone(){
  *M4work_pntr = false;
  *isInit_pntr = true;
  *capHeld_pntr = false;
  DRIVER_OFF;
  LEDR_OFF;
  RELAY_OFF;
  Serial.print("initDone, took ");Serial.print(millis()-task_start_time);Serial.println("ms");
  return true;
}

bool decapDone(){
  *M4work_pntr = false;
  *capHeld_pntr = true;
  DRIVER_OFF;
  LEDR_OFF;
  RELAY_OFF;
  Serial.print("decapDone, took ");Serial.print(millis()-task_start_time);Serial.println("ms");
  return true;
}

bool recapDone(){
  *M4work_pntr = false;
  *capHeld_pntr = false;
  DRIVER_OFF;
  LEDR_OFF;
  RELAY_OFF;
  Serial.print("recapDone, took ");Serial.print(millis()-task_start_time);Serial.println("ms");
  return true;
}

bool ZCurrentPos(long motorPos){
  ZPos = motorPos;
  return true;
}
