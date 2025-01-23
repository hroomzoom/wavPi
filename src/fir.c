#include "fir.h"

int fir_init(FIR *fir, float32_t *state, const float32_t *coeffs){
    
    arm_fir_init_f32(&fir->inst, NUM_TAPS, coeffs, state, PERIOD_SIZE);
    fir->raw = (float32_t *)malloc(PERIOD_SIZE * sizeof(float32_t));
    if(!fir->raw) {
        printf("Failed to allocate memory\n");
        return FIR_FAIL;
    }

    fir->filtered = (float32_t *)malloc(PERIOD_SIZE * sizeof(float32_t));
    if(!fir->filtered) {
        printf("Failed to allocate memory\n");
        return FIR_FAIL;
    }
    return FIR_SUCCESS;
}

void fir_process(FIR *left, FIR *right, short *in_buffer, short *out_buffer, size_t frames){
  
    for (size_t i = 0; i < frames; ++i) {
        left->raw[i] = in_buffer[2 * i] / 32768.0f;
        right->raw[i] = in_buffer[2 * i + 1] / 32768.0f;
    }

    arm_fir_f32(&left->inst, left->raw, left->filtered, frames);
    arm_fir_f32(&right->inst, right->raw, right->filtered, frames);

    for (size_t i = 0; i < frames; ++i) {
        out_buffer[2 * i] = (short)(left->filtered[i] * 32768.0f);
        out_buffer[2 * i + 1] = (short)(right->filtered[i] * 32768.0f);
    }

}