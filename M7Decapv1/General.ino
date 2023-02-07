void Status(){
  if(capHeld){
    Serial.println("I have a cap in my claws");
  } else if(isInit) {
    Serial.println("Machine available and initialised");
    } else {Serial.println("Machine available but not initialised");}
}
//This function returns the status of the decapper
//The status depends on 4 booleans, HS, capHeld, isInit and M4work
//0 : decapper out of order
//1 : decapper fully operational
//2 : decapper performed a decap and is waiting to recap
//3 : decapper not ready
//4 : decapper currently working
int getStatus(){
  int statusDecap = 0;
  if(HS)statusDecap = 0; //Decapper HS
  else if(M4work) statusDecap = 4; //Decapper working
  else if(!isInit) statusDecap = 3; //Decapper not init
  else if(capHeld) statusDecap = 2; //Decapper has a cap held, ready for recap
  else if(stopCalled) statusDecap = 5; //Stop has been called, need to perform a resume or abort before continuing
  else statusDecap = 1; //Decapper is ready
  
  return statusDecap; 
}

void resetFunc(void) {
  unsigned long *registerAddr;
  registerAddr = (unsigned long *)0xE000ED0C; //Writes to the AIRCR register of the stm32h747 to software restet the arduino
  //It is a 32 bit register set bit 2 to request a reset and write 0x05FA to enable the write
  //See Arm® v7-M Architecture Reference Manual for more information
  *registerAddr = (unsigned long) 0x05FA0304;
}

//Check that the encoder are within their tolerances
void checkEncoderStatus() {
  if(ZPos == ZTarget){
    if(abs(ZDevEnc) > 256) Zstate = false;
    else Zstate = true;
  }
  if(MPos == MTarget){
    if(abs(MDevEnc) > 256) Mstate = false;
    else Mstate = true;
  }
  if(CPos == CTarget){
    if(abs(CDevEnc) > 256) Cstate = false;
    else Cstate = true;
  }
}
