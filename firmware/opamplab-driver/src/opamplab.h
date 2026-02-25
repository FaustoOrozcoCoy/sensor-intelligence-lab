/**
 * @file opamplab.h
 * @author Fausto Orozco (forozco@uniquindio.edu.co)
 * @brief Hardware Abstraction Layer (HAL) for OpAmpLab130 ASIC.
 * @version 1.0
 * @date 2026-02-24
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef OPAMPLAB_H
#define OPAMPLAB_H

#include <stdint.h>
#include "opamplab_regs.h" // Archivo de registros

/**
 * @brief Driver Context Structure.
 * Holds the pin configuration and current state of the chip.
 */

// Estructura para manejar el Chip
typedef struct
{
    uint8_t pin_data;
    uint8_t pin_clk;
    uint8_t pin_latch;
    opamplab_config_t config; // Shadow register (copia local del estado)
} OpAmpLab_t;

/**
 * @brief Initializes the OpAmpLab driver and GPIOs.
 *
 * Configures the pins as outputs and resets the chip to default state (0x0000).
 *
 * @param dev Pointer to the driver handle structure.
 * @param data_pin ESP32 GPIO number for Serial Data.
 * @param clk_pin ESP32 GPIO number for Serial Clock.
 * @param latch_pin ESP32 GPIO number for Latch/Load.
 */

// --- API Pública ---
// Inicializa los pines y el estado por defecto
void OpAmpLab_Init(OpAmpLab_t *dev, uint8_t data_pin, uint8_t clk_pin, uint8_t latch_pin);

// Funciones de configuración (Setters)
/**
 * @brief Allow to power down or enable the OpAmpLab chip.
 *
 * @param dev Pointer to the driver handle.
 * @param enable If true, powers up the chip. If false, powers down the chip.
 */

void OpAmpLab_SetPower(OpAmpLab_t *dev, bool enable);

/**
 * @brief Sets the High Speed Mode of the OpAmpLab chip.
 *
 * @param dev Pointer to the driver handle.
 * @param high_speed If true, enables high speed mode. If false, disables high speed mode.
 */
void OpAmpLab_SetMode(OpAmpLab_t *dev, bool high_speed);

/**
 * @brief Sets the Bias Current level.
 *
 * Adjusts the tail current of the differential pair.
 *
 * @param dev Pointer to the driver handle.
 * @param bias_level Current level (0 = Min, 7 = Max). Values > 7 will be clamped.
 */
void OpAmpLab_SetBias(OpAmpLab_t *dev, uint8_t bias_level); // 0-7

/**
 * @brief Sets the Miller Compensation Capacitance level.
 *
 * Adjusts the compensation capacitance for stability and bandwidth control.
 *
 * @param dev Pointer to the driver handle.
 * @param cap_level Capacitance level (0 = Min, 15 = Max). Values > 15 will be clamped.
 */
void OpAmpLab_SetMiller(OpAmpLab_t *dev, uint8_t cap_level); // 0-15

// Función que habla con el hardware (Commit)
/**
 * @brief Commits the current configuration to the OpAmpLab chip.
 *
 * This function sends the 16-bit configuration word over the serial interface
 * to update the chip's settings. It should be called after any change in the configuration.
 *
 * @param dev Pointer to the driver handle.
 */
void OpAmpLab_Update(OpAmpLab_t *dev);

#endif // OPAMPLAB_H