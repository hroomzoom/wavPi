#include <sndfile.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "pcm.h"
#include "fir.h"
#include "coeffs.h"
#include "lfo.h"
#include "ui.h"


typedef struct {
    PCM *pcm;
    char *filter;
    float32_t lfo_freq;
    float32_t lfo_depth;
    pthread_mutex_t mutex;
    int running;  
} thread_args_t;

void setup_pcm(PCM *pcm, const char *wav_file);
void *audio_process(void *arg);
void *input_process(void *arg);

int main(int argc, char **argv){

    //Process args
    if(argc < 2){
        printf("Usage: <path/to/wav> <filter>\n");
        return EXIT_FAILURE;
    }

    PCM *pcm = (PCM *)malloc(sizeof(PCM));
    setup_pcm(pcm, argv[1]);

    pthread_t audio_thread, input_thread;
    thread_args_t args;
    args.pcm = pcm;
    args.filter = "none";
    args.running = 1;
    args.lfo_depth = levels[0]/100.0f;
    args.lfo_freq = (levels[0]/100.0f)*19 + 1;
    pthread_mutex_init(&args.mutex, NULL);
    

    pthread_create(&audio_thread, NULL, audio_process, &args);
    pthread_create(&input_thread, NULL, input_process, &args);

    // Wait for threads to finish
    pthread_join(input_thread, NULL);
    pthread_join(audio_thread, NULL);

    pthread_mutex_destroy(&args.mutex);
    close_pcm(pcm);
    free(pcm);
    return 0;
}

void setup_pcm(PCM *pcm, const char *wav_file){
    open_pcm(pcm, wav_file);
    setup_pcm_hw(pcm);
    setup_pcm_sw(pcm); 
}

void setup_fir(FIR *left, FIR *right, const char *filter){
    
    if(strcmp(filter,"low") == 0){
        fir_init(left, lowpass_state, lowpass_coeffs, LOWPASS_NUM_TAPS);
        fir_init(right, lowpass_state, lowpass_coeffs, LOWPASS_NUM_TAPS);
    }
    else if(strcmp(filter,"high") == 0){
        fir_init(left, highpass_state, highpass_coeffs, HIGHPASS_NUM_TAPS);
        fir_init(right, highpass_state, highpass_coeffs, HIGHPASS_NUM_TAPS);
    }
    else if(strcmp(filter,"band") == 0){
        fir_init(left, bandpass_state, bandpass_coeffs, BANDPASS_NUM_TAPS);
        fir_init(right, bandpass_state, bandpass_coeffs, BANDPASS_NUM_TAPS);
    }

}

void *audio_process(void *arg){

    thread_args_t *args = (thread_args_t *)arg;

    FIR *left_low = (FIR *)malloc(sizeof(FIR));
    FIR *right_low = (FIR *)malloc(sizeof(FIR));
    setup_fir(left_low, right_low, "low");

    FIR *left_high = (FIR *)malloc(sizeof(FIR));
    FIR *right_high = (FIR *)malloc(sizeof(FIR));
    setup_fir(left_high, right_high, "high");

    FIR *left_band = (FIR *)malloc(sizeof(FIR));
    FIR *right_band = (FIR *)malloc(sizeof(FIR));
    setup_fir(left_band, right_band, "band");

    short *in_buffer = (short *)malloc(PERIOD_SIZE * args->pcm->channels * sizeof(short));
    short *out_buffer = (short *)malloc(PERIOD_SIZE * args->pcm->channels * sizeof(short));

    int status;
    float32_t lfo_freq, lfo_depth;

    while(1){
        
        pthread_mutex_lock(&args->mutex);
        int running = args->running; 
        lfo_freq = args->lfo_freq;
        lfo_depth = args->lfo_depth;  
        pthread_mutex_unlock(&args->mutex);

        if (!running)
            break; 

        size_t frames_read = pcm_read(args->pcm, in_buffer, PERIOD_SIZE);
        if(frames_read == 0)
            break;
        

        process_tremolo(in_buffer, out_buffer, lfo_freq, lfo_depth, PERIOD_SIZE);
        status = pcm_write(args->pcm, out_buffer, frames_read);
        
        if(status != PCM_VALID)
            break;
    }

    free(left_low);
    free(right_low);
    free(left_high);
    free(right_high);
    free(left_band);
    free(right_band);
    free(in_buffer);
    free(out_buffer);

    return NULL;
}

void *input_process(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    int input;
    int selected = 0;

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, 1);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(LEVEL_COLOR, COLOR_GREEN, COLOR_BLACK); // Slider level
        init_pair(ITEM_COLOR, COLOR_WHITE, COLOR_BLACK); // Top bar
        init_pair(CHANNEL_COLOR, COLOR_BLACK,  COLOR_GREEN); // Highlighted channel
    }

    while (1) {
        
        draw_ui(selected);
        input = getch();

        pthread_mutex_lock(&args->mutex);
        switch (input) {
            case KEY_LEFT:
                selected = (selected - 1 + NUM_CHANNELS) % NUM_CHANNELS;
                break;
            case KEY_RIGHT:
                selected = (selected + 1) % NUM_CHANNELS;
                break;
            case KEY_UP:
                if (levels[selected] < 100) {
                    levels[selected] += 10;
                    if (selected == 0)
                        args->lfo_depth = levels[selected] / 100.0f;
                    else
                        args->lfo_freq = (levels[selected] / 100.0f) * 19 + 1;
                }
                break;
            case KEY_DOWN:
                if (levels[selected] > 0) {
                    levels[selected] -= 10;
                    if (selected == 0)
                        args->lfo_depth = levels[selected] / 100.0f;
                    else
                        args->lfo_freq = (levels[selected] / 100.0f) * 19 + 1;
                }
                break;
            case 27: // Escape key
                args->running = 0;
                pthread_mutex_unlock(&args->mutex);
                endwin();
                return NULL;
        }
        pthread_mutex_unlock(&args->mutex);
    }
    endwin();
    return NULL;
}

