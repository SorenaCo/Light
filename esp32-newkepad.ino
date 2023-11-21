#include <Keypad.h>   //keypad library
#include <OLED_I2C.h>  //OLED library
#include "EEPROM.h"     //EEPROM library

#define f1  14
#define f2  26
#define f3  33
#define f4  35

#define R1  12
#define R2  27

#define R3  23

OLED  myOLED(22, 21, 8); //OLED object

extern uint8_t SmallFont[];

extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {18, 2, 0, 16}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {17, 19, 4}; //connect to the column pinouts of the keypad
//Create an object of keypad
Keypad keypad1 = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); //Keypad object
String password = "";
String input_phrase = ""; //Client input phrase
bool pass_state = false;  //True if password == input else false
int OLED_count = 0;
bool pass_state_change1 = false;   //First confirmation for changing password by clicking on '#'
String new_password = "";          // Temporary new password variable for saving on EEPROM and password
bool pass_state_change2 = false;   //Socend confirmation for changing password by clicking on '#'
bool pass_state_change3 = false;   //Socend confirmation for changing password by clicking on '#'
int floor_name = 0;                //Current floor
bool Relay_3 = false;
void setup() {
  Serial.begin(9600);
  //defining input variable for each floor switch
  pinMode(f1, INPUT);
  pinMode(f2, INPUT);
  pinMode(f3, INPUT);
  pinMode(f4, INPUT);
  // defining output variable for each output
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
  digitalWrite(R3, LOW);
  if (!myOLED.begin(SSD1306_128X32))
    while (1);  // In case the library failed to allocate enough RAM for the display buffer...
  myOLED.setFont(SmallFont);
  //EEPROM reading password
  //EEPROM.begin(64);
  EEPROM.begin(128);
  int pass_len = EEPROM.read(0);  //Length of password in EEPROM
  String loading = "Loading";
  for (int j = 0 ; j < pass_len; j++) {
    password += char(EEPROM.read(j + 1));
    loading += ".";
    myOLED.print(loading, CENTER, 0);
    myOLED.print("Sorena", CENTER, 15);
    myOLED.update();
    delay(1000);
  }
  myOLED.setFont(SmallFont);
  myOLED.clrScr();
  myOLED.print("Enter password", LEFT, 0);
  myOLED.update();
  Serial.println(password);

}

