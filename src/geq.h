#ifndef GEW_H
#define GEQ_H

#define EQ_BANDS 30

#include <gtk/gtk.h>

void bind_geq();
void geq_set_sliders(int length, float x[], float y[]);
void geq_set_range(double min, double max);
void geq_get_freqs_and_gains(float *freqs, float *gains);

extern GtkAdjustment *geqa[];

#endif
