#ifndef LFO_H
#define LFO_H

#include "arm_math.h"

#define SAMPLE_RATE 48000
#define MIN_FREQ 1
#define MAX_FREQ

float32_t lfo_phase = 0.0f;
float32_t increment = 0.0f;

float32_t apply_tremolo(float32_t inputSample, float32_t lfo_frequency, float32_t depth) {
    float32_t lfoValue = (arm_sin_f32(lfo_phase) + 1.0f) * 0.5f;  

    increment = (2.0f * PI * lfo_frequency) / SAMPLE_RATE;
    lfo_phase += increment;
    if (lfo_phase >= 2.0f * PI) {
        lfo_phase -= 2.0f * PI;
    }

    float32_t tremolo = (1.0f - depth) + (depth * lfoValue);
    return inputSample * tremolo;
}

void process_tremolo(short* in_buffer, short* out_buffer, float32_t lfo_frequency, float32_t depth, size_t frames) {
    float32_t in_left = 0, in_right = 0;
    float32_t out_left = 0, out_right = 0;
    
    for (size_t i = 0; i < frames; ++i) {
        in_left = in_buffer[2 * i] / 32768.0f;
        in_right = in_buffer[2 * i + 1] / 32768.0f;

        out_left = apply_tremolo(in_left, lfo_frequency, depth);
        out_right = apply_tremolo(in_right, lfo_frequency, depth);

        out_buffer[2 * i] = (short)(out_left * 32768.0f);
        out_buffer[2 * i + 1] = (short)(out_right * 32768.0f);

    }
}



#endif