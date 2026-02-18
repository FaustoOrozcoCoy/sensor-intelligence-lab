#ifndef OPAMPLAB_H
#define OPAMPLAB_H

#include <stdint.h>
#include "opamplab_regs.h" // Archivo de registros

// Estructura para manejar el "Chip"
typedef struct {
    uint8_t pin_data;
    uint8_t pin_clk;
    uint8_t pin_latch;
    opamplab_config_t config; // Shadow register (copia local del estado)
} OpAmpLab_t;

// --- API Pública ---

// Inicializa los pines y el estado por defecto
void OpAmpLab_Init(OpAmpLab_t *dev, uint8_t data_pin, uint8_t clk_pin, uint8_t latch_pin);

// Funciones de configuración (Setters)
void OpAmpLab_SetPower(OpAmpLab_t *dev, bool enable);
void OpAmpLab_SetMode(OpAmpLab_t *dev, bool high_speed);
void OpAmpLab_SetBias(OpAmpLab_t *dev, uint8_t bias_level); // 0-7
void OpAmpLab_SetMiller(OpAmpLab_t *dev, uint8_t cap_level); // 0-15

// Función que habla con el hardware (Commit)
void OpAmpLab_Update(OpAmpLab_t *dev);

#endif // OPAMPLAB_H