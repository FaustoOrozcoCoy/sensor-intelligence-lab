#ifndef SAMPLER_TIMER_H
#define SAMPLER_TIMER_H

#include <Arduino.h>

// Public functions that main.cpp will see
void init_sampler_timer(uint32_t freq_hz);
void start_timer();
bool is_new_sample_ready();
uint16_t get_adc_value();
void clear_sample_flag();
uint32_t get_sample_count();

#endif