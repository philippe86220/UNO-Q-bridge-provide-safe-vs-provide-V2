#include <Arduino.h>
#include <Arduino_RouterBridge.h>

void printSlow(const char *s) {
  for (int i = 0; s[i] != '\0'; i++) {
    Monitor.print(s[i]);
    delay(10);
  }
  Monitor.println();
}

void printFromMpu(float value) {
  Monitor.println(value, 2);
}

void setup() {
  Bridge.begin();
  Monitor.begin();

  Bridge.provide("print_value", printFromMpu);
  //Bridge.provide_safe("print_value", printFromMpu);

  delay(5000);
}

void loop() {
  printSlow("Forum");
  delay(100);
}
