#ifndef JACKSTATUS_H
#define JACKSTATUS_H

#include <jack/transport.h>

typedef struct {
    volatile jack_nframes_t guard1;

    int realtime;
    jack_nframes_t buf_size;
    jack_nframes_t latency;
    long sample_rate;
    float cpu_load;

    /* transport info, consistent when (guard1 == guard2) */
    jack_transport_info_t tinfo;

    volatile jack_nframes_t guard2;
} jack_status_t;

void io_get_status(jack_status_t *jp);

#endif
