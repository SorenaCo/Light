
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <EEPROM.h>

const int csPin = 5;          // LoRa radio chip select
const int resetPin = 14;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin
const int On_pin = 16;
const int Off_pin = 17;
const int Sig_lamp = 4;

bool On_pin_state = true;
bool Off_pin_state = true;


String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0x536f72656e61; //Sorena    // address of this device
byte destination = 0x5669726154656368;  //Viratech    // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

void setup() {
  Serial.begin(115200);                   // initialize serial
  while (!Serial);
  EEPROM.begin(128);
  int defaul_state = EEPROM.read(0);
  pinMode(12, OUTPUT);
  pinMode(On_pin, INPUT);
  pinMode(Off_pin, INPUT_PULLUP);
  pinMode(Sig_lamp, OUTPUT);
  digitalWrite(Sig_lamp, defaul_state);
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
  LoRa.setTxPower(20);
  
  Serial.println("LoRa init succeeded.");
  //sendMessage("#S!");
}

void loop() {
  On_pin_state = digitalRead(On_pin);
  if (On_pin_state){
    Serial.println(analogRead(Off_pin));
    String message = "#I!";
    sendMessage(message);
    Serial.println("Sending " + message);
  }
  Off_pin_state = digitalRead(Off_pin);
  if (!Off_pin_state){
    Serial.println(analogRead(Off_pin));
    String message = "#O!";
    sendMessage(message);
    Serial.println("Sending " + message);
    Serial.println(analogRead(Off_pin));
  }
  //Serial.println(analogRead(On_pin));
  //delay(500);

  if (Serial.available()) {
    String message = Serial.readString();   // send a message
    sendMessage(message);
    Serial.println("Sending " + message);
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //Serial.println("Waiting...");
  //delay(100);
}

void sendMessage(String outgoing) {
  
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address  
  LoRa.write(localAddress);             // add sender address
  //LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  Serial.println("End Packet!");
  if(LoRa.endPacketMATG(false) == 0){
    ESP.restart();
  }
  //Serial.println(LoRa.endPacketMATG(false));// finish packet and send it
  //LoraBegin();
  //Serial.println("In Send!");  
  //msgCount++; 
  Serial.println("End Send!");                            // increment message ID
  //delay(500);
}

int onReceive(int packetSize) {
  String text;
  if (packetSize == 0) return 0;          // if there's no packet, return

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
    return 0;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return 0;                             // skip rest of function
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
  EEPROM.begin(128);
  if (text == "I"){
    digitalWrite(Sig_lamp, HIGH);
    EEPROM.write(0,1);
  }else if(text == "O"){
    digitalWrite(Sig_lamp, LOW);
    EEPROM.write(0,0);
  }
  EEPROM.commit();
  return 1;
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
