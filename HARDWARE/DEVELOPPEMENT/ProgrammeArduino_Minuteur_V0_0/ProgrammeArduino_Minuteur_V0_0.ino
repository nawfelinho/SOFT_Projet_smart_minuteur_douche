#include <Wire.h>
#include <U8g2lib.h>

#define BUTTON_PIN 05

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const unsigned long DURATION_MS = 5UL * 60UL * 1000UL;  // 5 minutes
const unsigned long RESET_HOLD_TIME = 1500;  // Appui long = 1.5 sec

bool timerRunning = false;
bool buttonPressed = false;
bool longPressActive = false;

unsigned long startTime = 0;
unsigned long buttonDownTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Wire.begin(22, 21);
  display.begin();

  // Affichage de bienvenue
  display.clearBuffer();
  display.setFont(u8g2_font_ncenB14_tr);
  display.drawStr(10, 30, "Minuteur 5 min");
  display.sendBuffer();
  delay(1500);
}

void loop() {
  // Gérer appui long
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      buttonDownTime = millis();
    } else {
      // Appui maintenu
      if (!longPressActive && millis() - buttonDownTime >= RESET_HOLD_TIME) {
        // RESET !
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
    }
  } else {
    if (buttonPressed && !longPressActive) {
      // Appui court → toggle du minuteur
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

  // Minuteur actif
  unsigned long elapsed = timerRunning ? millis() - startTime : 0;
  if (elapsed > DURATION_MS) elapsed = DURATION_MS;
  unsigned long remaining = DURATION_MS - elapsed;

  unsigned int minutes = remaining / 60000;
  unsigned int seconds = (remaining % 60000) / 1000;

  char timeStr[10];
  sprintf(timeStr, "%02d:%02d", minutes, seconds);

  // Affichage principal
  display.clearBuffer();

  // Texte du temps
  display.setFont(u8g2_font_ncenB14_tr);
  display.drawStr(5, 22, timeStr);

  // Dessin de la goutte stylée
  const int dropX = 100;
  const int dropY = 10;
  const int dropW = 20;
  const int dropH = 50;

  // Forme arrondie de la goutte (cercle haut + rectangle)
  display.drawRFrame(dropX, dropY, dropW, dropH, 10);

  // Calcul du remplissage
  float fillRatio = (float)(DURATION_MS - remaining) / DURATION_MS;
  int filledHeight = dropH * (1.0 - fillRatio);

  // Remplissage de la goutte
  display.setDrawColor(1);
  display.drawBox(dropX + 1, dropY + filledHeight + 1, dropW - 2, dropH - filledHeight - 2);

  // Animation de bulles qui montent dans la goutte
  if (timerRunning) {
    int t = millis() / 300;  // Changement de bulle chaque 300 ms
    int bubbleY = dropY + dropH - (t % (dropH - 6)) - 4;
    int bubbleX = dropX + 6 + (t % 2) * 5;
    if (bubbleY > dropY + 2)
      display.drawCircle(bubbleX, bubbleY, 2, U8G2_DRAW_ALL);
  }

  display.sendBuffer();
}
