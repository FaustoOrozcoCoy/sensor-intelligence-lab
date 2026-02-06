#include "sampler_timer.h"

// Private variables (encapsulated, not visible to main)
static volatile bool _new_data = false;
static volatile uint16_t _last_val = 0;
static volatile uint32_t _sample_count = 0;
static hw_timer_t *_timer = NULL;

// The ISR stays here, hidden from the rest of the world
void IRAM_ATTR onTimer()
{
    _last_val = analogRead(34);
    _new_data = true;
    _sample_count++;
    digitalWrite(2, !digitalRead(2));
}

void init_sampler_timer(uint32_t freq_hz)
{
    pinMode(2, OUTPUT);
    _timer = timerBegin(1000000); // 1MHz base
    timerAttachInterrupt(_timer, &onTimer);
    timerAlarm(_timer, 1000000 / freq_hz, true, 0);
}

void start_timer() { timerStart(_timer); }
bool is_new_sample_ready() { return _new_data; }
uint16_t get_adc_value() { return _last_val; }
void clear_sample_flag() { _new_data = false; }
uint32_t get_sample_count() { return _sample_count; }