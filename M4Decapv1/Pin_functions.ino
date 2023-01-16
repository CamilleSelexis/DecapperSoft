//Set the pin as output or input
void pin_init() {

  pinMode(CS_Z, OUTPUT);
  digitalWrite(CS_Z, HIGH);
  pinMode(CS_M, OUTPUT);
  digitalWrite(CS_M, HIGH);
  pinMode(CS_C,OUTPUT);
  digitalWrite(CS_C,HIGH);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);

  pinMode(EN_PIN,OUTPUT);
  //digitalWrite(EN_PIN,LOW);
  pinMode(TGT_Z,INPUT);
  pinMode(TGT_M,INPUT);
  pinMode(TGT_C,INPUT);
  
  //attachInterrupt(TGT_Z,tgtZ_reached,FALLING);
  attachInterrupt(TGT_M,tgtM_reached,FALLING);
  attachInterrupt(TGT_C,tgtC_reached,FALLING); //Cannot use D4 as interrupt source
  
//  attachInterrupt(digitalPinToInterrupt(TGT_Z),tgt1_reached,FALLING);
//  attachInterrupt(digitalPinToInterrupt(TGT_M),tgt2_reached,FALLING);
//  attachInterrupt(digitalPinToInterrupt(TGT_C),tgt3_reached,FALLING);
}

void tgtZ_reached(){
  Zrunning = false;
}
void tgtM_reached(){
  Mrunning = false;
}
void tgtC_reached(){
  Crunning = false;
}
