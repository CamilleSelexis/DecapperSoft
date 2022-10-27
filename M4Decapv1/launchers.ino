//use launchers so that the M7 is not held occupied

int launchDecap(int Cpos){
  *Pstate = 1;
  *PCpos = Cpos;
  return true;
}

bool launchRecap() {
  *Pstate = 2;
  return true;
}

bool launchInitDrivers() {
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

bool launchMoveZ(){
  *Pstate = 11;
  return true;
}
bool launchMoveM(){
  *Pstate = 12;
  return true;
}
bool launchMoveC(){
  *Pstate = 13;
  return true;
}

long launchZPos() {
  *Pstate = 31;
  return ZPos;
}

long launchMPos() {
  *Pstate = 32;
  return MPos;
}

long launchCPos() {
  *Pstate = 33;
  return CPos;
}
long launchZTarget() {
  return ZTarget;
}
long launchMTarget() {
  return MTarget;
}
long launchCTarget() {
  return CTarget;
}
