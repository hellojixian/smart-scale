#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "hw.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Variables to keep track of button states
bool btnOkPressed = false;
bool btnNextPressed = false;
bool btnPrevPressed = false;
bool btnCancelPressed = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // Debounce time in milliseconds

// Previous button states for polling
bool btnOkLastState = HIGH;
bool btnNextLastState = HIGH;
bool btnPrevLastState = HIGH;
bool btnCancelLastState = HIGH;

// Function prototypes
void checkButtonStates();
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
  display.display();

  // Setup buzzer pin as output
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off at start

  // Setup button pins as inputs with pullups
  pinMode(BTN_OK_PIN, INPUT_PULLUP);
  pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
  pinMode(BTN_PREV_PIN, INPUT_PULLUP);
  pinMode(BTN_CANCEL_PIN, INPUT_PULLUP);

  // Initialize the last button states
  btnOkLastState = digitalRead(BTN_OK_PIN);
  btnNextLastState = digitalRead(BTN_NEXT_PIN);
  btnPrevLastState = digitalRead(BTN_PREV_PIN);
  btnCancelLastState = digitalRead(BTN_CANCEL_PIN);

  // Play startup sound
  playStartupSound();

  Serial.println("System ready. Waiting for button presses (polling method)...");
}

void loop()
{
  // Check button states (polling approach)
  checkButtonStates();

  // Handle button presses
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

  // Small delay to prevent CPU from working too hard but still be responsive
  delay(2);
}

// Enhanced button polling function with optimized debouncing
void checkButtonStates()
{
  // Read current button states (LOW when pressed since connected to GND)
  bool okCurrState = digitalRead(BTN_OK_PIN);
  bool nextCurrState = digitalRead(BTN_NEXT_PIN);
  bool prevCurrState = digitalRead(BTN_PREV_PIN);
  bool cancelCurrState = digitalRead(BTN_CANCEL_PIN);

  // Get current time once for all checks
  unsigned long currentMillis = millis();
  bool timeCheck = (currentMillis - lastDebounceTime) > debounceDelay;

  // OK button - check for HIGH to LOW transition (button press)
  if (okCurrState != btnOkLastState)
  {
    if (timeCheck && okCurrState == LOW)
    {
      btnOkPressed = true;
      Serial.println("OK button pressed (polling)");
      lastDebounceTime = currentMillis;
    }
    btnOkLastState = okCurrState;
  }

  // NEXT button
  if (nextCurrState != btnNextLastState)
  {
    if (timeCheck && nextCurrState == LOW)
    {
      btnNextPressed = true;
      Serial.println("NEXT button pressed (polling)");
      lastDebounceTime = currentMillis;
    }
    btnNextLastState = nextCurrState;
  }

  // PREV button
  if (prevCurrState != btnPrevLastState)
  {
    if (timeCheck && prevCurrState == LOW)
    {
      btnPrevPressed = true;
      Serial.println("PREV button pressed (polling)");
      lastDebounceTime = currentMillis;
    }
    btnPrevLastState = prevCurrState;
  }

  // CANCEL button
  if (cancelCurrState != btnCancelLastState)
  {
    if (timeCheck && cancelCurrState == LOW)
    {
      btnCancelPressed = true;
      Serial.println("CANCEL button pressed (polling)");
      lastDebounceTime = currentMillis;
    }
    btnCancelLastState = cancelCurrState;
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
