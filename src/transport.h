#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <jack/transport.h>

void transport_control(jack_nframes_t nframes);
jack_transport_state_t transport_get_state();

/* UI functions */
void transport_play();
void transport_set_position(jack_nframes_t frame);
void transport_stop();
void transport_toggle_play();

#endif /* TRANSPORT_H */
