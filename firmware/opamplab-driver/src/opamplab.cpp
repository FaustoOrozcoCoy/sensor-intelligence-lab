#include <Arduino.h>
#include "opamplab.h"

// Función privada (interna) para enviar los bits
static void _send_bits(OpAmpLab_t *dev) {
    uint16_t data = dev->config.raw_data;

    // 1. Latch abajo
    digitalWrite(dev->pin_latch, LOW);
    delayMicroseconds(1);

    // 2. Shift out (MSB First)
    for (int i = 15; i >= 0; i--) {
        bool bit_val = (data >> i) & 0x01;
        digitalWrite(dev->pin_data, bit_val);
        
        // Clock pulse
        digitalWrite(dev->pin_clk, LOW);
        delayMicroseconds(1);
        digitalWrite(dev->pin_clk, HIGH);
        delayMicroseconds(1);
    }

    // 3. Latch arriba (Transfer)
    digitalWrite(dev->pin_latch, LOW);
    digitalWrite(dev->pin_latch, HIGH);
}

// --- Implementación de la API ---

void OpAmpLab_Init(OpAmpLab_t *dev, uint8_t data_pin, uint8_t clk_pin, uint8_t latch_pin) {
    // Guardar configuración de pines
    dev->pin_data = data_pin;
    dev->pin_clk = clk_pin;
    dev->pin_latch = latch_pin;
    
    // Configurar GPIOs
    pinMode(dev->pin_data, OUTPUT);
    pinMode(dev->pin_clk, OUTPUT);
    pinMode(dev->pin_latch, OUTPUT);

    // Estado inicial limpio
    dev->config.raw_data = 0;
    
    // Aplicar reset físico
    digitalWrite(dev->pin_latch, HIGH);
    _send_bits(dev);
}

void OpAmpLab_SetPower(OpAmpLab_t *dev, bool enable) {
    dev->config.fields.enable = enable;
}

void OpAmpLab_SetMode(OpAmpLab_t *dev, bool high_speed) {
    dev->config.fields.mode = high_speed;
}

void OpAmpLab_SetBias(OpAmpLab_t *dev, uint8_t bias_level) {
    // Protección básica (clamping)
    if (bias_level > 7) bias_level = 7;
    dev->config.fields.bias_adj = bias_level;
}

void OpAmpLab_SetMiller(OpAmpLab_t *dev, uint8_t cap_level) {
    if (cap_level > 15) cap_level = 15;
    dev->config.fields.miller_cap = cap_level;
}

void OpAmpLab_Update(OpAmpLab_t *dev) {
    _send_bits(dev);
}