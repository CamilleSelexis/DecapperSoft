//Set the pin as output or input
void pin_init() {

  pinMode(CS1, OUTPUT);
  digitalWrite(CS1, HIGH);
  pinMode(CS2, OUTPUT);
  digitalWrite(CS2, HIGH);
  pinMode(CS3,OUTPUT);
  digitalWrite(CS3,HIGH);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);

  pinMode(EN_PIN,OUTPUT);
  digitalWrite(EN_PIN,LOW);
  pinMode(TGT1,INPUT);
  pinMode(TGT2,INPUT);
  pinMode(TGT3,INPUT);
  /*
  attachInterrupt(digitalPinToInterrupt(TGT1),tgt1_reached,FALLING);
  attachInterrupt(digitalPinToInterrupt(TGT2),tgt2_reached,FALLING);
  attachInterrupt(digitalPinToInterrupt(TGT3),tgt3_reached,FALLING);
  */
}
/*
void tgt1_reached(){
  state = 21;
}
void tgt2_reached(){
  state = 22;
}
void tgt3_reached(){
  state = 23;
}
*/
