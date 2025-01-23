#include <sndfile.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "pcm.h"
#include "fir.h"

#define NUM_TAPS 32

static float32_t state[PERIOD_SIZE + NUM_TAPS - 1];

const float32_t coeffs[NUM_TAPS] = {
    0.0025, 0.0058, 0.0111, 0.0186, 0.0280, 0.0390, 0.0510, 0.0635, 0.0758,
    0.0872, 0.0972, 0.1052, 0.1106, 0.1132, 0.1128, 0.1096, 0.1034, 0.0946,
    0.0834, 0.0703, 0.0558, 0.0405, 0.0252, 0.0104, -0.0027, -0.0142, -0.0238,
    -0.0314, -0.0370, -0.0406, -0.0422, -0.0418
};

int main(int argc, char **argv){

    //Process args
    if(argc < 2){
        printf("Usage: %s <path/to/wav>\n", argv[0]);
        return EXIT_FAILURE;
    }

    //Open and read wav paramters
    PCM *pcm = (PCM *)malloc(sizeof(PCM));
    if (!pcm) {
        printf("Failed to allocate memory\n");
        return EXIT_FAILURE;
    }
    int status = open_pcm(pcm, argv[1]);
    if(status != PCM_VALID)
        return PCM_INVALID;
    
    //Set up audio output parameters
    printf("setting hw params ...\n");
    status = setup_pcm_hw(pcm);
    if(status != PCM_VALID)
        return PCM_INVALID;
    printf("setting sw params ...\n");
    status = setup_pcm_sw(pcm);
    if(status != PCM_VALID)
        return PCM_INVALID;

    //Set up FIR filters
    printf("initializing fir filters ...\n");
    FIR *left_fir = (FIR *)malloc(sizeof(FIR));
    FIR *right_fir = (FIR *)malloc(sizeof(FIR));
    if(!left_fir || !right_fir){
        printf("Failed to allocate memory\n");
        return EXIT_FAILURE;
    }
    status = fir_init(left_fir, state, coeffs);
    if(status != FIR_SUCCESS)
        return FIR_FAIL;
    status = fir_init(right_fir, state, coeffs);
    if(status != FIR_SUCCESS)
        return FIR_FAIL;
    
    //Set up buffers
    printf("allocating buffers ...\n");
    short *in_buffer = (short *)malloc(PERIOD_SIZE * pcm->channels * sizeof(short));
    short *out_buffer = (short *)malloc(PERIOD_SIZE * pcm->channels * sizeof(short));
    if(!in_buffer || !out_buffer){
        printf("Failed to allocate memory\n");
        return EXIT_FAILURE;
    }


    printf("playing wav ...\n");
    while(1){
        size_t frames_read = pcm_read(pcm, in_buffer, PERIOD_SIZE);
        if(frames_read == 0)
            break;
        fir_process(left_fir, right_fir, in_buffer, out_buffer, frames_read);
        status = pcm_write(pcm, out_buffer, frames_read);
        if(status != PCM_VALID)
            break;
    }
    printf("Done playing wav\n");

    //Tidy up
    close_pcm(pcm);
    free(pcm);
    free(left_fir);
    free(right_fir);
    free(in_buffer);
    free(out_buffer);
    
    return 0;
}
