#ifndef PCM_H
#define PCM_H

#include <sndfile.h>
#include <alsa/asoundlib.h>
#include <stdio.h>

#define BUFFER_SIZE 24000
#define PERIOD_SIZE 6000

#define PCM_VALID 0
#define PCM_INVALID -1

typedef struct {
    unsigned int rate;
    unsigned int channels;
    int res; 
    SNDFILE *sndfile;
    snd_pcm_t *handle; 
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_sw_params_t *swparams;
    
}PCM;

int open_pcm(PCM *pcm, const char* file);
int setup_pcm_hw(PCM *pcm);
int setup_pcm_sw(PCM *pcm);
size_t pcm_read(PCM *pcm, short *buffer, int frames);
int pcm_write(PCM *pcm, short *buffer, size_t frames);
void close_pcm(PCM *pcm);

#endif