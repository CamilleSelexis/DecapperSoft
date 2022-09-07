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
//1 : decapper available
//2 : decapper used
//3 : decapper not init
//4 : decapper working
int getStatus(){
  int statusDecap = 0;
  if(HS)statusDecap = 0; //Decapper HS
  else if(M4work) statusDecap = 4; //Decapper working
  else if(!isInit) statusDecap = 3; //Decapper not init
  else if(capHeld) statusDecap = 2; //Decapper has a cap held, ready for recap
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
