#include <Arduino.h>

void setup() {
    // Inicializar serial para debug
    Serial.begin(115200);
}

void loop() {
    Serial.println("System Ready - Fausto S.A.");
    delay(1000);
}