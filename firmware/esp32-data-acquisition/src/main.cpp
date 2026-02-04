#include <Arduino.h>
#include <esp32-hal-timer.h>

// ============================================================================
// CONFIGURACIÓN DEL TIMER (API MODERNA)
// ============================================================================
// Clock base del ESP32: 80 MHz
// Queremos interrupción cada 1ms (1kHz)
//
// Con la API moderna, especificamos directamente la frecuencia en Hz:
// - Frecuencia deseada: 1,000,000 Hz (1 MHz)
// - Esto genera interrupciones cada 1µs
// - Contador para 1ms: 1,000 ciclos @ 1MHz = 1ms
//
// Fórmula:
// - frequency = 80,000,000 Hz / prescaler
// - Para 1MHz: prescaler = 80,000,000 / 1,000,000 = 80
// ============================================================================

// Configuración de pines
#define PIN_DEBUG_TIMER 2   // GPIO2 para medir con osciloscopio
#define PIN_ANALOG_INPUT 34 // GPIO34 (ADC1_CH6) - solo entrada

// Configuración del timer
#define TIMER_FREQUENCY 1000000 // 1 MHz (permite contar cada microsegundo)
#define ALARM_VALUE 1000        // 1000 ciclos @ 1MHz = 1ms (1kHz)

// Variables volátiles para sincronización ISR-Loop
volatile uint32_t timerInterruptCount = 0;
volatile uint16_t lastAnalogValue = 0;
volatile bool newDataAvailable = false;

// Objeto del timer
hw_timer_t *timer = NULL;

// ============================================================================
// RUTINA DE SERVICIO DE INTERRUPCIÓN (ISR)
// ============================================================================
// Ejecutada cada 1ms por el Hardware Timer
// IMPORTANTE: Mantener corta y rápida, sin delays ni funciones bloqueantes
void IRAM_ATTR onTimerInterrupt()
{
    // Invertir pin digital para visualizar en osciloscopio
    // Esto alterna entre HIGH y LOW cada 1ms
    digitalWrite(PIN_DEBUG_TIMER, !digitalRead(PIN_DEBUG_TIMER));

    // Leer valor analógico simulado
    // En aplicación real, aquí iría lectura de sensor
    lastAnalogValue = analogRead(PIN_ANALOG_INPUT);

    // Incrementar contador de interrupciones
    timerInterruptCount++;

    newDataAvailable = true;
}

// ============================================================================
// CONFIGURACIÓN DEL TIMER (API MODERNA)
// ============================================================================
void setupTimer()
{
    // Parámetros API moderna:
    // timer = timerBegin(frequency)
    // frequency: especificar en Hz (ej: 1,000,000 para 1MHz)
    // Retorna: puntero a hw_timer_t si es exitoso, NULL si hay error

    timer = timerBegin(TIMER_FREQUENCY);

    if (timer == NULL)
    {
        Serial.println("ERROR: No se pudo inicializar el timer");
        return;
    }

    // Adjuntar la ISR al timer (API moderna)
    // timerAttachInterrupt(timer, userFunc)
    // No hay parámetros adicionales como en la versión antigua
    timerAttachInterrupt(timer, &onTimerInterrupt);

    // Configurar alarma con auto-reload (API moderna)
    // timerAlarm(timer, alarm_value, autoreload, reload_count)
    // alarm_value: valor de alarma en ciclos del timer
    // autoreload: true para reiniciar automáticamente
    // reload_count: 0 = ilimitado (siempre que autoreload=true)
    timerAlarm(timer, ALARM_VALUE, true, 0);

    // Iniciar el timer (API moderna requiere iniciar explícitamente)
    timerStart(timer);
}

// ============================================================================
// SETUP
// ============================================================================
void setup()
{
    // Inicializar comunicación serial
    Serial.begin(921600);
    delay(1000);

    Serial.println("\n=================================");
    Serial.println("ESP32 Hardware Timer - 1kHz");
    Serial.println("(API Moderna)");
    Serial.println("=================================");

    // Configurar pines
    pinMode(PIN_DEBUG_TIMER, OUTPUT);
    digitalWrite(PIN_DEBUG_TIMER, LOW);

    // Configurar entrada analógica
    pinMode(PIN_ANALOG_INPUT, INPUT);

    // Configurar el Hardware Timer
    setupTimer();

    Serial.println("Timer configurado:");
    Serial.println("  - Frecuencia: 1 MHz");
    Serial.println("  - Valor de alarma: 1000 ciclos");
    Serial.println("  - Período: 1ms (1kHz)");
    Serial.println("  - PIN DEBUG: GPIO2");
    Serial.println("  - PIN ANALOG: GPIO34");
    Serial.println("  - ISR: onTimerInterrupt()");
    Serial.println("=================================\n");
}

// ============================================================================
// LOOP PRINCIPAL
// ============================================================================
void loop()
{
    // Leer datos de manera segura usando volatile
    if (newDataAvailable)
    {
        // Copiar valores volátiles a variables locales
        // para evitar que cambien a mitad de la lectura
        uint32_t interruptCount = timerInterruptCount;
        uint16_t analogValue = lastAnalogValue;

        // Limpiar bandera
        newDataAvailable = false;

        // Mostrar estadísticas cada 100ms
        Serial.print(interruptCount);
        Serial.print(",");
        Serial.println(analogValue);
    }
}

// ============================================================================
// FUNCIONES OPCIONALES DE CONTROL
// ============================================================================
void stopTimer()
{
    if (timer != NULL)
    {
        timerStop(timer);
        timerDetachInterrupt(timer);
        timerEnd(timer);
        timer = NULL;
    }
}

void pauseTimer()
{
    if (timer != NULL)
    {
        timerStop(timer);
    }
}

void resumeTimer()
{
    if (timer != NULL)
    {
        timerStart(timer);
    }
}

/*
 * CAMBIOS PRINCIPALES - API MODERNA:
 *
 * 1. timerBegin(frequency):
 *    - Antiguo: timerBegin(timer_num, prescaler, countUp)
 *    - Moderno: timerBegin(frequency)
 *    - Ahora especificas frecuencia en Hz directamente (1,000,000 = 1MHz)
 *
 * 2. timerAlarm() reemplaza timerAlarmWrite():
 *    - Antiguo: timerAlarmWrite(timer, value, autoreload) + timerAlarmEnable()
 *    - Moderno: timerAlarm(timer, alarm_value, autoreload, reload_count)
 *    - La alarma se habilita automáticamente
 *
 * 3. timerAttachInterrupt() simplificado:
 *    - Antiguo: timerAttachInterrupt(timer, callback, edge_trigger)
 *    - Moderno: timerAttachInterrupt(timer, userFunc)
 *    - Sin parámetro de edge_trigger
 *
 * 4. timerStart() explícito:
 *    - Moderno requiere llamar timerStart() después de configurar
 *
 * 5. Funciones de control nuevas:
 *    - timerStart(): inicia el timer
 *    - timerStop(): pausa el timer
 *    - timerRestart(): reinicia contador a 0
 *    - timerRead(): lee contador actual
 *    - timerReadMicros(), timerReadMillis(), etc.
 *
 * CÁLCULO DE FRECUENCIA:
 * - Clock ESP32: 80 MHz
 * - Frecuencia deseada: 1 MHz
 * - Prescaler = 80 MHz / 1 MHz = 80
 * - Ciclos para 1ms = 1,000,000 Hz × 0.001s = 1,000 ciclos ✓
 */