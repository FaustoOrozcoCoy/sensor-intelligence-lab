#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h" // Necesario para semáforos

// Usamos un Semáforo Binario, NO un Mutex
SemaphoreHandle_t syncSemaphore;

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;

// Tarea
void blinkLED(void *parameters) {

  // 1. COPIAR EL DATO (Critical)
  // Leemos la memoria del Stack de 'setup' antes de que desaparezca
  int num = *(int *)parameters;

  // 2. SEÑALIZAR (Signaling)
  // Le decimos a setup: "¡Ya lo tengo! Puedes continuar".
  xSemaphoreGive(syncSemaphore);

  Serial.print("Received in Task: ");
  Serial.println(num);

  pinMode(led_pin, OUTPUT);

  while (1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(num));
    digitalWrite(led_pin, LOW);
    vTaskDelay(pdMS_TO_TICKS(num));
  }
}

void setup() {
  long int delay_arg;

  Serial.begin(115200);
  vTaskDelay(pdMS_TO_TICKS(1000));
  
  Serial.println("--- FreeRTOS Binary Semaphore Challenge ---");
  Serial.println("Enter delay (ms):");

  while (Serial.available() <= 0);
  delay_arg = Serial.parseInt();
  
  Serial.print("Sending from Setup: ");
  Serial.println(delay_arg);

  // 1. CREAR SEMÁFORO BINARIO
  // A diferencia del Mutex, este inicia "VACÍO" (0).
  // Nadie puede pasar hasta que alguien haga "Give".
  syncSemaphore = xSemaphoreCreateBinary();

  // 2. Crear Tarea (pasando puntero al stack local de setup)
  xTaskCreatePinnedToCore(blinkLED, "Blink", 1024, (void *)&delay_arg, 1, NULL, app_cpu);

  // 3. BLOQUEAR SETUP (WAIT FOR SIGNAL)
  // Intentamos tomar el semáforo. Como está vacío, nos bloqueamos aquí.
  // El CPU se va a ejecutar la tarea blinkLED.
  Serial.println("Setup: Esperando a que la tarea lea el dato...");
  
  xSemaphoreTake(syncSemaphore, portMAX_DELAY);
  
  // 4. CONTINUAR
  Serial.println("Setup: La tarea ya leyó. Puedo terminar.");
  Serial.println("Done!");
  
  // Aquí termina setup(). 'delay_arg' se destruye, pero ya no importa.
}

void loop() {
  vTaskDelete(NULL);
}