void loop() {
  char key = keypad1.getKey();// Read the key
  bool first_floor = digitalRead(f1);
  bool second_floor = digitalRead(f2);
  bool third_floor = digitalRead(f3);
  bool forth_floor = digitalRead(f4);

  // statement for checking the position of elevator to stop the engine
  if (floor_name == 1 and first_floor == 1) {
    myOLED.clrScr();
    myOLED.print("stop floor1", LEFT, 0);
    myOLED.print("To change password ", LEFT, 7);
    myOLED.print("press # ", LEFT, 14);
    myOLED.update();
    //floor_name = 1;
    Serial.println("f1");
    digitalWrite(R1, LOW);
    digitalWrite(R2, LOW);
    floor_name = 0;
  } else if (floor_name == 2 and second_floor == 1) {
    myOLED.clrScr();
    myOLED.print("stop floor2", LEFT, 0);
    myOLED.print("To change password ", LEFT, 7);
    myOLED.print("press # ", LEFT, 14);
    myOLED.update();
    //floor_name = 2;
    Serial.println("f2");
    digitalWrite(R1, LOW);
    digitalWrite(R2, LOW);
    digitalWrite(R1, HIGH);
    delay(200);
    digitalWrite(R1, LOW);
    floor_name = 0;
  } if (floor_name == 3 and third_floor == 0) {
    Serial.println("HERE!");
    myOLED.clrScr();
    myOLED.print("stop floor3", LEFT, 0);
    myOLED.print("To change password ", LEFT, 7);
    myOLED.print("press # ", LEFT, 14);
    myOLED.update();
    //floor_name = 3;
    Serial.println("f3");
    digitalWrite(R1, LOW);
    digitalWrite(R2, LOW);
    digitalWrite(R1, HIGH);
    delay(200);
    digitalWrite(R1, LOW);
    floor_name = 0;
  }
  if (floor_name == 4 and forth_floor == 1) {
    Serial.println("HERE!");
    myOLED.clrScr();
    myOLED.print("stop floor4", LEFT, 0);
    myOLED.print("To change password ", LEFT, 7);
    myOLED.print("press # ", LEFT, 14);
    myOLED.update();
    //floor_name = 3;
    Serial.println("f4");
    digitalWrite(R2, LOW);
    digitalWrite(R1, HIGH);
    delay(200);
    digitalWrite(R1, LOW);
    floor_name = 0;

  }

  if (key) {
    Serial.println(key);

  }
  // statement for changing and authorizing the password and moving the elevator
  if (pass_state and key and pass_state_change3) {
    if (key == '#' and new_password.length() >= 4) {
      EEPROM.begin(128);
      EEPROM.write(0, new_password.length());
      EEPROM.commit();
      for (int i = 0; i < new_password.length(); i++) {
        EEPROM.write(i + 1, new_password[i]);
        EEPROM.commit();
      }
      password = new_password;
      myOLED.clrScr();
      myOLED.print("New password saved", LEFT, 0);
      myOLED.update();
      delay(1500);
      myOLED.clrScr();
      myOLED.print("Enter password", LEFT, 0);
      myOLED.update();
      pass_state_change3 = false;
      pass_state = false;
      new_password = "";
    } else if (key == '#' and new_password.length() < 4) {
      myOLED.clrScr();
      myOLED.print("Password is too", LEFT, 0);
      myOLED.print("short", LEFT, 8);
      myOLED.update();
      delay(2500);
      myOLED.clrScr();
      myOLED.print("Enter new password", LEFT, 0);
      myOLED.update();
      new_password = "";
    }
    else if (key == '*') {
      myOLED.clrScr();
      myOLED.print("Invalid character!", LEFT, 0);
      myOLED.print("Insert new password:", LEFT, 8);
      myOLED.update();
      new_password = "";
    } else {
      new_password += key;
      myOLED.clrScr();
      myOLED.print(new_password, LEFT, 0);
      myOLED.update();
    }

  } else if (pass_state and key and pass_state_change2) {
    if (key == '#') {
      pass_state_change2 = false;
      pass_state_change3 = true;
      myOLED.clrScr();
      myOLED.print("Insert new password", LEFT, 0);
      myOLED.update();
    } else if (key == '*') {
      pass_state = false;
      Serial.println("OUT");
      myOLED.clrScr();
      myOLED.print("OUT", LEFT, 0);
      myOLED.update();
      delay(1500);
      myOLED.clrScr();
      myOLED.print("Enter password", LEFT, 0);
      myOLED.update();
    }
  } else if (pass_state and key and pass_state_change1) {
    if (key == '#') {
      pass_state_change1 = false;
      pass_state_change2 = true;
      myOLED.clrScr();
      myOLED.print("To change password ", LEFT, 0);
      myOLED.print("press # ", LEFT, 8);
      myOLED.update();
    } else if (key == '*') {
      pass_state = false;
      Serial.println("OUT");
      myOLED.clrScr();
      myOLED.print("OUT", LEFT, 0);
      myOLED.update();
      delay(1500);
      myOLED.clrScr();
      myOLED.print("Enter password", LEFT, 0);
      myOLED.update();
    }
  } else if (pass_state and key) {
    input_phrase = "";
    if (key == '*') {
      pass_state = false;
      Serial.println("OUT");
      myOLED.clrScr();
      myOLED.print("OUT", LEFT, 0);
      myOLED.update();
      delay(1500);
      myOLED.clrScr();
      myOLED.print("Enter password", LEFT, 0);
      myOLED.update();
    } else if (key == '1') {
      myOLED.clrScr();
      myOLED.print("First floor...", LEFT, 0);
      myOLED.update();
      floor_name = 1;
      digitalWrite(R1, HIGH);
      Relay_3 = false;
    } else if (key == '2' and first_floor == 1) {
      myOLED.clrScr();
      myOLED.print("Second Floor...", LEFT, 0);
      myOLED.print("Down ", LEFT, 8);
      myOLED.update();
      floor_name = 2;
      digitalWrite(R2, HIGH);
      Relay_3 = false;

    } else if (key == '2' and (third_floor == 0 or forth_floor == 1)) {
      myOLED.clrScr();
      myOLED.print("Second floor...", LEFT, 0);
      myOLED.print("Up ", LEFT, 8);

      myOLED.update();
      floor_name = 2;
      digitalWrite(R1, HIGH);
      Relay_3 = false;

    } else if (key == '3' and (first_floor == 1 or second_floor == 1)) {
      myOLED.clrScr();
      myOLED.print("Third floor...", LEFT, 0);
      myOLED.update();
      floor_name = 3;
      digitalWrite(R2, HIGH);
      Relay_3 = false;

    } else if (key == '3' and forth_floor == 1 ) {
      myOLED.clrScr();
      myOLED.print("Third floor...", LEFT, 0);
      myOLED.update();
      floor_name = 3;
      digitalWrite(R1, HIGH);
      Relay_3 = false;

    } else if (key == '4') {
      myOLED.clrScr();
      myOLED.print("Forth floor...", LEFT, 0);
      myOLED.update();
      floor_name = 4;
      digitalWrite(R2, HIGH);
      Relay_3 = true;

    } else if (key == '0') {
      myOLED.clrScr();
      myOLED.print("User stop!", LEFT, 0);
      myOLED.print("please back to 1", LEFT, 7);
      myOLED.update();
      floor_name = 4;
      digitalWrite(R2, LOW);
      digitalWrite(R1 , LOW);
      digitalWrite(R1, HIGH);
      delay(50);
      digitalWrite(R1, LOW);

    } else if (key == '8') {
      myOLED.clrScr();
      myOLED.print("User Up!", LEFT, 0);
      myOLED.print("Press 0", LEFT, 7);
      myOLED.update();
      floor_name = 4;
      digitalWrite(R2, LOW);
      digitalWrite(R1, HIGH);
    } else if (key == '9') {
      myOLED.clrScr();
      myOLED.print("User Down!", LEFT, 0);
      myOLED.print("Press 0", LEFT, 7);
      myOLED.update();
      floor_name = 4;
      digitalWrite(R1, LOW);
      digitalWrite(R2, HIGH);
    } else if (key == '#') {
      myOLED.clrScr();
      myOLED.print("To change password ", LEFT, 0);
      myOLED.print("press # ", LEFT, 8);
      myOLED.update();
      pass_state_change1 = true;
    }

  } else if (key == '#') {
    Serial.println("here");
    Serial.println(input_phrase);
    if (input_phrase == password) {
      Serial.println("Pass is currect!");
      pass_state = true;
      myOLED.clrScr();
      myOLED.print("Pass is currect!", LEFT, 0);
      myOLED.print("Ready! ", LEFT, 8);

      myOLED.update();
    } else {
      Serial.println("Pass is not currect!");
      myOLED.clrScr();
      myOLED.print("Pass is not currect!", LEFT, 0);
      myOLED.print("Enter password", LEFT, 8);
      myOLED.update();
    }
    input_phrase = "";
  } else if (key) {
    input_phrase += key;
    myOLED.clrScr();
    myOLED.print("Enter password:", LEFT, 0);
    myOLED.print(input_phrase, LEFT, 9);
    myOLED.update();


  }




}
