#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "hw.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Variables to keep track of button states
volatile bool btnOkPressed = false;
volatile bool btnNextPressed = false;
volatile bool btnPrevPressed = false;
volatile bool btnCancelPressed = false;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200; // Debounce time in milliseconds

// Function prototypes
void btnOkISR();
void btnNextISR();
void btnPrevISR();
void btnCancelISR();
void playBuzzerTone();
void playStartupSound();
void displayButtonName(const char *buttonName);

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Smart Scale Starting...");

  // Initialize I2C
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Clear the display buffer
  display.clearDisplay();

  // Display "Hello world"
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 2);
  display.println(F("Hello world"));
  display.setTextSize(2);
  display.setCursor(10, 16);
  display.println(F("LoadScale"));

  // draw a horizontal line at 32 pixels from the top
  // display.drawFastHLine(0, 0, SCREEN_WIDTH, SSD1306_WHITE);
  // display.drawFastHLine(0, 31, SCREEN_WIDTH, SSD1306_WHITE);
  display.display();

  // Setup buzzer pin as output
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off at start

  // Setup button pins and attach interrupts
  pinMode(BTN_OK_PIN, INPUT_PULLUP);
  pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
  pinMode(BTN_PREV_PIN, INPUT_PULLUP);
  pinMode(BTN_CANCEL_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BTN_OK_PIN), btnOkISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_NEXT_PIN), btnNextISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_PREV_PIN), btnPrevISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_CANCEL_PIN), btnCancelISR, FALLING);

  // Play startup sound
  playStartupSound();

  Serial.println("System ready. Waiting for button presses...");
}

void loop()
{
  // Check for button presses and update display
  if (btnOkPressed)
  {
    displayButtonName("OK");
    playBuzzerTone();
    btnOkPressed = false;
  }

  if (btnNextPressed)
  {
    displayButtonName("NEXT");
    playBuzzerTone();
    btnNextPressed = false;
  }

  if (btnPrevPressed)
  {
    displayButtonName("PREV");
    playBuzzerTone();
    btnPrevPressed = false;
  }

  if (btnCancelPressed)
  {
    displayButtonName("CANCEL");
    playBuzzerTone();
    btnCancelPressed = false;
  }

  // Small delay to prevent CPU from working too hard
  delay(10);
}

// Button interrupt handlers
void btnOkISR()
{
  // Simple debounce check
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    btnOkPressed = true;
    lastDebounceTime = millis();
  }
}

void btnNextISR()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    btnNextPressed = true;
    lastDebounceTime = millis();
  }
}

void btnPrevISR()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    btnPrevPressed = true;
    lastDebounceTime = millis();
  }
}

void btnCancelISR()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    btnCancelPressed = true;
    lastDebounceTime = millis();
  }
}

// Play a short beep on the buzzer
void playBuzzerTone()
{
  // Generate a 1kHz tone for 100ms
  for (int i = 0; i < 100; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(500); // 1kHz tone
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(500);
  }
}

// Play a startup melody on the buzzer
void playStartupSound()
{
  // Play a short ascending tone sequence
  // First tone: 440Hz (A4) for 100ms
  for (int i = 0; i < 50; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(1136); // ~440Hz
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(1136);
  }
  delay(30); // Short pause between notes

  // Second tone: 523Hz (C5) for 100ms
  for (int i = 0; i < 50; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(955); // ~523Hz
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(955);
  }
  delay(30); // Short pause between notes

  // Third tone: 659Hz (E5) for 100ms
  for (int i = 0; i < 50; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(759); // ~659Hz
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(759);
  }
  delay(30); // Short pause between notes

  // Fourth tone: 880Hz (A5) for 150ms
  for (int i = 0; i < 75; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(568); // ~880Hz
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(568);
  }
}

// Display button name on OLED
void displayButtonName(const char *buttonName)
{
  Serial.print("Button pressed: ");
  Serial.println(buttonName);

  // Clear display and show button name
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Button:"));
  display.println(buttonName);
  display.display();
}
