#ifndef JACKSTATUS_H
#define JACKSTATUS_H

#include <jack/transport.h>

typedef struct {
    int		realtime;
    int		active;
    long	sample_rate;
    float	cpu_load;
    jack_nframes_t buf_size;
    jack_nframes_t latency;
} jack_status_t;

void io_get_status(jack_status_t *jp);

#endif
