//use launchers so that the M7 is not held occupied

int launchDecap(long Cpos){
  *Pstate = 1;
  *PCpos = Cpos;
  return true;
}

bool launchRecap() {
  *Pstate = 2;
  return true;
}

bool launchInitialize() {
  *Pstate = 3;
  return true;
}

byte launchSetParams() {
  *Pstate = 4;
  return true;
}

bool launchReadParams() {
  *Pstate = 5;
  return true;
}

bool launchZrelMove(long value){
  *Pstate = 11;
  *pZvalue = value;
  return true;
}

bool launchMrelMove(long value){
  *Pstate = 12;
  *pMvalue = value;
  return true;
}

bool launchCrelMove(long value){
  *Pstate = 13;
  *pCvalue = value;
  return true;
}

bool launchZsetVMAX(long value){
  *Pstate = 41;
  *pZvalue = value;
  return true;
}

bool launchMsetVMAX(long value){
  *Pstate = 42;
  *pMvalue = value;
  return true;
}

bool launchCsetVMAX(long value){
  *Pstate = 43;
  *pCvalue = value;
  return true;
}

bool launchZsetCurrentScaling(int value){
  *Pstate = 51;
  *piZvalue = value;
  return true;
}
bool launchMsetCurrentScaling(int value){
  *Pstate = 52;
  *piMvalue = value;
  return true;
}
bool launchCsetCurrentScaling(int value){
  *Pstate = 53;
  *piCvalue = value;
  return true;
}

bool launchinitControllers(){
  *Pstate = 101;
  return true;
}
