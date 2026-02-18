#ifndef OPAMPLAB_REGS_H
#define OPAMPLAB_REGS_H

#include <stdint.h>

/**
 * @brief OpAmpLab130 Register Map
 * Total Shift Register Length: 16 bits
 * 
 * Bit [0]:     ENABLE (Power On/Off)
 * Bit [1]:     MODE (0=Low Noise, 1=High Speed)
 * Bits [4:2]:  BIAS_CURRENT (0=1uA ... 7=100uA)
 * Bits [8:5]:  MILLER_CAP (0=0pF ... 15=10pF)
 * Bits [15:9]: RESERVED
 */

typedef union {
    struct {
        uint16_t enable      : 1; // LSB
        uint16_t mode        : 1;
        uint16_t bias_adj    : 3;
        uint16_t miller_cap  : 4;
        uint16_t reserved    : 7; // Padding hasta 16 bits
    } fields;
    uint16_t raw_data;            // Acceso al word completo para enviar por Serial
} opamplab_config_t;

// Valores por defecto
#define OPAMPLAB_DEFAULT_CONFIG  0x0000 // Todo apagado

#endif // OPAMPLAB_REGS_H