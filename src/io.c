#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#include "process.h"
#include "plugin.h"
#include "io.h"

void cleanup(void);

jack_client_t *client;

static int dummy_mode = 0;

static int nchannels = NCHANNELS;	/* actual number of channels */
static jack_port_t *input_ports[NCHANNELS];
static jack_port_t *output_ports[NCHANNELS];
static char *in_names[NCHANNELS] = { "in_L", "in_R" };
static char *out_names[NCHANNELS] = { "out_L", "out_R" };

/* I/O system latencies */
static jack_nframes_t total_latency = 0;
static jack_nframes_t latency_delay[LAT_NSOURCES] = {0};
static char *latency_sources[LAT_NSOURCES] = {
    "I/O Buffering",
    "Fourier Transform",
    "Limiter"
};

void io_set_latency(int source, jack_nframes_t delay)
{
    if (source < 0 || source >= LAT_NSOURCES) {
	printf("JAM internal error: unknown latency source.\n");
	return;
    }

    printf("Latency due to %s is %ld frames.\n",
	   latency_sources[source], delay);
    total_latency += delay - latency_delay[source];
    latency_delay[source] = delay;
}

static jack_nframes_t io_block_size;

/* io_set_granularity -- set desired I/O block size.
   Must be called *before* starting the JACK process thread.
*/
void io_set_granularity(jack_nframes_t block_size)
{
    printf("JAM I/O granularity is %ld frames.\n", block_size);
    io_block_size = block_size;
}

/* io_process -- JACK process callback.
   Runs as a high-priority realtime thread.  CANNOT EVER WAIT.
*/
int io_process(jack_nframes_t nframes, void *arg)
{
    jack_default_audio_sample_t *in[NCHANNELS], *out[NCHANNELS];
    int chan;
    int return_code = 0;		/* 0 means success */

    /* get input and output buffer addresses from JACK */
    for (chan = 0; chan < nchannels; chan++) {
	assert(input_ports[chan] && output_ports[chan]);
	in[chan] = jack_port_get_buffer(input_ports[chan], nframes);
	out[chan] = jack_port_get_buffer(output_ports[chan], nframes);
    }

    assert(io_block_size);
    if (nframes < io_block_size) {
	/* JACK buffer is smaller than desired DSP granularity.  Soon,
	   we will dispatch a separate thread to handle this case,
	   queuing buffers to it until there are io_block_size frames
	   available.  For now, just process it one small chunks at a
	   time.
	 */
	return process_signal(nframes, nchannels, in, out);
    }

    assert(nframes % io_block_size == 0);
    while (nframes >= io_block_size)  {

	int rc = process_signal(io_block_size, nchannels, in, out);
	if (rc != 0)
	    return_code = rc;

	for (chan = 0; chan < nchannels; chan++) {
	    in[chan] += io_block_size;
	    out[chan] += io_block_size;
	}
	nframes -= io_block_size;
    }

    return return_code;
}

int backend_init(int argc, char *argv[])
{
    int opt;
    int show_help = 0;
    char client_name[256];

    while ((opt = getopt(argc, argv, "hd")) != -1) {
	switch (opt) {
	case 'h':
	    /* Force help to be shown */
	    show_help = 1;
	    break;
	case 'd':
	    /* Dummy mode, no jack */
	    dummy_mode = 1;
	    break;
	default:
	    show_help = 1;
	    break;
	}
    }

    if (dummy_mode) {
	process_init(48000.0f, 1024);
	return 0;
    }

    if ((argc != 5 && argc != 1) || show_help) {
	fprintf(stderr,
		"Usage %s: [<inport> <inport> <outport> <outport>]\n\n",
		argv[0]);
	exit(1);
    }

    /* Register with jack */
    snprintf(client_name, 255, "jam");
    if ((client = jack_client_new(client_name)) == 0) {
	fprintf(stderr, "jack server not running?\n");
	exit(1);
    }
    printf("Registering as %s\n", client_name);

    process_init((float) jack_get_sample_rate(client),
		 jack_get_buffer_size(client));

    jack_set_process_callback(client, io_process, NULL);

    return 0;
}

int backend_activate(int argc, char *argv[])
{
    char *ioports[4];
    unsigned int i;
    int chan;

    if (dummy_mode) {
	return 0;
    }

    for (chan = 0; chan < nchannels; chan++) {
	input_ports[chan] =
	    jack_port_register(client, in_names[chan],
			       JACK_DEFAULT_AUDIO_TYPE,
			       JackPortIsInput, 0);
	// jack_port_set_latency(input_ports[chan], total_latency);
	output_ports[chan] =
	    jack_port_register(client, out_names[chan],
			       JACK_DEFAULT_AUDIO_TYPE,
			       JackPortIsOutput, 0);
    }

    // JOQ: allocate DSP buffers
    // JOQ: create DSP thread

    /* Note: All ports MUST already be registered. */
    if (jack_activate(client)) {
	fprintf(stderr, "cannot activate client");
	exit(1);
    }

    if (argc == 5) {
	for (i = 0; i < 4; i++) {
	    ioports[i] = argv[i + 1];
	}
    } else {
	ioports[0] = "alsa_pcm:capture_1";
	ioports[1] = "alsa_pcm:capture_2";
	ioports[2] = "alsa_pcm:playback_1";
	ioports[3] = "alsa_pcm:playback_2";
    }

    if (jack_connect(client, ioports[0], jack_port_name(input_ports[0]))) {
	fprintf(stderr, "Cannot make connection\n");
    }
    if (jack_connect(client, ioports[1], jack_port_name(input_ports[1]))) {
	fprintf(stderr, "Cannot make connection\n");
    }
    if (jack_connect(client, jack_port_name(output_ports[0]), ioports[2])) {
	fprintf(stderr, "Cannot make connection\n");
    }
    if (jack_connect(client, jack_port_name(output_ports[1]), ioports[3])) {
	fprintf(stderr, "Cannot make connection\n");
    }

    return 0;
}

void cleanup()
{
    printf("quiting\n");

    /* Leave the jack graph */
    if (!dummy_mode) {
	jack_client_close(client);
    }

    /* And we're done */
    exit(0);
}

/* vi:set ts=8 sts=4 sw=4: */
