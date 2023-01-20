void setup(){
  Serial.begin(115200);
  // configure LED PWM functionalitites
  ledcSetup(0, 1000, 8);
  //pinMode(27, OUTPUT);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(32, 0);
}

void loop(){
  if (Serial.available()){
    
    int v = Serial.readString().toInt();
    Serial.print("v: ");
    Serial.println(v);
    int value = (v * 100/255) +2;
//   
    
    Serial.print("value: ");
    Serial.println(value);
    ledcWrite(0, value);
    dacWrite(32, value);
  }
//ledcWrite(0, 180);
  //digitalWrite(27,HIGH);
//  ledcWrite(0, 0);
//  delay(2000);
//  ledcWrite(0, 50);
//  delay(2000);
  //digitalWrite(27,LOW);
//  ledcWrite(0, 126);
//  delay(2000);
//  ledcWrite(0, 200);
//  delay(2000);
//  ledcWrite(0, 255);
//  delay(2000);
}
