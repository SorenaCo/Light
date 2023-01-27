
#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 5;          // LoRa radio chip select
const int resetPin = 14;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin
//const int On_pin = 16;
//const int Off_pin = 17;
//const int Sig_lamp = 4;
const int Moto_state_pin = 27;
const int Start_pin = 26;
const int  Stop_pin = 25;

bool Moto_state1 = false;
bool Moto_state2 = false;

bool On_pin_state = true;
bool Off_pin_state = true;

bool change_primary = false;
bool change_confirm = false;
bool check_1 = true;
unsigned long tm_check = 0; //Set a variable to save time
int interval_check = 1000;  //Intrval time to check the state

unsigned long tm_state_send = 0;

String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0x5669726154656368;     // address of this device
byte destination = 0x536f72656e61;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

void setup() {
  Serial.begin(115200);                   // initialize serial
  while (!Serial);
  pinMode(Moto_state_pin, INPUT);
  pinMode(Start_pin, OUTPUT);
  pinMode(Stop_pin, OUTPUT);
  
  digitalWrite(Start_pin, LOW);
  digitalWrite(Stop_pin, LOW);
  Serial.println("LoRa Duplex");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin
  LoRa.setSPIFrequency(200000);
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  LoRa.setSpreadingFactor(12);
  //LoRa.setSignalBandwidth(125E3);
  LoRa.setTxPower(12);
  
  Serial.println("LoRa init succeeded.");

  delay(1000);
  Moto_state1 = digitalRead(Moto_state_pin);
  if (Moto_state1){
    sendMessage("#I!");
  }else if (!Moto_state1){
    sendMessage("#O!");
  }
    Moto_state2 = Moto_state1;
    tm_state_send = millis();
}

void loop() {
  Moto_state2 = digitalRead(Moto_state_pin);
//  if (Moto_state2 != Moto_state1) {
//    if (Moto_state2){
//    sendMessage("#I!");
//    Serial.println("I");
//  }else if (!Moto_state2){
//    sendMessage("#O!");
//    Serial.println("O");
//  }
//  Moto_state1 = Moto_state2;
//    //Serial.println("Check loop");
//    }
  
  if (Moto_state2 != Moto_state1 and check_1) {
    tm_check = millis();
    check_1 = false;
    change_primary = true;
    Serial.println("Check loop");
    }
 if (Moto_state2 == Moto_state1 and change_primary){
    check_1 = true;
    change_primary = false;
    Serial.println("change primary loop");
 }
  if ((millis() - tm_check)>interval_check and change_primary){
      if(Moto_state2 != Moto_state1){
        change_confirm = true;
      }else{
        check_1 = true;
        change_primary = false;
      }
    }
  if(change_confirm){
    if (Moto_state2){
    sendMessage("#I!");
  }else if (!Moto_state2){
    sendMessage("#O!");
  }
    Moto_state1 = Moto_state2;
    change_confirm = false;
    check_1 = true;
    change_primary = false;
    Serial.print("Moto state1: ");
    Serial.print(Moto_state1);
  }
//  Serial.print("State1: ");
//  Serial.print(Moto_state1);
//  Serial.print("       State2: ");
//  Serial.println(Moto_state2);
//  delay(500);

  if (Serial.available()) {
    String message = Serial.readString();   // send a message
    sendMessage(message);
    Serial.println("Sending " + message);
  }
  if ((millis() - tm_state_send)>1200000){
    if (Moto_state1){
    sendMessage("#I!");
  }else if (!Moto_state1){
    sendMessage("#O!");
  }
  tm_state_send = millis();
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) {
  
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address  
  LoRa.write(localAddress);             // add sender address
  //LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  Serial.println("End packet");
  if(LoRa.endPacketMATG(false) == 0){
    ESP.restart();
  }   // finish packet and send it
  //LoraBegin();
  //Serial.println("In Send!");  
  //msgCount++; 
  Serial.println("End Send!");                            // increment message ID
}

void onReceive(int packetSize) {
  String text;
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  //byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  //Serial.println("Received from: 0x" + String(sender, HEX));
  //Serial.println("Sent to: 0x" + String(recipient, HEX));
  //Serial.println("Message ID: " + String(incomingMsgId));
  //Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
  //sendMessage(incoming);
  //Serial.println("Sending " + incoming);
  text = incoming.substring(incoming.indexOf("#") + 1, incoming.indexOf("!"));
  if (text == "I"){
    digitalWrite(Start_pin, HIGH);
    delay(500);
    digitalWrite(Start_pin, LOW);
  }else if(text == "O"){
    digitalWrite(Stop_pin, HIGH);
    delay(500);
    digitalWrite(Stop_pin, LOW);
  }else if(text == "S"){
    if (Moto_state1){
    sendMessage("#I!");
    }else if (!Moto_state1){
    sendMessage("#O!");
    }
  }
}

void LoraBegin(){
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin
  LoRa.setSPIFrequency(8e6);
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  LoRa.setSpreadingFactor(12);
  //LoRa.setSignalBandwidth(125E3);
  //LoRa.setTxPower(20);
  
  Serial.println("LoRa init succeeded.");
}
