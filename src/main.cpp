#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  int result = myFunction(2, 3);
  Serial.print("The result of myFunction(2, 3) is: ");
  Serial.println(result);
  Serial.println("Hello, world!");
  delay(1000); // Wait for 1 second
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}