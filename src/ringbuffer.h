#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#include <jack/types.h>

typedef struct  
{
  jack_default_audio_sample_t *buf;
  jack_nframes_t len;
} 
ringbuffer_data_t ;

typedef struct
{
  jack_default_audio_sample_t *buf;
  volatile jack_nframes_t write_ptr;
  volatile jack_nframes_t read_ptr;
  jack_nframes_t size;
  jack_nframes_t size_mask;
  int mlocked;
} 
ringbuffer_t ;

ringbuffer_t *ringbuffer_create(int sz);
void ringbuffer_free(ringbuffer_t *rb);

int ringbuffer_mlock(ringbuffer_t *rb);
void ringbuffer_reset(ringbuffer_t *rb);

void ringbuffer_write_advance(ringbuffer_t *rb, jack_nframes_t cnt);
void ringbuffer_read_advance(ringbuffer_t *rb, jack_nframes_t cnt);

jack_nframes_t ringbuffer_write_space(ringbuffer_t *rb);
jack_nframes_t ringbuffer_read_space(ringbuffer_t *rb);

jack_nframes_t ringbuffer_read(ringbuffer_t *rb,
			       jack_default_audio_sample_t *dest,
			       jack_nframes_t cnt);
jack_nframes_t ringbuffer_write(ringbuffer_t *rb,
				jack_default_audio_sample_t *src,
				jack_nframes_t cnt);

void ringbuffer_get_read_vector(ringbuffer_t *rb, ringbuffer_data_t *vec);
void ringbuffer_get_write_vector(ringbuffer_t *rb, ringbuffer_data_t *vec);

#endif
