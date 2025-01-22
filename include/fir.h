#ifndef FIR_H
#define FIR_H

#include "arm_math.h"
#include "pcm.h"

#define NUM_TAPS 32

#define FIR_FAIL -1
#define FIR_SUCCESS 0

typedef struct{
    arm_fir_instance_f32 inst;
    float32_t *raw;
    float32_t *filtered;
}FIR;

int fir_init(FIR *fir, float32_t *state, const float32_t *coeffs);
void fir_process(FIR *left, FIR *right, short *in_buffer, short *out_buffer, size_t frames);
void generate_fir_coeffs(const char *script_file_path, 
                         int num_taps, 
                         unsigned int rate, 
                         const char *filter_type, 
                         const char *window, 
                         float cut_offs[], 
                         int cut_offs_num);

#endif