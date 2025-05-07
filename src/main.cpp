#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "hw.h"
#include <stm32f1xx_hal.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Variables to keep track of button states
bool btnOkPressed = false;
bool btnNextPressed = false;
bool btnPrevPressed = false;
bool btnCancelPressed = false;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // Debounce time in milliseconds

// Battery measurement variables
float batteryVoltage = 0.0;
unsigned long lastBatteryCheckTime = 0;
const unsigned long batteryCheckInterval = 2000; // Check battery every 2 seconds

// Volatile flags for interrupt handlers
volatile bool btnOkFlag = false;
volatile bool btnNextFlag = false;
volatile bool btnPrevFlag = false;
volatile bool btnCancelFlag = false;

// Function prototypes
void playBuzzerTone();
void playStartupSound();
void displayButtonName(const char *buttonName);
float readBatteryVoltage();
void displayBatteryLevel();

// Interrupt handlers for buttons
void btnOkISR();
void btnNextISR();
void btnPrevISR();
void btnCancelISR();
void processButtonFlags();

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Smart Scale Starting...");

  // Set up built-in LED for debugging
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

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

  // Setup button interrupts
  attachInterrupt(digitalPinToInterrupt(BTN_OK_PIN), btnOkISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_NEXT_PIN), btnNextISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_PREV_PIN), btnPrevISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_CANCEL_PIN), btnCancelISR, FALLING);

  // Setup battery measurement pins
  pinMode(VMETER_CTL_PIN, OUTPUT);
  pinMode(VMETER_SIG_PIN, INPUT_ANALOG);
  digitalWrite(VMETER_CTL_PIN, LOW); // Initially disable the MOSFET

  // Play startup sound
  playStartupSound();

  // 设置中断优先级
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0); // 预占优先级0，子优先级0
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);

  Serial.println("System ready. Waiting for button presses (EXTI method)...");
}

void loop()
{
  // Process any button flags set by interrupts
  processButtonFlags();

  // Check if it's time to measure battery voltage
  unsigned long currentMillis = millis();
  if (currentMillis - lastBatteryCheckTime >= batteryCheckInterval)
  {
    // Measure battery voltage
    batteryVoltage = readBatteryVoltage();
    displayBatteryLevel();
    lastBatteryCheckTime = currentMillis;
  }

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

// Interrupt Service Routines for buttons
void btnOkISR()
{
  unsigned long currentMillis = millis();
  if ((currentMillis - lastDebounceTime) > debounceDelay)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED for visual feedback
    btnOkFlag = true;
    lastDebounceTime = currentMillis;
  }
}

void btnNextISR()
{
  unsigned long currentMillis = millis();
  if ((currentMillis - lastDebounceTime) > debounceDelay)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED for visual feedback
    btnNextFlag = true;
    lastDebounceTime = currentMillis;
  }
}

void btnPrevISR()
{
  unsigned long currentMillis = millis();
  if ((currentMillis - lastDebounceTime) > debounceDelay)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED for visual feedback
    btnPrevFlag = true;
    lastDebounceTime = currentMillis;
  }
}

void btnCancelISR()
{
  unsigned long currentMillis = millis();
  if ((currentMillis - lastDebounceTime) > debounceDelay)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED for visual feedback
    btnCancelFlag = true;
    lastDebounceTime = currentMillis;
  }
}

// Process button flags (set by interrupts) in the main loop
void processButtonFlags()
{
  // Process the OK button flag
  if (btnOkFlag)
  {
    btnOkPressed = true;
    btnOkFlag = false;
    Serial.println("OK button pressed (EXTI)");
  }

  // Process the NEXT button flag
  if (btnNextFlag)
  {
    btnNextPressed = true;
    btnNextFlag = false;
    Serial.println("NEXT button pressed (EXTI)");
  }

  // Process the PREV button flag
  if (btnPrevFlag)
  {
    btnPrevPressed = true;
    btnPrevFlag = false;
    Serial.println("PREV button pressed (EXTI)");
  }

  // Process the CANCEL button flag
  if (btnCancelFlag)
  {
    btnCancelPressed = true;
    btnCancelFlag = false;
    Serial.println("CANCEL button pressed (EXTI)");
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

// Read battery voltage using voltage divider (10K/47K)
float readBatteryVoltage()
{
  // Enable the MOSFET to allow voltage reading
  digitalWrite(VMETER_CTL_PIN, HIGH);

  // Small delay to ensure the circuit stabilizes
  delay(100);

  // Read the analog value with oversampling for better accuracy
  long adcSum = 0;
  const int samplesCount = 10;
  for (int i = 0; i < samplesCount; i++)
  {
    adcSum += analogRead(VMETER_SIG_PIN);
    delay(10);
  }
  int adcValue = adcSum / samplesCount;

  // Disable the MOSFET to save power
  digitalWrite(VMETER_CTL_PIN, LOW);

  // Calculate the actual battery voltage
  // For a 12-bit ADC (0-4095), with 3.3V reference
  // Using calibration based on actual measurements:
  float voltage = (adcValue / 4095.0) * 3.3 * 5.05; // 5.5 is the voltage divider ratio (47K + 10K) / 10K

  Serial.print("Battery ADC Value: ");
  Serial.print(adcValue);
  Serial.print(", Raw: ");
  Serial.print(voltage);
  Serial.print("V");
  Serial.println("V");

  return voltage;
}

// Display battery level on OLED
void displayBatteryLevel()
{
  display.clearDisplay();

  // Display battery voltage
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Battery Status:"));

  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(batteryVoltage, 2);
  display.println(F("V"));

  // Draw battery icon
  int batteryWidth = 40;
  int batteryHeight = 20;
  int batteryX = SCREEN_WIDTH - batteryWidth - 5;
  int batteryY = 6;

  // Adjust battery level calculation to use calibrated voltage ranges
  // For a LiPo battery: ~3.0V is empty, ~4.2V is full
  int batteryLevel = map(min(max(int(batteryVoltage * 100), 300), 420), 300, 420, 0, 100);
  int fillWidth = map(batteryLevel, 0, 100, 0, batteryWidth - 4);

  // Draw battery outline
  display.drawRect(batteryX, batteryY, batteryWidth, batteryHeight, SSD1306_WHITE);
  display.drawRect(batteryX + batteryWidth, batteryY + batteryHeight / 4, 4, batteryHeight / 2, SSD1306_WHITE);

  // Fill battery based on level
  if (batteryLevel > 0)
  {
    display.fillRect(batteryX + 2, batteryY + 2, fillWidth, batteryHeight - 4, SSD1306_WHITE);
  }

  // Display percentage
  display.setTextSize(1);
  display.setCursor(batteryX, batteryY + batteryHeight + 2);
  display.print(batteryLevel);
  display.println(F("%"));

  display.display();
}
