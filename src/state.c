#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libgen.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>

#include "config.h"
#include "main.h"
#include "state.h"
#include "process.h"
#include "scenes.h"

/* The smallest value that counts as a change, should be approximately
 * epsilon+delta */
#define MIN_CHANGE (FLT_EPSILON + FLT_EPSILON)

float                   s_value[S_SIZE];
static float            s_target[S_SIZE];
static int              s_duration[S_SIZE];
static int              s_changed[S_SIZE];
static GtkAdjustment   *s_adjustment[S_SIZE];
static s_callback_func  s_callback[S_SIZE];

static s_state       *last_state = NULL;
static int	      last_changed = S_NONE;

static GList         *history = NULL;
static GList         *undo_pos = NULL;

static int suppress_feedback = 0;

static void s_set_events(int id, float value);
void set_EQ_curve_values ();
void unset_scene_buttons ();
void s_update_title();

static const gchar *filename = NULL;

void state_init()
{
    unsigned int i;

    for (i=0; i<S_SIZE; i++) {
	s_value[i] = 0.0f;
	s_target[i] = 0.0f;
	s_duration[i] = 0;
	s_changed[i] = 0;
	s_adjustment[i] = NULL;
	s_callback[i] = NULL;
    }
    s_history_add("Initial state");
}

void s_suppress_push()
{
    suppress_feedback++;
}

void s_suppress_pop()
{
    suppress_feedback--;
}

void s_set_callback(int id, s_callback_func callback)
{
    assert(id >= 0 && id < S_SIZE);

    s_callback[id] = callback;
}

void s_set_adjustment(int id, GtkAdjustment *adjustment)
{
    assert(id >= 0 && id < S_SIZE);

    s_adjustment[id] = adjustment;
}

void s_set_value_ui(int id, float value)
{
    s_value[id] = value;

    if (suppress_feedback) {
	return;
    }
    assert(id >= 0 && id < S_SIZE);

    if (last_changed != id) {
	s_history_add(g_strdup_printf("%s = %f", s_description[id],
		      s_value[id]));
    }
    last_state->value[id] = value;

#if 0
    /* This code is confusing in use, so I've removed it - swh */

    if (value - MIN_CHANGE < last_state->value[id] &&
	value + MIN_CHANGE > last_state->value[id]) {
	last_changed = S_NONE;
    } else {
	last_changed = id;
    }
#else
    last_changed = id;
#endif

    if (s_callback[id]) {
	(*s_callback[id])(id, value);
    }
}

void s_set_value(int id, float value, int duration)
{
    /* We dont want to call this yet... s_set_value_ui(id, value); */
    s_duration[id] = duration;
    s_target[id] = value;
    if (s_adjustment[id]) {
	gtk_adjustment_set_value(s_adjustment[id], value);
    }
}

void s_set_value_block(float *values, int base, int count)
{
    int i;

    for (i = 0 ; i < count ; i++) {
	s_value[base + i] = values[i];
    }
    last_changed = base;
    //s_set_events(base, values[i]);
}

void s_set_value_no_history(int id, float value)
{
    suppress_feedback++;
    s_value[id] = value;
    s_target[id] = value;
    s_set_events(id, value);
    suppress_feedback--;
}

void s_clear_history()
{
    GList *p;

    for (p=history; p; p=p->next) {
	free(p->data);
    }
    g_list_free(history);
    history = NULL;
    s_history_add("Initial state");
    undo_pos = history;
    s_restore_state((s_state *)history->data);
}

void s_history_add(const char *description)
{
    s_state *ns;

    ns = malloc(sizeof(s_state));
    ns->description = (char *)description;
    memcpy(ns->value, s_value, S_SIZE * sizeof(float));
    if (undo_pos) {
	while (undo_pos->next) {
	    g_list_remove_all(history, undo_pos->next);
	}
    }
    history = g_list_append(history, ns);
    undo_pos = g_list_last(history);
    /* printf("add %s\n", description); */
    last_state = ns;
}

void s_undo() 
{
    if (!undo_pos) {
	return;
    }
    undo_pos = g_list_previous(undo_pos);
    if (!undo_pos) {
	return;
    }
    s_restore_state((s_state *)undo_pos->data);

    set_EQ_curve_values ();
}

void s_redo() 
{
    if (undo_pos) {
	if (undo_pos->next) {
	    undo_pos = g_list_next(undo_pos);
	    s_restore_state((s_state *)undo_pos->data);
	}
    } else {
	undo_pos = history;
	undo_pos = g_list_next(undo_pos);
	s_restore_state((s_state *)undo_pos->data);
    }

    set_EQ_curve_values ();
}

