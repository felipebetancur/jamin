#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "ladspa.h"
#include "plugin.h"

static char *plugin_path;

void plugin_init()
{
	if (getenv("LADSPA_PATH")) {
		plugin_path = getenv("LADSPA_PATH");
	} else {
		plugin_path = "/usr/local/lib/ladspa:/usr/lib/ladspa";
	}
}

plugin *plugin_load(char *file)
{
	char *dir;
	char *path_tok;
	char path[512];
	void *dl;

	path_tok = strdup(plugin_path);
	for (dir = strtok(path_tok, ":"); dir; dir = strtok(NULL, ":")) {
		snprintf(path, 511, "%s/%s", dir, file);
		if ((dl = dlopen(path, RTLD_LAZY))) {
			plugin *ret = malloc(sizeof(plugin));
			LADSPA_Descriptor *(*d)(unsigned long);

			ret->dl = dl;
			d = dlsym(dl, "ladspa_descriptor");
			ret->descriptor = (*d)(0);

			return ret;
		}
	}
	fprintf(stderr, "Cannot find plugin '%s'\n", file);
	free(path_tok);

	return NULL;
}

LADSPA_Handle plugin_instantiate(plugin *p, unsigned long fs)
{
	if (p->descriptor->instantiate) {
		return (*p->descriptor->instantiate)(p->descriptor, fs);
	} else {
	       fprintf(stderr, "Cannot find instantiate function for %s\n", p->descriptor->Label);
	}

	return NULL;
}
