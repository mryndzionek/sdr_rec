/*  =========================================================================
    rtl_demod - rtl_fm clone using liquid-dsp library

    -------------------------------------------------------------------------
    Copyright (c) 2013 Mariusz Ryndzionek - mryndzionek@gmail.com           

    This is free software; you can redistribute it and/or modify it under   
    the terms of the GNU Lesser General Public License as published by the  
    Free Software Foundation; either version 3 of the License, or (at your  
    option) any later version.                                              

    This software is distributed in the hope that it will be useful, but    
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTA-   
    BILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General  
    Public License for more details.                                        

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see http://www.gnu.org/licenses/.      
    =========================================================================
 */

#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <sys/resource.h>
#include <liquid/liquid.h>
#include <rtl-sdr.h>

#include "normalizer.h"
#include "debug.h"
#include "convenience.h"

#define DEFAULT_SAMPLE_RATE		2048000
#define DEFAULT_ASYNC_BUF_NUMBER	32
#define DEFAULT_BUF_LENGTH		(4 * 1024)
#define MINIMAL_BUF_LENGTH		512
#define MAXIMAL_BUF_LENGTH		(256 * 16384)

void usage() {
    printf("Usage: rtl_demod [OPTION]\n");
    printf("\n");
    printf("  h     : help\n");
    printf("  f     : center frequency [Hz], default: 100 MHz\n");
    printf("  b     : bandwidth [Hz],        default: 800 kHz\n");
    printf("  B     : output_block_size      default: 16 * 16384\n");
    printf("  G     : gain [dB],             default:  0 = auto\n");
    printf("  p     : ppm_error,             default:  0\n");
    printf("  n     : FFT size,              default:  64\n");
    printf("  o     : offset                 default: -65 dB\n");
    printf("  s     : samplerate,            default: 2048000 Hz)]\n");
    printf("  r     : FFT rate [Hz],         default:   10 Hz\n");
    printf("  L     : output file log size,  default: 4096 samples\n");
    printf("  F     : output filename,       default: 'asgram_rx.dat'\n");
    printf("  d     : device_index,          default: 0\n");
}

static int do_exit = 0;
static uint32_t bytes_to_read = 0;
static rtlsdr_dev_t *dev = NULL;

static void sighandler(int signum)
{
    fprintf(stderr, "Signal caught, exiting!\n");
    do_exit = 1;
    rtlsdr_cancel_async(dev);
}

static int16_t to_int16(float f)
{
    if (f > 1.0) f = 1.0;
    if (f < -1.0) f = -1.0;
    return (int16_t)(f * 0x7fff);
}

