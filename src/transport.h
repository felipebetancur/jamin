#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <jack/transport.h>

void transport_control(jack_nframes_t nframes);
void transport_status(jack_transport_info_t *jp);

/* UI functions */
void transport_play();
void transport_rewind();
void transport_stop();
void transport_toggle_play();

#endif /* TRANSPORT_H */
