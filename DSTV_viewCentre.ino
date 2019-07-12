#include <EEPROM.h>

#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SPI.h>
//#include <string.h>

//sda=10, rst=9, sck=13, miso=12, mosi=11
#define SS_PIN 10
#define RST_PIN 9
#define redLed 2
#define greenLed 3
#define blueLed 4
#define buz 5
#define button 6
#define button2 8

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7,3,POSITIVE);


String strID[4] = {"1178642119","16516312899","6174452"};
char one[] = "YUKEN";
char two[] = "YUSUF";
byte cardId[4];
int cardss[4];
boolean okay = true;
byte users;

void cardAdder(){
  byte cardVal[4];
  while(okay && digitalRead(button) == HIGH){
    getCardId();
    if(cardId[0] != 0 && cardId[1] != 0){
      if(validate(cardId) == 0){
        for(int n = 0; n < 4; n++){
          cardVal[n] = cardId[n]; 
        }
        saveCard(cardVal, location());
        lcd.clear();
        lcd.print("SAVE CONFIRMED");
        indicator('g',1);
        delay(500);
        indicator('g',0);
        delay(500);
        while(digitalRead(button) == HIGH){
          digitalWrite(buz, HIGH);
        }
        digitalWrite(buz, LOW);
        delay(500);
      }
      else{
        lcd.clear();
        lcd.print("CARD PRE-EXISTED");
        buzzer('l',1);    
        indicator('r',1);
        delay(500);
        indicator('r',0);
        delay(500);   
      }
    }
  }
  for(int r = 0; r < 4; r++){
        cardId[r] = 0;
      }
}
int location(){
  int addr; int stepper = 1; byte temp[4];
  while(stepper <= EEPROM.length()){
    int tmp = stepper; 
    for(int i = 0; i < 4; i++){
      EEPROM.get(stepper, temp[i]);
      stepper++;
    }
    stepper++;
    if(temp[0] == 0 && temp[1] == 0 && temp[2] == 0 && temp[3] == 0){
      addr = tmp;
      break;
    }  
  }
  return addr;
}

void cardRemover(){
  byte cardVal[4];
  int counter = 1;
  byte temp;
  while(okay == true && digitalRead(button2) == HIGH){
    getCardId();
    if(cardId[0] != 0 && cardId[1] != 0){
      if(validate(cardId) == 1){
        while(counter <= EEPROM.length()){
          int temp = counter;
          for(int c = 0; c < 4; c++){
            EEPROM.get(counter, cardVal[c]);
            counter++;
          }
          if(cardId[0] == cardVal[0] && cardId[1] == cardVal[1] && cardId[2] == cardVal[2] && cardId[3] == cardVal[3]){
            for(int c = temp; c <= counter; c++){
              EEPROM.write(c, 0);
            }    
            lcd.clear();
            lcd.print("CARD REMOVED");
            indicator('r',1);
            delay(500);
            indicator('r',0);
            delay(500);
            EEPROM.get(0, users);
            users = users - 1;
            EEPROM.write(0, users);
          }
          counter++; 
        }
        while(digitalRead(button2) == HIGH){
          digitalWrite(buz, HIGH);
        }
        digitalWrite(buz, LOW);
        delay(500);
      }
      else{
        lcd.clear();
        lcd.print("CARD NOT FOUND");
        buzzer('l',1);
        indicator('r',1);
        delay(500);
        indicator('r',0);
        delay(500);   
      }
    }
  }
  for(int r = 0; r < 4; r++){
        cardId[r] = 0;
      }
}


int validate(byte conf[4]){
  int checker = 0;
  int counter = 1;
  byte temp[4];
  while(counter <= EEPROM.length()){
    for(int x = 0; x < 4; x++){
      EEPROM.get(counter, temp[x]);
      counter++;
    }
    if(conf[0]==temp[0] && conf[1]==temp[1] && conf[2]==temp[2] && conf[3]==temp[3]){
      checker = 1;
      break;
    }
    else{
      checker = 0;
    }
    counter++;
  }
  return checker;
}
  

