#include <sndfile.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "pcm.h"
#include "fir.h"
#include "coeffs.h"


void usage(const char* arg_zero){
    printf("Usage: %s <path/to/wav> <filter>\n", arg_zero);
    printf("filter options:\n");
    printf(" --low\n");  //low pass
    printf(" --high\n"); //high pass
    printf(" --band\n"); //band pass
    printf(" --norm\n"); //normal
}

int main(int argc, char **argv){

    //Process args
    if(argc < 3){
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    //Check filter arg
    char *filter_str = argv[2];
    if(strcmp(filter_str,"--low") != 0 && 
       strcmp(filter_str,"--high") != 0 && 
       strcmp(filter_str,"--band") != 0 &&
       strcmp(filter_str,"--norm")){
        usage(argv[0]);
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
    //Init based on selected filter
    int filter_on = 0;

    if(strcmp(filter_str,"--low") == 0){
        status = fir_init(left_fir, lowpass_state, lowpass_coeffs, LOWPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;
        status = fir_init(right_fir, lowpass_state, lowpass_coeffs, LOWPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;
        
        filter_on = 1;
    }
    else if(strcmp(filter_str,"--high") == 0){
        status = fir_init(left_fir, highpass_state, highpass_coeffs, HIGHPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;
        status = fir_init(right_fir, highpass_state, highpass_coeffs, HIGHPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;
        
        filter_on = 1;
    }
    else if(strcmp(filter_str,"--band") == 0){
         status = fir_init(left_fir, bandpass_state, bandpass_coeffs, BANDPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;
        status = fir_init(right_fir, bandpass_state, bandpass_coeffs, BANDPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;

        filter_on = 1;
    }
    
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
        if(filter_on){
            fir_process(left_fir, right_fir, in_buffer, out_buffer, frames_read);
            status = pcm_write(pcm, out_buffer, frames_read);
        }
        else
            status = pcm_write(pcm, in_buffer, frames_read);

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
