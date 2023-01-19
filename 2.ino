int red = 9;
int green = 10;
int blue = 11;

int r;
int g;
int b;


void setup() {
  Serial.begin(9600);
  pinMode(red,OUTPUT);  
  pinMode(green,OUTPUT);
  pinMode(blue,OUTPUT);

}

void loop() {
  while(Serial.available()){
    r = Serial.parseInt();
    g = Serial.parseInt();
    b = Serial.parseInt();
    }
  analogWrite(red,r);
  analogWrite(green,g);
  analogWrite(blue,b);

}
