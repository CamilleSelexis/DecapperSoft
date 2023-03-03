bool M4TaskCompleted(){
  *M4work_pntr = false;
  Serial.println("M4 finished its task and is now ready");
  return true;
}

//Properly set the flags once the init is done
bool initDone(bool ZEnc, bool MEnc, bool CEnc){
  //reset all status flags
  *M4work_pntr = false;
  *isInit_pntr = true;
  *capHeld_pntr = false;
  errorState = false;
  LEDR_OFF;
  RELAY_OFF;
  Zstate = ZEnc;
  Mstate = MEnc;
  Cstate = CEnc;
  Serial.print("initDone, took ");Serial.print(millis()-task_start_time);Serial.println("ms");
  if(ZEnc && MEnc && CEnc){
    return true;    
  }
  else{
    errorState = true;
    return false;
   }
}

bool decapDone(bool decapSuccess){
  *M4work_pntr = false;
  LEDR_OFF;
  RELAY_OFF;
  Serial.print("decapDone, took ");Serial.print(millis()-task_start_time);Serial.println("ms");
  if(decapSuccess){
     *capHeld_pntr = true;
  }
  else{
    errorState = true;
    return false;
  }
  return true;
}

bool recapDone(bool recapSuccess){
  *M4work_pntr = false;
  LEDR_OFF;
  RELAY_OFF;
  Serial.print("recapDone, took ");Serial.print(millis()-task_start_time);Serial.println("ms");
  if(recapSuccess){
    *capHeld_pntr = false;
  }
  else{
    errorState = true;
    return false;
  }
  return true;
}
//---------------------------------------------------Can be removed
bool ZCurrentPos(long motorPos){
  ZPos = motorPos;
  return true;
}
//--------------------------------------------------------------------------
//bool currentMotorPositionRPC(long ZPosRPC, long MPosRPC, long CPosRPC, long ZTargetRPC, long MTargetRPC, long CTargetRPC){
bool currentMotorPositionRPC(long ZPosRPC, long MPosRPC, long CPosRPC,
                            long ZTargetRPC, long MTargetRPC, long CTargetRPC,
                              uint32_t ZPosEncRPC, uint32_t MPosEncRPC, uint32_t CPosEncRPC,
                              float ZAngleEncRPC, float MAngleEncRPC, float CAngleEncRPC,
                              float ZTurnEncRPC, float MTurnEncRPC, float CTurnEncRPC,
                              long ZDevEncRPC, long MDevEncRPC, long CDevEncRPC){
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
  /*ZTurnEnc = ZPosEnc - ZposBefore;
  MTurnEnc = MPosEnc - MposBefore;
  CTurnEnc = CPosEnc - CposBefore;*/
  ZTurnEnc = ZTurnEncRPC;
  MTurnEnc = MTurnEncRPC;
  CTurnEnc = CTurnEncRPC;
  ZDevEnc = ZDevEncRPC;
  MDevEnc = MDevEncRPC;
  CDevEnc = CDevEncRPC;
  return true;
}
