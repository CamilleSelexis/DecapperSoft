bool ZsetVMAX(long value){
  ControllerZ.setVMAX(value<<8,0); //set VMAX in standard format Usteps/s
  return true;
}
bool MsetVMAX(long value){
  ControllerM.setVMAX(value<<8,0);
  return true;
}
bool CsetVMAX(long value){
  ControllerC.setVMAX(value<<8,0);
  return true;
}
bool ZsetCurrentScaling(int value){
  ControllerZ.setCurrentScale(value);
  return true;
}
bool MsetCurrentScaling(int value){
  ControllerM.setCurrentScale(value);
  return true;
}
bool CsetCurrentScaling(int value){
  ControllerC.setCurrentScale(value);
  return true;
}
