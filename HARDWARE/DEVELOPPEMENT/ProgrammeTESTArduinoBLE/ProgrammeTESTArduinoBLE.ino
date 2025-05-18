#include <Wire.h>
#include <U8g2lib.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define BUTTON_PIN 12

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const unsigned long DURATION_MS = 5UL * 60UL * 1000UL;  // 5 minutes
const unsigned long RESET_HOLD_TIME = 1500;

bool timerRunning = false;
bool buttonPressed = false;
bool longPressActive = false;

unsigned long startTime = 0;
unsigned long buttonDownTime = 0;
unsigned long lastBLESend = 0;

BLECharacteristic* pCharacteristic;
bool deviceConnected = false;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Wire.begin(22, 21);
  display.begin();

  // BLE setup
  BLEDevice::init("MinuteurESP32");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE prêt, en attente de connexion");

  // Affichage de bienvenue
  display.clearBuffer();
  display.setFont(u8g2_font_ncenB14_tr);
  display.drawStr(10, 30, "Minuteur 5 min");
  display.sendBuffer();
  delay(1500);
}

void loop() {
  // Gestion bouton
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      buttonDownTime = millis();
    } else if (!longPressActive && millis() - buttonDownTime >= RESET_HOLD_TIME) {
      // Réinitialisation
      timerRunning = false;
      buttonPressed = true;
      longPressActive = true;
      startTime = 0;

      display.clearBuffer();
      display.setFont(u8g2_font_ncenB14_tr);
      display.drawStr(10, 30, "Reset !");
      display.sendBuffer();
      delay(1000);
    }
  } else {
    if (buttonPressed && !longPressActive) {
      // Appui court
      if (!timerRunning) {
        startTime = millis();
        timerRunning = true;
        Serial.println("Minuteur demarre");
      } else {
        timerRunning = false;
        Serial.println("Minuteur pause");
      }
    }
    buttonPressed = false;
    longPressActive = false;
  }

  // Calcul temps
  unsigned long elapsed = timerRunning ? millis() - startTime : 0;
  if (elapsed > DURATION_MS) elapsed = DURATION_MS;
  unsigned long remaining = DURATION_MS - elapsed;

  unsigned int minutes = remaining / 60000;
  unsigned int seconds = (remaining % 60000) / 1000;

  char timeStr[10];
  sprintf(timeStr, "%02d:%02d", minutes, seconds);

  // Affichage OLED
  display.clearBuffer();
  display.setFont(u8g2_font_ncenB14_tr);
  display.drawStr(5, 22, timeStr);

  // Dessin goutte
  const int dropX = 100;
  const int dropY = 10;
  const int dropW = 20;
  const int dropH = 50;
  display.drawRFrame(dropX, dropY, dropW, dropH, 10);

  float fillRatio = (float)(DURATION_MS - remaining) / DURATION_MS;
  int filledHeight = dropH * (1.0 - fillRatio);
  display.setDrawColor(1);
  display.drawBox(dropX + 1, dropY + filledHeight + 1, dropW - 2, dropH - filledHeight - 2);

  // Bulles
  if (timerRunning) {
    int t = millis() / 300;
    int bubbleY = dropY + dropH - (t % (dropH - 6)) - 4;
    int bubbleX = dropX + 6 + (t % 2) * 5;
    if (bubbleY > dropY + 2)
      display.drawCircle(bubbleX, bubbleY, 2, U8G2_DRAW_ALL);
  }

  display.sendBuffer();

  // Envoi BLE chaque seconde
  if (deviceConnected && millis() - lastBLESend >= 1000) {
    char msg[32];
    if (timerRunning)
      sprintf(msg, "Temps restant: %s", timeStr);
    else
      sprintf(msg, "Pause: %s", timeStr);

    pCharacteristic->setValue(msg);
    pCharacteristic->notify();
    Serial.print("Envoyé BLE: ");
    Serial.println(msg);
    lastBLESend = millis();
  }
}