// main program
int main (int argc, char **argv)
{
    // command-line options
    int verbose = 1;

    int ppm_error = 0;
    int gain = 0;
    float rx_resamp_rate;
    float bandwidth      = 800e3f;
    int r, n_read;

    uint32_t frequency = 100000000;
    uint32_t samp_rate = DEFAULT_SAMPLE_RATE;
    uint32_t out_block_size = DEFAULT_BUF_LENGTH;
    uint8_t *buffer;
    complex float *buffer_norm;

    int dev_index = 0;
    int dev_given = 0;

    struct sigaction sigact;
    normalizer_t *norm;

    float kf = 0.1f;                    // modulation factor
    liquid_freqdem_type type = LIQUID_FREQDEM_DELAYCONJ;

    //
    int d;
    while ((d = getopt(argc,argv,"hf:b:B:G:p:s:")) != EOF) {
            switch (d) {
                case 'h':   usage();                    return 0;
                case 'f':   frequency   = atof(optarg); break;
                case 'b':   bandwidth   = atof(optarg); break;
                case 'B':   out_block_size = (uint32_t)atof(optarg); break;
                case 'G':   gain = (int)(atof(optarg) * 10); break;
                case 'p':   ppm_error = atoi(optarg); break;
                case 's':   samp_rate = (uint32_t)atofs(optarg); break;
                case 'd':
                    dev_index = verbose_device_search(optarg);
                    dev_given = 1;
                    break;
                default:    usage();                    return 1;
            }
    }

    if (!dev_given) {
            dev_index = verbose_device_search("0");
    }

    if (dev_index < 0) {
            exit(1);
    }

    r = rtlsdr_open(&dev, (uint32_t)dev_index);
    if (r < 0) {
            fprintf(stderr, "Failed to open rtlsdr device #%d.\n", dev_index);
            exit(1);
    }

    sigact.sa_handler = sighandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, NULL);
    sigaction(SIGTERM, &sigact, NULL);
    sigaction(SIGQUIT, &sigact, NULL);
    sigaction(SIGPIPE, &sigact, NULL);

    /* Set the sample rate */
    verbose_set_sample_rate(dev, samp_rate);

    /* Set the frequency */
    verbose_set_frequency(dev, frequency);

    if (0 == gain) {
            /* Enable automatic gain */
            verbose_auto_gain(dev);
    } else {
            /* Enable manual gain */
            gain = nearest_gain(dev, gain);
            verbose_gain_set(dev, gain);
    }

    verbose_ppm_set(dev, ppm_error);

    rx_resamp_rate = bandwidth/samp_rate;

    printf("frequency       :   %10.4f [MHz]\n", frequency*1e-6f);
    printf("bandwidth       :   %10.4f [kHz]\n", bandwidth*1e-3f);
    printf("sample rate     :   %10.4f kHz = %10.4f kHz * %8.6f\n",
           samp_rate * 1e-3f,
           bandwidth    * 1e-3f,
           1.0f / rx_resamp_rate);
    printf("verbosity       :    %s\n", (verbose?"enabled":"disabled"));

    unsigned int i,j;

    // add arbitrary resampling component
    msresamp_crcf resamp = msresamp_crcf_create(rx_resamp_rate, 60.0f);
    assert(resamp);

    //allocate recv buffer
    buffer = malloc(out_block_size * sizeof(uint8_t));
    assert(buffer);

    buffer_norm = malloc(out_block_size * sizeof(complex float));
    assert(buffer_norm);

    // create buffer for arbitrary resamper output
    complex float buffer_resamp[(int)(2.0f/rx_resamp_rate) + 64];
    int16_t buffer_demod[(int)(2.0f/rx_resamp_rate) + 64];
    debug("resamp_buffer_len: %d", (int)(2.0f/rx_resamp_rate) + 64);

    norm = normalizer_create();

    verbose_reset_buffer(dev);

    freqdem dem = freqdem_create(kf,type);

    while (!do_exit) {
            // grab data from device
            r = rtlsdr_read_sync(dev, buffer, out_block_size, &n_read);
            if (r < 0) {
                    fprintf(stderr, "WARNING: sync read failed.\n");
                    break;
            }

            if ((bytes_to_read > 0) && (bytes_to_read < (uint32_t)n_read)) {
                    n_read = bytes_to_read;
                    do_exit = 1;
            }

            // push data through arbitrary resampler and give to frame synchronizer
            // TODO : apply bandwidth-dependent gain
            for (i=0; i<n_read/2; i++) {
                    // grab sample from usrp buffer
                    buffer_norm[i] = normalizer_normalize(norm, *((uint16_t*)buffer+i));
            }
            // push through resampler (one at a time)
            unsigned int nw;
            float demod;
            msresamp_crcf_execute(resamp, buffer_norm, n_read/2, buffer_resamp, &nw);

            for(j=0;j<nw;j++)
                {
                    freqdem_demodulate(dem, buffer_resamp[j], &demod);
                    buffer_demod[j] = to_int16(demod);

                }

            if (fwrite(buffer_demod, 2, nw, stdout) != (size_t)nw) {
                    fprintf(stderr, "Short write, samples lost, exiting!\n");
                    break;
            }

            if ((uint32_t)n_read < out_block_size) {
                    fprintf(stderr, "Short read, samples lost, exiting!\n");
                    break;
            }

            if (bytes_to_read > 0)
                bytes_to_read -= n_read;

    }

    // destroy objects
    freqdem_destroy(dem);
    normalizer_destroy(&norm);
    msresamp_crcf_destroy(resamp);

    rtlsdr_close(dev);
    free (buffer);

    return 0;
}

