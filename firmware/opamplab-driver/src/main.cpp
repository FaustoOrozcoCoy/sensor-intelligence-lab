#include <Arduino.h>
#include "opamplab.h"

// Instancia del driver
OpAmpLab_t myChip;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Iniciando OpAmpLab System...");

    // Inicializar el chip en los pines definidos
    // DATA=23, CLK=18, LATCH=5
    OpAmpLab_Init(&myChip, 23, 18, 5);

    // Configurar escenario de prueba
    Serial.println("Configurando: High Speed, Bias=3, Miller=10pF");
    
    OpAmpLab_SetPower(&myChip, true);
    OpAmpLab_SetMode(&myChip, true); // High Speed
    OpAmpLab_SetBias(&myChip, 3);
    OpAmpLab_SetMiller(&myChip, 10);

    // Enviar al silicio
    OpAmpLab_Update(&myChip);
    
    Serial.printf("Configuracion enviada (Hex): 0x%04X\n", myChip.config.raw_data);
}

void loop() {
    // Aquí luego pondremos la CLI
    vTaskDelay(1000);
}