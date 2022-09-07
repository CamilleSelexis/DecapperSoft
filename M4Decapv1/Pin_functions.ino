void pin_init() {
  
  pinMode(CS1, OUTPUT);
  digitalWrite(CS1, HIGH);
  pinMode(CS2, OUTPUT);
  digitalWrite(CS2, HIGH);
  pinMode(CS3,OUTPUT);
  digitalWrite(CS3,HIGH);
  pinMode(TGT1,INPUT);
  pinMode(TGT2,INPUT);
  pinMode(TGT3,INPUT);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);

  //Interrupt on the TGT pins
  attachInterrupt(digitalPinToInterrupt(TGT1),tgt1_reached,RISING);
  attachInterrupt(digitalPinToInterrupt(TGT2),tgt2_reached,RISING);
  attachInterrupt(digitalPinToInterrupt(TGT3),tgt3_reached,RISING);
}

void tgt1_reached(){
  state = 4;
}
void tgt2_reached(){
  state = 0;
}
void tgt3_reached(){
  state = 0;
}
