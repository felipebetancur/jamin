#ifndef IO_H
#define IO_H

#include <jack/types.h>

/* types of latency sources */
#define LAT_BUFFERS	0		/* I/O buffering */
#define LAT_FFT		1		/* Fourier transform */
#define LAT_LIMITER	2		/* Limiter */
#define LAT_NSOURCES	3

void io_set_latency(int latency_source, jack_nframes_t delay);

void io_set_granularity(jack_nframes_t block_size);

extern float sample_rate;

void io_init(int argc, char *argv[]);
void io_activate();
void io_cleanup();

#endif
