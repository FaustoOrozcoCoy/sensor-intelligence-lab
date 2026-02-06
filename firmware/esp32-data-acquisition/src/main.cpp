#include <Arduino.h>
#include "drivers/sampler_timer.h"

void setup()
{
    Serial.begin(921600);
    init_sampler_timer(1000); // 1kHz
}

void loop()
{
    if (is_new_sample_ready())
    {
        Serial.printf("%lu,%u\n", get_sample_count(), get_adc_value());
        clear_sample_flag();
    }
}