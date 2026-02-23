#include <Arduino.h>
#include <unity.h>           // El framework de testing
#include "opamplab.h"        // Tu driver

OpAmpLab_t test_dev;

// 1. SETUP DEL TEST (Se ejecuta antes de cada prueba)
void setUp(void) {
    // Inicializamos la estructura a ceros antes de cada test
    test_dev.config.raw_data = 0;
}

// 2. TEARDOWN (Se ejecuta después de cada prueba)
void tearDown(void) {
    // Limpieza si fuera necesaria
}

// 3. CASO DE PRUEBA: Verificar Bias
// Queremos probar que si ponemos Bias 3, los bits sean correctos.
// Bias 3 en binario es 011. En tu mapa de registros, Bias está en bits [4:2].
// Deberíamos ver: ... 0 0000 1100 ... (El 11 desplazado 2 lugares)
void test_set_bias_logic(void) {
    // Acción
    OpAmpLab_SetBias(&test_dev, 3);
    
    // Verificación (Assert)
    // Bias 3 (011) desplazado 2 bits a la izquierda es 12 (1100 binario = 0xC)
    TEST_ASSERT_EQUAL_HEX16(0x000C, test_dev.config.raw_data);
}

// 4. CASO DE PRUEBA: Verificar Clamping (Protección)
// Si meto Bias 50 (que no existe, max es 7), ¿tu código lo protege?
void test_bias_overflow_protection(void) {
    OpAmpLab_SetBias(&test_dev, 50); // Intentamos romperlo
    
    // Debería limitarlo a 7 (111), desplazado 2 bits -> 28 (0x1C)
    TEST_ASSERT_EQUAL_HEX16(0x001C, test_dev.config.raw_data);
}

// 5. RUNNER (El main del test)
void setup() {
    // Esperar a que el Serial esté listo para que PlatformIO lea el resultado
    delay(2000); 
    
    UNITY_BEGIN(); // Iniciar Unity
    
    RUN_TEST(test_set_bias_logic);
    RUN_TEST(test_bias_overflow_protection);
    
    UNITY_END(); // Terminar
}

void loop() {
    // Nada que hacer
}