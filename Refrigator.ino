#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// RFID setup
#define RST_PIN    5
#define SS_PIN     4
#define RELAY_PIN  2

// LEDs and Buzzer
#define RED_LED   12
#define BUZZER    15

MFRC522 rfid(SS_PIN, RST_PIN);

// LCD setup (I2C 16x2, SDA=21, SCL=22)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT22 setup
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Authorized RFID cards
byte authorizedUIDs[][4] = {
  {0x62, 0x50, 0x2C, 0x3C},
  {0x53, 0x24, 0x20, 0x14}
};

// Fan control pins
const int fan1_pwm = 27;
const int fan2_pwm = 26;
const int fan1_in1 = 25;
const int fan1_in2 = 33;
const int fan2_in1 = 32;
const int fan2_in2 = 14;
const int pot_pin = 34;

bool fans_enabled = false;

void setup() {
  Serial.begin(115200);
  SPI.begin(18, 19, 23, SS_PIN);  // SCK, MISO, MOSI, SS
  rfid.PCD_Init();
  dht.begin();

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RFID Fan System");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Door locked initially

  // Fan motor pins
  pinMode(fan1_pwm, OUTPUT);
  pinMode(fan2_pwm, OUTPUT);
  pinMode(fan1_in1, OUTPUT);
  pinMode(fan1_in2, OUTPUT);
  pinMode(fan2_in1, OUTPUT);
  pinMode(fan2_in2, OUTPUT);

  // Set fan direction
  digitalWrite(fan1_in1, HIGH);
  digitalWrite(fan1_in2, LOW);
  digitalWrite(fan2_in1, HIGH);
  digitalWrite(fan2_in2, LOW);

  // LED and Buzzer pins
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Initial states
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  Serial.println("ESP32 RFID & Fan Control Ready");
}

void loop() {
  handleRFID();
  checkTemperature();
  handleFanSpeed();
}

void handleRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  if (checkUID(rfid.uid.uidByte)) {
    Serial.println("Access Granted");
    lcd.setCursor(0, 0);
    lcd.print("Access: Granted ");

    // Quick beep for granted access
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);

    digitalWrite(RELAY_PIN, LOW);  // Unlock door
    delay(3000);
    digitalWrite(RELAY_PIN, HIGH); // Lock again
  } else {
    Serial.println("Access Denied");
    lcd.setCursor(0, 0);
    lcd.print("Access: Denied  ");

    // Red LED + buzzer on for 2 seconds
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
    delay(200);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
    delay(200);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
  }
  lcd.setCursor(0, 0);
  lcd.print("Put Your ID     ");
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

bool checkUID(byte *uid) {
  for (int i = 0; i < sizeof(authorizedUIDs) / 4; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (uid[j] != authorizedUIDs[i][j]) {
        match = false;
        break;
      }
    }
    if (match) return true;
  }
  return false;
}

void checkTemperature() {
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("Failed to read from DHT22");
    return;
  }

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temp, 1);
  lcd.print("C  ");
}

void handleFanSpeed() {
  int pot_value = analogRead(pot_pin); // 0 to 4095
  int pot_percent = map(pot_value, 0, 1023, 0, 100); // 0 to 100 scale
  int fan_speed = map(pot_value, 0, 1023, 0, 255);   // PWM speed

  Serial.println(fan_speed);

  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("DHT22 read error.");
    return;
  }

  if (pot_percent >= 20) {
    // Manual mode
    analogWrite(fan1_pwm, fan_speed);
    analogWrite(fan2_pwm, fan_speed);
  } else {
    // Auto temperature mode
    if (temp > 35) {
      analogWrite(fan1_pwm, 255); // Full speed
      analogWrite(fan2_pwm, 255);
    } else {
      analogWrite(fan1_pwm, 0);
      analogWrite(fan2_pwm, 0);
    }
  }
}
