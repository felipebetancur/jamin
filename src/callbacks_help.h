char general_help[] = {
"    JAMin is the JACK Audio Mastering interface.\n\n\
    Web site: <http://jamin.sourceforge.net>\n\n\
    JAMin is designed to perform professional audio mastering of any number \
of input streams.  It consists of a number of tools to do this: a 1024 band \
hand drawn EQ with modifiable parametric controls, a 31 band graphic EQ, \
3 band compressor, 3 band stereo width control, lookahead limiter, boost, and \
a number of other features.\n\n\
    Steve Harris is the JAMin principle author and team leader.\n\n\
    SourceForge CVS developers, in alphabetical order:\n\n\
    Jan Depner\n\
    Steve Harris\n\
    Jack O'Quin\n\
    Ron Parker\n\
    Patrick Shirkey\n\n\
    Others have contributed significant ideas or patches.  Alexandre \
Prokoudine helped define the translation infrastructure and provided our \
Russian translation.\n\n\
    JAMin is released under the GNU General Public License and is copyright \
(c) 2003 J. Depner, S. Harris, J. O'Quin, R. Parker, and P. Shirkey. \n\
    This program is free software; you can redistribute it and/or modify it \
under the terms of the GNU General Public License as published by the Free \
Software Foundation; either version 2 of the License, or (at your option) any \
later version.\n\
    This program is distributed in the hope that it will be useful, but \
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or \
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for \
more details.\n\
    You should have received a copy of the GNU General Public License along \
with this program; if not, write to the Free Software Foundation, Inc., 675 \
Mass Ave, Cambridge, MA 02139, USA.\n"
};


char help_help[] = {
"    Context specific help can be obtained by moving the mouse pointer into \
one of the tool areas (compressor, EQ, limiter, input, etc) and pressing \
<Shift><F1>.  Pressing <Shift><F1> while the cursor is over the Help button \
will cause the general JAMin help message to be displayed.\n"
};


char hdeq_help[] = {
"    The hand drawn EQ (HDEQ) allows the user to draw the EQ curve using the \
mouse.  The curve is then splined to fill 1024 EQ bands.  There are a number \
of other options available in the HDEQ.  There are user defined notches that \
act as a parametric EQ.  There are also crossover controls that will allow \
the user to change the compressor crossover points.  The following is a quick \
guide to using the HDEQ:\n\n\
    In the background window - left click and release to begin drawing the \
curve.  Left click again to end the curve.  You can define any portion of the \
curve, you don't have to define the entire curve.  Drawing is only allowed \
from left to right.\n\n\
    Over the crossover bar handles - left click and hold to drag the \
crossover bars.\n\n\
    Over the notch handles - left click and hold to drag the notch center or \
cuttoff frequency and gain.  <Ctrl>-left click will reset the notch to 0.\n\n\
    Over the notch width handle - left click and hold to widen or narrow the \
notches, except the high and low cutoff notches which have no width handles.\n"
};


char crossover_help[] = {
"    The adjustable crossover is used to split the entire audible range into \
three sections.  It applies to the compressors and stereo widths. If the \
crossovers are set to 500Hz and 5KHz then the first stereo width control and \
compressor works for 25-500Hz, the second for 500Hz-5KHz, and the last for \
5KHz-20KHz.  The crossover has no effect on the HDEQ.  However, the crossover \
bars that are visible in the HDEQ serve as a visual reference.\n"
};


char quit_help[] = {
"    Intuitively obvious to the most casual observer\n"
};


char geq_help[] = {
"    The graphic EQ can be used to set gain for specific bands of the audio \
spectrum.  The center of the band is annotated at the bottom of each fader.\n"
};


char input_help[] = {
"    Input help not yet available.\n"
};


char spectrum_help[] = {
"    Spectrum help not yet available.\n"
};


char comp_curve_help[] = {
"    Comp curve help not yet available.\n"
};


char comp_help[] = {
"    Comp help not yet available.\n"
};


char stereo_help[] = {
"    Stereo help not yet available.\n"
};


char gain_meter_help[] = {
"    Gain meter help not yet available.\n"
};


char limiter_help[] = {
"    Limiter help not yet available.\n"
};


char boost_help[] = {
"    Boost help not yet available.\n"
};


char output_help[] = {
"    Output help not yet available.\n"
};


char eq_options_help[] = {
"    EQ options help not yet available.\n"
};


char spectrum_options_help[] = {
"    Spectrum options help not yet available.\n"
};


char crossover_label_help[] = {
"    Crossover label help not yet available.\n"
};


char status_label_help[] = {
"    Status label help not yet available.\n"
};


char transport_controls_help[] = {
"    Transport controls help not yet available.\n"
};


char bypass_help[] = {
"    Bypass help not yet available.\n"
};


char load_help[] = {
"    Load help not yet available.\n"
};


char save_help[] = {
"    Save help not yet available.\n"
};


char undo_help[] = {
"    Undo help not yet available.\n"
};


char scenes_help[] = {
"    Scenes help not yet available.\n"
};