void s_restore_state(s_state *state)
{
    int i, duration;

    /* printf("restore %s\n", state->description); */
    /* crossfade in 3ms, sounds a bit better */
    duration = (int)(sample_rate * 0.003f);
    suppress_feedback++;
    for (i=0; i<S_SIZE; i++) {
	/* set the target and duration for crosssfade, but set the controls to
	 * the endpoint */
	s_target[i] = state->value[i];
	s_duration[i] = duration;
	//s_set_events(i, state->value[i]);
    }
    suppress_feedback--;
}

void s_crossfade_to_state(s_state *state, float time)
{
    int i, duration;

    /* printf("restore %s\n", state->description); */
    duration = (int)(sample_rate * time);
    suppress_feedback++;
    for (i=0; i<S_SIZE; i++) {
	/* set the target and duration for crosssfade, but set the controls to
	 * the endpoint */
	s_target[i] = state->value[i];
	s_duration[i] = duration;
	//s_set_events(i, state->value[i]);
    }
    suppress_feedback--;
}

static void s_set_events(int id, float value)
{
    if (s_callback[id]) {
	(*s_callback[id])(id, value);
    }
    if (s_adjustment[id]) {
	gtk_adjustment_set_value(s_adjustment[id], value);
    }
}

void s_set_description(int id, const char *desc)
{
    if (last_changed != id) {
	s_history_add(desc);
    }
    last_changed = id;
}

void s_save_session_from_ui (GtkWidget *w, gpointer user_data)
{
    GtkFileSelection *file_selector = (GtkFileSelection *) user_data;

    s_save_session(gtk_file_selection_get_filename (GTK_FILE_SELECTION (file_selector)));
}
    
void s_save_session (const char *fname)
{
    xmlDocPtr doc;
    xmlNodePtr rootnode, node, sc_node;
    unsigned int i, j;
    char tmp[256];

    /* Check to see if we have been passed a filename, if not fall back to
     * previous one */
    if (fname) {
	filename = fname;
	s_update_title();
    }
    if (!filename) {
	fprintf(stderr, "No filename found at %s:%d, not saving\n", __FILE__,
		__LINE__);
    }

    xmlSetCompressMode(5);
    doc = xmlNewDoc("1.0");
    rootnode = xmlNewDocRawNode(doc, NULL, "jam-param-list", NULL);
    xmlSetProp(rootnode, "version", VERSION);
    xmlDocSetRootElement(doc, rootnode);
    node = xmlNewText("\n");
    xmlAddChild(rootnode, node);

    /* Save current active state */
    for (i=0; i<S_SIZE; i++) {
	node = xmlNewDocRawNode(doc, NULL, "parameter", NULL);
	snprintf(tmp, 255, "%g", s_value[i]);
	xmlSetProp(node, "name", s_symbol[i]);
	xmlSetProp(node, "value", tmp);
	xmlAddChild(rootnode, node);
	node = xmlNewText("\n");
	xmlAddChild(rootnode, node);
    }

    /* Save scenes */
    for (j=0; j<NUM_SCENES; j++) {
	s_state *st = get_scene(j);
	sc_node = xmlNewDocRawNode(doc, NULL, "scene", NULL);
	snprintf(tmp, 255, "%d", j);
	xmlSetProp(sc_node, "number", tmp);
	if (!st) {
	    xmlAddChild(rootnode, sc_node);
	    node = xmlNewText("\n");
	    xmlAddChild(rootnode, node);
	    continue;
	}
	xmlSetProp(sc_node, "name", get_scene_name(j));
	node = xmlNewText("\n");
	xmlAddChild(sc_node, node);
	xmlAddChild(rootnode, sc_node);
	node = xmlNewText("\n");
	xmlAddChild(rootnode, node);

	for (i=0; i<S_SIZE; i++) {
	    node = xmlNewDocRawNode(doc, NULL, "parameter", NULL);
	    snprintf(tmp, 255, "%g", st->value[i]);
	    xmlSetProp(node, "name", s_symbol[i]);
	    xmlSetProp(node, "value", tmp);
	    xmlAddChild(sc_node, node);
	    node = xmlNewText("\n");
	    xmlAddChild(sc_node, node);
	}
    }
    xmlSaveFile(filename, doc);
    xmlFreeDoc(doc);
}

