#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>

#include "config.h"
#include "state.h"

/* The smallest value that counts as a changei, should be aproximatly
 * epsilon+delta */
#define MIN_CHANGE (FLT_EPSILON + FLT_EPSILON)

typedef struct {
    char *description;
    float value[S_SIZE];
} s_state;

float                   s_value[S_SIZE];
static float            s_target[S_SIZE];
static int              s_duration[S_SIZE];
static GtkAdjustment   *s_adjustment[S_SIZE];
static s_callback_func  s_callback[S_SIZE];

static s_state       *last_state = NULL;
static int	      last_changed = S_NONE;
static char          *last_description = NULL;

static GList         *history = NULL;
static GList         *undo_pos = NULL;

static int suppress_feedback = 0;

static void s_history_add(const char *description);
static void s_restore_state(s_state *state);
static void s_set_events(int id, float value);

void state_init()
{
    unsigned int i;

    for (i=0; i<S_SIZE; i++) {
	s_value[i] = 0.0f;
	s_target[i] = 0.0f;
	s_adjustment[i] = NULL;
	s_callback[i] = NULL;
    }
    s_history_add("Initial state");
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
    if (suppress_feedback) {
	return;
    }
    assert(id >= 0 && id < S_SIZE);

    if (last_changed != S_NONE && last_changed != id) {
	s_history_add(g_strdup_printf("%s = %f", s_description[last_changed],
		      s_value[last_changed]));
    }
    if (last_description) {
	s_history_add(last_description);
	last_description = NULL;
    }
    s_value[id] = value;
    if (value - MIN_CHANGE < last_state->value[id] &&
	value + MIN_CHANGE > last_state->value[id]) {
	last_changed = S_NONE;
    } else {
	last_changed = id;
    }
    if (s_callback[id]) {
	(*s_callback[id])(id, value);
    }
}

void s_set_value(int id, float value, int duration)
{
    s_set_value_ui(id, value);
    s_duration[id] = duration;
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

static void s_history_add(const char *description)
{
    s_state *ns;

    ns = malloc(sizeof(s_state));
    ns->description = (char *)description;
    memcpy(ns->value, s_value, S_SIZE * sizeof(float));
    history = g_list_append(history, ns);
    undo_pos = g_list_last(history);
    printf("add %s\n", description);
    last_state = ns;
}

void s_undo() {
    void set_EQ_curve_values ();


    if (!undo_pos) {
	return;
    }
    s_restore_state((s_state *)undo_pos->data);
    undo_pos = g_list_previous(undo_pos);

    set_EQ_curve_values ();
}

static void s_restore_state(s_state *state)
{
    int i;

    printf("restore %s\n", state->description);
    suppress_feedback++;
    for (i=0; i<S_SIZE; i++) {
	s_value[i] = state->value[i];
	s_set_events(i, s_value[i]);
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

void s_save_session (GtkWidget *w, gpointer user_data)
{
    const gchar      *filename;
    GtkFileSelection *file_selector = (GtkFileSelection *) user_data;
    xmlDocPtr doc;
    xmlNodePtr rootnode, node;
    unsigned int i;
    char tmp[256];

    filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (file_selector));

    xmlSetCompressMode(5);
    doc = xmlNewDoc("1.0");
    rootnode = xmlNewDocRawNode(doc, NULL, "jam-param-list", NULL);
    xmlSetProp(rootnode, "version", VERSION);
    xmlDocSetRootElement(doc, rootnode);
    node = xmlNewText("\n");
    xmlAddChild(rootnode, node);
    for (i=0; i<S_SIZE; i++) {
	node = xmlNewDocRawNode(doc, NULL, "parameter", NULL);
	snprintf(tmp, 255, "%g", s_value[i]);
	xmlSetProp(node, "name", s_symbol[i]);
	xmlSetProp(node, "value", tmp);
	xmlAddChild(rootnode, node);
	node = xmlNewText("\n");
	xmlAddChild(rootnode, node);
    }
    xmlSaveFile(filename, doc);
    xmlFreeDoc(doc);
}

void s_startElement(void *user_data, const xmlChar *name,
                    const xmlChar **attrs);

void s_load_session (GtkWidget *w, gpointer user_data)
{
    const gchar      *filename;
    GtkFileSelection *file_selector = (GtkFileSelection *) user_data;
    xmlSAXHandlerPtr  handler;

    filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION
                                                (file_selector));

    handler = calloc(1, sizeof(xmlSAXHandler));
    handler->startElement = s_startElement;
    xmlSAXUserParseFile(handler, NULL, filename);
    last_description = g_strdup_printf("Loaded %s", filename);
    last_changed = S_NONE;
    free(handler);
}

void s_startElement(void *user_data, const xmlChar *name, const xmlChar **attrs)
{
    const xmlChar **p;
    unsigned int i, found = 0;
    const char *symbol = NULL, *value = NULL;


    /* Check its the right element */
    if (strcmp(name, "parameter")) {
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
	    s_value[i] = atof(value);
	    suppress_feedback++;
	    s_set_events(i, s_value[i]);
	    suppress_feedback--;
	    //printf("load %s = %g\n", symbol, s_value[i]);
	    found = 1;
	}
    }
    if (!found) {
	fprintf(stderr, "Unknown symbol: %s\n", *p);
    }
}

/* vi:set ts=8 sts=4 sw=4: */
