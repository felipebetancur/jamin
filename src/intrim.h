#ifndef INTRIM_H
#define INTRIM_H

void bind_intrim();
void in_meter_value(float amp[]);
void out_meter_value(float amp[]);
void update_pan_label(float balance);

extern float in_gain[];
extern float in_trim_gain;
extern float in_pan_gain[];

#endif