void s_startElement(void *user_data, const xmlChar *name,
                    const xmlChar **attrs);

void s_load_session_from_ui (GtkWidget *w, gpointer user_data)
{
    GtkFileSelection *file_selector = (GtkFileSelection *) user_data;

    s_load_session(gtk_file_selection_get_filename (GTK_FILE_SELECTION
                                                (file_selector)));
}
    
void s_load_session (const char *fname)
{
    xmlSAXHandlerPtr handler;
    int scene = -1;

    unset_scene_buttons ();

    if (fname) {
	filename = fname;
    }
    if (!filename) {
	filename = PACKAGE_DATA_DIR "/jamin/examples/default.jam";
    }
    s_update_title();

    handler = calloc(1, sizeof(xmlSAXHandler));
    handler->startElement = s_startElement;
    xmlSAXUserParseFile(handler, &scene, filename);
    s_history_add(g_strdup_printf("%s", filename));
    last_changed = S_LOAD;
    free(handler);

    if (!fname) {
	filename = NULL;
    }


    hdeq_set_xover ();
    set_EQ_curve_values ();
}

void s_startElement(void *user_data, const xmlChar *name, const xmlChar **attrs)
{
    const xmlChar **p;
    unsigned int i, found = 0;
    const char *symbol = NULL, *value = NULL;
    int *scene = (int *)user_data;

    if (!strcmp(name, "jam-param-list")) {
	return;
    }

    if (!strcmp(name, "scene")) {
	const char *sname = NULL;

	for (p=attrs; p && *p; p+=2) {
	    if (!strcmp(*p, "name")) {
		sname = *(p+1);
	    } else if (!strcmp(*p, "number")) {
		*scene = atoi(*(p+1));
	    }
	}

	if (sname && *scene > -1) {
	    set_scene(*scene);
	    set_scene_name(*scene, sname);
	}

	return;
    }

    /* Check its a parameter element */
    if (strcmp(name, "parameter")) {
	fprintf(stderr, "Unhandled element: %s\n", name);
	return;
    }

    /* Find the name and value attributes */
    for (p=attrs; p && *p; p+=2) {
	if (!strcmp(*p, "name")) {
	    symbol = *(p+1);
	} else if (!strcmp(*p, "value")) {
	    value = *(p+1);
	}
    }

    /* Find the matching symbol, this is horribly inefficient */
    for (i=0; i<S_SIZE && !found; i++) {
	if (!strcmp(symbol, s_symbol[i])) {
	    if (*scene == -1) {
		s_value[i] = atof(value);
		suppress_feedback++;
		s_set_events(i, s_value[i]);
		suppress_feedback--;
	    } else {
		s_state *st = get_scene(*scene);
		if (st) {
		    st->value[i] = atof(value);
		} else {
		    fprintf(stderr, "Bad scene number %d\n", *scene);
		}
	    }
	    //printf("load %s = %g\n", symbol, s_value[i]);
	    found = 1;
	    break;
	}
    }
    if (!found) {
	fprintf(stderr, "Unknown symbol: %s\n", *p);
    }
}

void s_crossfade(const int nframes)
{
    unsigned int i;

    for (i=0; i<S_SIZE; i++) {
        if (s_duration[i] != 0) {
/* debug crap if (i == S_IN_GAIN) printf("%d\t%f\n", s_duration[i], s_value[i]); */
            s_duration[i] -= nframes;
            if (s_duration[i] > nframes) {
                s_value[i] += ((float)nframes / (float)s_duration[i]) *
                                (s_target[i] - s_value[i]);
            } else {
                s_value[i] = s_target[i];
                s_duration[i] = 0;
            }
	    s_changed[i] = 1;
        }
    }
}

void s_crossfade_ui()
{
    unsigned int i;

    suppress_feedback++;
    for (i=0; i<S_SIZE; i++) {
	if (s_changed[i]) {
	    s_changed[i] = 0;
	    s_set_events(i, s_value[i]);
	}
    }
    suppress_feedback--;
}

int s_have_filename()
{
    return (filename != NULL);
}

void s_update_title()
{
    char *title;
    char *base;
    char *tmp;

    tmp = strdup(filename);
    base = basename(tmp);
    title = g_strdup_printf(PACKAGE " - %s - " VERSION, base);
    free(tmp);
    gtk_window_set_title ((GtkWindow *) main_window, title);
    free(title);
}

void s_set_filename(const char *fname)
{
    filename = fname;
}

/* vi:set ts=8 sts=4 sw=4: */