void showUserId(){
  int cardReader[4];
  String strID;
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial()) { 
    return 0;
  }
  indicator('r',1);
  buzzer('s',1);
  for(int i = 0; i < 4; i++){
    cardReader[i] = rfid.uid.uidByte[i];
  }
  
  lcd.setCursor(0,0);
  lcd.print("CARD NUMBER :");
  lcd.setCursor(0,1);
  for(int i = 0; i < 4; i++){
    Serial.println(cardReader[i]);  
  }
  Serial.println();
  
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1000);
  indicator('r',0);
  //return cardReader;
}





void buzzer(char leength, int num){
  if(leength == 'l'){
    for(int i = 0; i < num; i++){
      digitalWrite(buz, HIGH);
      delay(1000);
      digitalWrite(buz, LOW);
      delay(200);
    }
  }
  else if(leength == 's'){
    for(int i = 0; i < num; i++){
      digitalWrite(buz, HIGH);
      delay(300);
      digitalWrite(buz, LOW);
      delay(300);
    }
  }
}
void indicator(char led, int val){
    if(led == 'r'){
      if(val == 1){
        digitalWrite(redLed, HIGH);
      }
      else{
        digitalWrite(redLed, LOW);
      }
    }
    else if(led == 'g'){
      if(val == 1){
        digitalWrite(greenLed, HIGH);
      }
      else{
        digitalWrite(greenLed, LOW);
      }
    }
    else if(led == 'b'){
      if(val == 1){
        digitalWrite(blueLed, HIGH);
      }
      else{
        digitalWrite(blueLed, LOW);
      }
    }
}

void getCardId(){
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial()) { 
    lcd.clear();
    lcd.print("SHOW KEY CARD");
    return 0;
  }
  indicator('b',1);
  for(int i = 0; i < 4; i++){
    cardId[i] = rfid.uid.uidByte[i];
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  buzzer('s',1);
  delay(500);
  indicator('b',0);
  
}
void saveCard(byte cardVal[], int addr){
  for(int k = 0; k < 4; k++){
    byte s;
    EEPROM.write(addr, cardVal[k]);
    addr++;
  }
  okay = false;
  EEPROM.get(0, users);
  users++;
  EEPROM.write(0, users);
}

void starter(){
  getCardId();
  if(cardId[0] != 0 && cardId[1] != 0){
    int x = validate(cardId);
    if(x == 1){
      lcd.clear();
      indicator('g',1);
      lcd.print("CARD FOUND");
      delay(100);
      indicator('g',0);
      for(int r = 0; r < 4; r++){
        cardId[r] = 0;
      }
    }
    else{
      lcd.clear();
      indicator('r',1);
      lcd.print("UNIDENTIFIED");
      delay(1000);
      indicator('r',0);
      for(int r = 0; r < 4; r++){
        cardId[r] = 0;
      }
    }
  }
}


void setup() {
  Serial.begin(9600);
  SPI.begin();
  lcd.begin(16,2);
  
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(buz, OUTPUT);
  pinMode(button, INPUT);
  pinMode(button2, INPUT);
  
  rfid.PCD_Init();
  byte x;
  EEPROM.get(0,x);
  Serial.print("Number of users: ");
  Serial.println(x);
  printMemory();
}

void printMemory(){
  int counter = 1; byte holder[4]; int z = 1;
  while(counter <= EEPROM.length()){
    for(int i = 0; i < 4; i++){
      EEPROM.get(counter, holder[i]);
      counter++;  
    }
    Serial.print(z);
    Serial.print(" : ");
    for(byte sender:holder){
      Serial.print(sender);
    } 
    Serial.println();
    counter++; z++;
  } 
}

void loop() {
  indicator('b',1);
  delay(200);
  indicator('b',0);
  delay(200);
  EEPROM.get(0,users);
  lcd.clear();
  lcd.print("SHOW KEY CARD");
  delay(200);
  if(digitalRead(button) == HIGH){
    cardAdder();
    okay = true;
    byte x;
    EEPROM.get(0,x);
    Serial.print("Number of users: ");
    Serial.println(x);
  }
  if(digitalRead(button2) == HIGH){
    cardRemover();
    okay = true;
    byte x;
    EEPROM.get(0,x);
    Serial.print("Number of users: ");
    Serial.println(x);
  }
  starter();
}

