#include "pcm.h"

int open_pcm(PCM *pcm, const char* file){
    
    //Open file
    SF_INFO sfinfo;
    pcm->sndfile = sf_open(file ,SFM_READ, &sfinfo);

    //Get rate and channel num
    pcm->rate = sfinfo.samplerate;
    pcm->channels = sfinfo.channels;

    //Check channels
    if(pcm->channels != 2){
        printf("Error: must be 2 channel\n");
        return PCM_INVALID;
    }

    //Check format
    unsigned int fileType = sfinfo.format & SF_FORMAT_TYPEMASK;
    unsigned int encoding = sfinfo.format & SF_FORMAT_SUBMASK;

    if(fileType != SF_FORMAT_WAV){
        printf("Error: invalid WAV format\n");
        return PCM_INVALID;
    }

    if(encoding != SF_FORMAT_PCM_16){
        printf("Error: encoding must be 16-bit\n");
        return PCM_INVALID;
    }

    //Open handle
    pcm->handle = NULL;
    pcm->res = snd_pcm_open(&pcm->handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (pcm->res < 0) {
        fprintf(stderr, "Failed to open PCM device: %s\n", snd_strerror(pcm->res));
        return PCM_INVALID;
    }

    return PCM_VALID;
}

int setup_pcm_hw(PCM *pcm){
    
    //Set hw parameters
    unsigned int buffer_size = BUFFER_SIZE;
    snd_pcm_uframes_t period_size = PERIOD_SIZE;

    snd_pcm_hw_params_alloca(&pcm->hwparams);
    pcm->res = snd_pcm_hw_params_any(pcm->handle, pcm->hwparams);
    if (pcm->res < 0) {
        fprintf(stderr, "Failed to initialize hardware parameters: %s\n", snd_strerror(pcm->res));
        snd_pcm_close(pcm->handle);
        return PCM_INVALID;
    }

    snd_pcm_hw_params_set_access(pcm->handle, pcm->hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm->handle, pcm->hwparams, pcm->channels);
    snd_pcm_hw_params_set_rate(pcm->handle, pcm->hwparams, pcm->rate, 0);
    snd_pcm_hw_params_set_buffer_size(pcm->handle, pcm->hwparams, buffer_size);
    snd_pcm_hw_params_set_period_size(pcm->handle, pcm->hwparams, period_size, 0);
    pcm->res = snd_pcm_hw_params(pcm->handle, pcm->hwparams);
    if (pcm->res < 0) {
        fprintf(stderr, "Failed to set hardware parameters: %s\n", snd_strerror(pcm->res));
        snd_pcm_close(pcm->handle);
        return PCM_INVALID;
    }

    return PCM_VALID;
}

int setup_pcm_sw(PCM *pcm){

    //Set sw params
    unsigned int avail_min = PERIOD_SIZE;
    unsigned int start_threshold = BUFFER_SIZE;
    unsigned int stop_threshold = BUFFER_SIZE;
    unsigned int silence_threshold = 0;
    unsigned int silence_size = 0;

    snd_pcm_sw_params_alloca(&pcm->swparams);
    snd_pcm_sw_params_set_avail_min(pcm->handle, pcm->swparams, avail_min);
    snd_pcm_sw_params_set_start_threshold(pcm->handle, pcm->swparams, start_threshold);
    snd_pcm_sw_params_set_stop_threshold(pcm->handle, pcm->swparams, stop_threshold);
    snd_pcm_sw_params_set_silence_threshold(pcm->handle, pcm->swparams, silence_threshold);
    snd_pcm_sw_params_set_silence_size(pcm->handle, pcm->swparams, silence_size);
    pcm->res = snd_pcm_sw_params(pcm->handle, pcm->swparams);
    if (pcm->res < 0) {
        fprintf(stderr, "Failed to set hardware parameters: %s\n", snd_strerror(pcm->res));
        snd_pcm_close(pcm->handle);
        return PCM_INVALID;
    }
    return PCM_VALID;

}

size_t pcm_read(PCM *pcm, short *buffer, int frames){
    return sf_readf_short(pcm->sndfile, buffer, frames);
}

int pcm_write(PCM *pcm, short *buffer, size_t frames){
    int err = snd_pcm_writei(pcm->handle, buffer, frames);
    if (err == -EPIPE) {
        fprintf(stderr, "Buffer underrun occurred, recovering...\n");
        snd_pcm_prepare(pcm->handle);
    } else if (err < 0) {
        fprintf(stderr, "Error writing to PCM device: %s\n", snd_strerror(err));
        return PCM_INVALID;
    }
    return PCM_VALID;
}

void close_pcm(PCM* pcm){
    snd_pcm_drain(pcm->handle);
	snd_pcm_close(pcm->handle);
}

