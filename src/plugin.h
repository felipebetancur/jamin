#ifndef PLUGIN_H
#define PLUGIN_H

#include "ladspa.h"

typedef struct {
	void *dl;
	LADSPA_Descriptor *descriptor;
} plugin;

void plugin_init();

plugin *plugin_load(char *file);

LADSPA_Handle plugin_instantiate(plugin *p, unsigned long fs);

static inline void plugin_connect_port(plugin *p, LADSPA_Handle h, unsigned
		long port, LADSPA_Data *data);

static inline void plugin_run(plugin *p, LADSPA_Handle *h, unsigned long
		sample_count);

static inline void plugin_connect_port(plugin *p, LADSPA_Handle h, unsigned
		long port, LADSPA_Data *data)
{
	(*p->descriptor->connect_port)(h, port, data);
}

static inline void plugin_run(plugin *p, LADSPA_Handle *h, unsigned long
		sample_count)
{
	(*p->descriptor->run)(h, sample_count);
}

#endif
