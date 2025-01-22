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

void generate_fir_coeffs(const char *script_file_path, 
                         int num_taps, 
                         unsigned int rate, 
                         const char *filter_type, 
                         const char *window, 
                         float cut_offs[],
                         int cut_off_num){
    
    //command sting
    char cmd_str[100];
    
    //place cut offs in str
    char cut_off_str[50];
    
    if(cut_off_num == 2)
        snprintf(cut_off_str, sizeof(cut_off_str), "%.2f %.2f",cut_offs[0], cut_offs[1]);
    else
         snprintf(cut_off_str, sizeof(cut_off_str), "%.2f",cut_offs[0]);

    snprintf(cmd_str, sizeof(cmd_str), "python %s %d %d %s %s %s", script_file_path, num_taps, rate, filter_type, window, cut_off_str);
    printf("cmd: %s\n", cmd_str);
    
    // Open a pipe to the command
    FILE *pipe = popen(cmd_str, "r");
   

    // Read the output from the Python command
    char buffer[1024];
    printf("Python output:\n");
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        printf("%s", buffer);
    }

    // Close the pipe
    pclose(pipe);



}