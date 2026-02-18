#include <Arduino.h>
#include "opamplab.h"

// Instancia del driver
OpAmpLab_t myChip;

void setup()
{
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
}

// Función para parsear y ejecutar comandos
void process_command(String cmd)
{
  cmd.trim(); // Quitar espacios extra

  // Comando: set_bias <val>
  if (cmd.startsWith("set_bias"))
  {
    int val = cmd.substring(9).toInt();
    OpAmpLab_SetBias(&myChip, val);
    OpAmpLab_Update(&myChip);
    Serial.printf("Bias actualizado a: %d\n", val);
  }
  // Comando: set_miller <val>
  else if (cmd.startsWith("set_miller"))
  {
    int val = cmd.substring(11).toInt();
    OpAmpLab_SetMiller(&myChip, val);
    OpAmpLab_Update(&myChip);
    Serial.printf("Miller Cap actualizado a: %d\n", val);
  }
  // Comando: enable <0/1>
  else if (cmd.startsWith("enable"))
  {
    int val = cmd.substring(7).toInt();
    OpAmpLab_SetPower(&myChip, (bool)val);
    OpAmpLab_Update(&myChip);
    Serial.printf("Power: %s\n", val ? "ON" : "OFF");
  }
  // Comando: status
  else if (cmd.startsWith("status"))
  {
    Serial.println("--- OpAmpLab Status ---");
    Serial.printf("Raw Hex: 0x%04X\n", myChip.config.raw_data);
    Serial.printf("Enable: %d\n", myChip.config.fields.enable);
    Serial.printf("Bias:   %d\n", myChip.config.fields.bias_adj);
    Serial.printf("Miller: %d\n", myChip.config.fields.miller_cap);
    Serial.println("-----------------------");
  }
  else
  {
    Serial.println("Comando desconocido. Usa: set_bias, set_miller, enable, status");
  }
}

void loop()
{
  if (Serial.available())
  {
    String input = Serial.readStringUntil('\n');
    process_command(input);
  }

  // Pequeño delay para estabilidad del RTOS (si lo usaras)
  vTaskDelay(10);
}

