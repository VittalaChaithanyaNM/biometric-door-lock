#include <Adafruit_Fingerprint.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo setup
Servo lockServo;
int lockPin = 9;

// Fingerprint setup
SoftwareSerial fingerSerial(2, 3); // RX, TX
Adafruit_Fingerprint finger(&fingerSerial);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {4, 5, 6, 7};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Passkey
const String masterPass = "1234";

void setup() {
  lcd.begin();
  lcd.backlight();

  Serial.begin(9600);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    lcd.print("Fingerprint ready");
  } else {
    lcd.print("Sensor error");
    while (1);
  }

  lockServo.attach(lockPin);
  lockServo.write(0); // Locked
  delay(2000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("1.FP 2.Passkey");
  lcd.setCursor(0, 1);
  lcd.print("3.Enroll");

  char option = keypad.getKey();
  if (option == '1') {
    handleFingerprint();
  } else if (option == '2') {
    handlePasskey();
  } else if (option == '3') {
    enrollFingerprint();
  }
}

void handlePasskey() {
  lcd.clear();
  lcd.print("Enter Passkey:");
  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') break;
      input += key;
      lcd.setCursor(0, 1);
      lcd.print(input);
    }
  }
  if (input == masterPass) {
    unlockDoor("Passkey");
  } else {
    lcd.clear();
    lcd.print("Wrong passkey");
    delay(2000);
    lcd.clear();
  }
}

void handleFingerprint() {
  lcd.clear();
  lcd.print("Place finger...");
  int id = getFingerprintID();
  if (id >= 0) {
    unlockDoor("Fingerprint");
  } else {
    lcd.clear();
    lcd.print("Access Denied");
    delay(2000);
    lcd.clear();
  }
}

int getFingerprintID() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  return finger.fingerID;
}

void enrollFingerprint() {
  lcd.clear();
  lcd.print("Enter ID (0-127):");
  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') break;
      input += key;
      lcd.setCursor(0, 1);
      lcd.print(input);
    }
  }

  int id = input.toInt();
  if (id < 0 || id > 127) return;

  lcd.clear();
  lcd.print("Enrolling...");
  finger.enrollStart(id);
  int p = -1;
  lcd.setCursor(0, 1);
  lcd.print("Place finger");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  finger.image2Tz(1);
  lcd.clear();
  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  lcd.clear();
  lcd.print("Place again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  finger.image2Tz(2);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    finger.storeModel(id);
    lcd.clear();
    lcd.print("Enrolled ID:");
    lcd.print(id);
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("Enroll failed");
    delay(2000);
  }
}

void unlockDoor(String method) {
  lcd.clear();
  lcd.print("Unlocked via");
  lcd.setCursor(0, 1);
  lcd.print(method);
  Serial.println("ACCESS_GRANTED:" + method);
  lockServo.write(90); // Unlock
  delay(5000);
  lockServo.write(0); // Lock
  lcd.clear();
}
