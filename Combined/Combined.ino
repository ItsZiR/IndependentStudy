#include <Adafruit_Fingerprint.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h> 
#include <Wire.h> 

//---Sensor---
volatile int finger_status = -1;
SoftwareSerial mySerial(2, 3); // TX/RX on fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

//---Keypad---
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {10, 9, 8, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 5, 4}; //connect to the column pinouts of the keypad
//Create an object of keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String password = "114514";
int pwDigit;
String userEnter;

//---LCD---
LiquidCrystal_I2C lcd(0x27,16,2);  //設定LCD位置0x27,設定LCD大小為16*2

void setup()  
{
  //---Sensor---
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");

  //---Keypad---
  pwDigit = 0;
  userEnter = "";
  Serial.println("I'm ready.");
  Serial.println("Waiting for valid finger or to enter the password...");

  //---LCD---
  //初始化LCD 
  lcd.init(); 
  lcd.backlight(); //開啟背光
  lcd.print("Waiting..."); //只能顯示英數
}

void loop()
{
  char key = keypad.getKey();// Read the key
  finger_status = getFingerprintIDez();

  if(key){
    if(pwDigit == 0){      
      lcd.clear();
    }
    lcd.setCursor(pwDigit, 0);
    Serial.print(key);
    userEnter += key;
    pwDigit ++;
    lcd.print(key);
    if(pwDigit == password.length()){
        lcd.clear();
        lcd.setCursor(0, 0);
      if(userEnter == password){
        Serial.println("\nWelcome!");
        lcd.print("Validated");
        lcd.setCursor(0, 1);
        lcd.print("Welcome!");
      }
      else{
        Serial.println("\nIncorrect Password.");
        lcd.print("Try again!");
      }
      pwDigit = 0;
      userEnter = "";
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Waiting...");
      Serial.print("Please enter the password or fingerprint : ");
    }
  }
  else if(finger_status!=-1 and finger_status!=-2){
    Serial.println("Match");
    if(finger.fingerID == 1){
      Serial.println("Welcome back, Zir!");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hello, Zir!");
    }
    pwDigit = 0;
    userEnter = "";
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting...");
    Serial.print("Please enter the password or fingerprint : ");
  }
  /*
  else{
    if (finger_status==-2){
        if(pwDigit != 0 and userEnter != ""){
          Serial.print("Not Match");
        }
    }
  }
  */
  //delay(50);
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  //if (p!=2){Serial.println(p);}
  if (p != FINGERPRINT_OK)  return -1;
  
  p = finger.image2Tz();
  //if (p!=2){Serial.println(p);}
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -2;
  
  // found a match!
  Serial.print("\nFound ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}