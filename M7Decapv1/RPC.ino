bool M4TaskCompleted(){
  *M4work_pntr = false;
  Serial.println("M4 finished its task and is now ready");
  return true;
}

//Properly set the flags once the init is done
bool initDone(bool ZEnc, bool MEnc, bool CEnc){
  *M4work_pntr = false;
  *isInit_pntr = true;
  *capHeld_pntr = false;
  DRIVER_OFF;
  LEDR_OFF;
  RELAY_OFF;
  Zstate = ZEnc;
  Mstate = MEnc;
  Cstate = CEnc;
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

//bool currentMotorPositionRPC(long ZPosRPC, long MPosRPC, long CPosRPC, long ZTargetRPC, long MTargetRPC, long CTargetRPC){
bool currentMotorPositionRPC(long ZPosRPC, long MPosRPC, long CPosRPC,
                            long ZTargetRPC, long MTargetRPC, long CTargetRPC,
                              long ZPosEncRPC, long MPosEncRPC, long CPosEncRPC,
                              float ZAngleEncRPC, float MAngleEncRPC, float CAngleEncRPC,
                              float ZTurnEncRPC, float MTurnEncRPC, float CTurnEncRPC){
  ZPos = ZPosRPC;
  MPos = MPosRPC;
  CPos = CPosRPC;
  ZTarget = ZTargetRPC;
  MTarget = MTargetRPC;
  CTarget = CTargetRPC;
  ZPosEnc = ZPosEncRPC;
  MPosEnc = MPosEncRPC;
  CPosEnc = CPosEncRPC;
  ZAngleEnc = ZAngleEncRPC;
  MAngleEnc = MAngleEncRPC;
  CAngleEnc = CAngleEncRPC;
  ZTurnEnc = ZTurnEncRPC;
  MTurnEnc = MTurnEncRPC;
  CTurnEnc = CTurnEncRPC;
  return true;
}
