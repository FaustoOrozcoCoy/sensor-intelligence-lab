#include <Arduino.h>
#include "opamplab_regs.h"

// Definición de Pines para el Shift Register
#define PIN_DATA  23
#define PIN_CLK   18
#define PIN_LATCH 5

opamplab_config_t current_config;

void send_to_chip(uint16_t data) {
    // Protocolo Shift Register Simple (Tipo 74HC595)
    
    // 1. Bajamos LATCH para empezar (preparamos el chip)
    digitalWrite(PIN_LATCH, LOW);
    delayMicroseconds(1000);

    // 2. Empujamos los 16 bits uno por uno (MSB first o LSB first)
    // Usaremos MSB First (Bit 15 primero)
    for (int i = 15; i >= 0; i--) {
        // Ponemos el bit en DATA
        bool bit_val = (data >> i) & 0x01;
        digitalWrite(PIN_DATA, bit_val);
        
        // Pulso de Reloj (Rising Edge captura el dato)
        digitalWrite(PIN_CLK, LOW);
        delayMicroseconds(1000);
        digitalWrite(PIN_CLK, HIGH);
        delayMicroseconds(1000);
    }

    // 3. Subimos LATCH para transferir los datos a la salida física
    digitalWrite(PIN_LATCH, HIGH);
}

void setup() {
    Serial.begin(115200);
    
    // Configurar pines
    pinMode(PIN_DATA, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_LATCH, OUTPUT);
    digitalWrite(PIN_LATCH, HIGH);

    // Configuración Inicial
    current_config.raw_data = 0;
    
    // Configurar Chip: Enable ON, Bias medio (3), Miller medio (5)
    current_config.fields.enable = 1;
    current_config.fields.bias_adj = 3;
    current_config.fields.miller_cap = 5;

    Serial.printf("Enviando config: 0x%04X\n", current_config.raw_data);
}

void loop() {
    // Nada por ahora
    delay(2000);
    send_to_chip(current_config.raw_data);
}