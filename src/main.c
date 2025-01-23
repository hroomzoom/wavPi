#include <sndfile.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "pcm.h"
#include "fir.h"
#include "coeffs.h"

// #define NUM_TAPS 330

// static float32_t state[PERIOD_SIZE + NUM_TAPS - 1];

// const float32_t coeffs[NUM_TAPS] = {
//     -0.000003, -0.000003, -0.000003, -0.000004, -0.000004,
//     -0.000004, -0.000005, -0.000006, -0.000007, -0.000008,
//     -0.000009, -0.000010, -0.000011, -0.000012, -0.000014,
//     -0.000016, -0.000017, -0.000019, -0.000021, -0.000023,
//     -0.000025, -0.000028, -0.000030, -0.000033, -0.000035,
//     -0.000038, -0.000041, -0.000044, -0.000047, -0.000050,
//     -0.000053, -0.000056, -0.000059, -0.000062, -0.000065,
//     -0.000068, -0.000071, -0.000073, -0.000076, -0.000079,
//     -0.000081, -0.000083, -0.000084, -0.000086, -0.000087,
//     -0.000087, -0.000087, -0.000087, -0.000086, -0.000084,
//     -0.000082, -0.000079, -0.000075, -0.000070, -0.000064,
//     -0.000057, -0.000048, -0.000039, -0.000028, -0.000016,
//     -0.000002, 0.000013, 0.000030, 0.000049, 0.000069,
//     0.000092, 0.000117, 0.000143, 0.000173, 0.000204,
//     0.000238, 0.000274, 0.000314, 0.000355, 0.000400,
//     0.000448, 0.000499, 0.000552, 0.000610, 0.000670,
//     0.000734, 0.000801, 0.000872, 0.000946, 0.001025,
//     0.001107, 0.001192, 0.001282, 0.001375, 0.001473,
//     0.001574, 0.001680, 0.001789, 0.001903, 0.002021,
//     0.002142, 0.002268, 0.002398, 0.002532, 0.002669,
//     0.002811, 0.002957, 0.003106, 0.003259, 0.003416,
//     0.003576, 0.003740, 0.003907, 0.004078, 0.004251,
//     0.004428, 0.004607, 0.004789, 0.004974, 0.005161,
//     0.005350, 0.005541, 0.005733, 0.005928, 0.006124,
//     0.006320, 0.006518, 0.006717, 0.006915, 0.007114,
//     0.007313, 0.007512, 0.007710, 0.007907, 0.008103,
//     0.008298, 0.008491, 0.008682, 0.008871, 0.009057,
//     0.009241, 0.009422, 0.009599, 0.009773, 0.009943,
//     0.010109, 0.010270, 0.010427, 0.010579, 0.010726,
//     0.010868, 0.011004, 0.011134, 0.011259, 0.011377,
//     0.011489, 0.011594, 0.011692, 0.011784, 0.011868,
//     0.011945, 0.012015, 0.012078, 0.012133, 0.012180,
//     0.012219, 0.012251, 0.012275, 0.012291, 0.012299,
//     0.012299, 0.012291, 0.012275, 0.012251, 0.012219,
//     0.012180, 0.012133, 0.012078, 0.012015, 0.011945,
//     0.011868, 0.011784, 0.011692, 0.011594, 0.011489,
//     0.011377, 0.011259, 0.011134, 0.011004, 0.010868,
//     0.010726, 0.010579, 0.010427, 0.010270, 0.010109,
//     0.009943, 0.009773, 0.009599, 0.009422, 0.009241,
//     0.009057, 0.008871, 0.008682, 0.008491, 0.008298,
//     0.008103, 0.007907, 0.007710, 0.007512, 0.007313,
//     0.007114, 0.006915, 0.006717, 0.006518, 0.006320,
//     0.006124, 0.005928, 0.005733, 0.005541, 0.005350,
//     0.005161, 0.004974, 0.004789, 0.004607, 0.004428,
//     0.004251, 0.004078, 0.003907, 0.003740, 0.003576,
//     0.003416, 0.003259, 0.003106, 0.002957, 0.002811,
//     0.002669, 0.002532, 0.002398, 0.002268, 0.002142,
//     0.002021, 0.001903, 0.001789, 0.001680, 0.001574,
//     0.001473, 0.001375, 0.001282, 0.001192, 0.001107,
//     0.001025, 0.000946, 0.000872, 0.000801, 0.000734,
//     0.000670, 0.000610, 0.000552, 0.000499, 0.000448,
//     0.000400, 0.000355, 0.000314, 0.000274, 0.000238,
//     0.000204, 0.000173, 0.000143, 0.000117, 0.000092,
//     0.000069, 0.000049, 0.000030, 0.000013, -0.000002,
//     -0.000016, -0.000028, -0.000039, -0.000048, -0.000057,
//     -0.000064, -0.000070, -0.000075, -0.000079, -0.000082,
//     -0.000084, -0.000086, -0.000087, -0.000087, -0.000087,
//     -0.000087, -0.000086, -0.000084, -0.000083, -0.000081,
//     -0.000079, -0.000076, -0.000073, -0.000071, -0.000068,
//     -0.000065, -0.000062, -0.000059, -0.000056, -0.000053,
//     -0.000050, -0.000047, -0.000044, -0.000041, -0.000038,
//     -0.000035, -0.000033, -0.000030, -0.000028, -0.000025,
//     -0.000023, -0.000021, -0.000019, -0.000017, -0.000016,
//     -0.000014, -0.000012, -0.000011, -0.000010, -0.000009,
//     -0.000008, -0.000007, -0.000006, -0.000005, -0.000004,
//     -0.000004, -0.000004, -0.000003, -0.000003, -0.000003
// };

void usage(const char* arg_zero){
    printf("Usage: %s <path/to/wav> <filter>\n", arg_zero);
    printf("filter options:\n");
    printf(" --low\n");  //low pass
    printf(" --high\n"); //high pass
    printf(" --band\n"); //band pass
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
       strcmp(filter_str,"--band") != 0){
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
    if(strcmp(filter_str,"--low") == 0){
        status = fir_init(left_fir, lowpass_state, lowpass_coeffs, LOWPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;
        status = fir_init(right_fir, lowpass_state, lowpass_coeffs, LOWPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;
    }
    else if(strcmp(filter_str,"--high") == 0){
        status = fir_init(left_fir, highpass_state, highpass_coeffs, HIGHPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;
        status = fir_init(right_fir, highpass_state, highpass_coeffs, HIGHPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;

    }
    else{
         status = fir_init(left_fir, bandpass_state, bandpass_coeffs, BANDPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;
        status = fir_init(right_fir, bandpass_state, bandpass_coeffs, BANDPASS_NUM_TAPS);
        if(status != FIR_SUCCESS)
            return FIR_FAIL;

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
