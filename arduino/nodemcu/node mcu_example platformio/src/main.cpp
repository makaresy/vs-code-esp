#include <Arduino.h>

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  // put your setup code here, to run once:
}

void loop() {

  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
  delay(100);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(200);  
  // put your main code here, to run repeatedly:
}