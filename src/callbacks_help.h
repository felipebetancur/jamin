/*
 *  Copyright (C) 2003 Jan C. Depner
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  $Id: callbacks_help.h,v 1.18 2004/01/05 22:21:51 jdepner Exp $
 */

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


char prerequisites_help[] = {
"    JAMin would not be functional without many other open source packages \
and we thought that it would only be right to list them here:\n\n\
    JACK - JACK Audio Connection Kit\n\
    http://jackit.sourceforge.net\n\n\
    SWH plugins - Steve Harris' LADSPA plugins\n\
    http://plugin.org.uk\n\n\
    LADSPA - Linux Audio Developer's Simple Plugin API\n\
    http://www.ladspa.org\n\n\
    FFTW - Fastest Fourier Transform in the West\n\
    http://www.fftw.org\n\n\
    libsndfile - Erik de Castro Lopo's sound file I/O\n\
    http://www.zip.com.au/~erikd/libsndfile\n\n\
    GTK - The GIMP Toolkit\n\
    http://www.gtk.org\n\n\
    ALSA - Advanced Linux Sound Architecture\n\
    http://www.alsa-project.org\n\n\
Many thanks to all of the authors of these packages!\n"
};


char help_help[] = {
"    Context specific help can be obtained by moving the mouse pointer into \
one of the tool areas (compressor, EQ, limiter, input, etc) and pressing \
<Shift><F1>.\n"
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


char geq_help[] = {
"    The graphic EQ (GEQ) can be used to set gain for specific bands of the \
audio spectrum.  The center of the band is annotated at the bottom of each \
fader.  Setting a fader in the GEQ will override all changes made in the HDEQ \
and cause that curve to be redrawn.  It will not override the parametric \
notch settings in the HDEQ.\n"
};


char input_help[] = {
"    The input section allows you to set the input gain to the JAMin system.  \
You can also pan the input left or right.\n"
};


char spectrum_help[] = {
"    Spectrum help not yet available.\n"
};


char comp_curve_help[] = {
"    The compressor curves show a graphical representation of the compression \
for each compressor band.  The bands are defined by the crossover that can \
be set using the crossover faders or the crossover bars in the HDEQ.  The \
X-axis shows the input in db while the Y-axis shows the output in db.  The \
scale is from -60 to 0 in X and -60 to +30 in Y.\n"
};


char comp_help[] = {
"    The compressors allow you to set compression parameters for each \
compressor band.  The bands are defined by the crossover that can be set \
using the crossover faders or the crossover bars in the HDEQ.  The parameters \
are, from left to right:\n\n\
    A - attack in milliseconds\n\
    R - release in milliseconds\n\
    T - threshold in db\n\
    r - compression ratio (N:1)\n\
    K - knee (0.0 [hard] to 1.0 [soft])\n\
    M - makeup gain in db\n\
    AM - automatic makeup gain computation\n\n\
    A full explanation of the use of these parameters is covered in the user \
manual.\n"
};


char stereo_help[] = {
"    The stereo width controls define the apparent 'wideness' of the stereo \
signal for each of the three bands.  The bands are defined by the crossovers \
that can be set using the crossover faders or the crossover bars in the \
HDEQ.  More negative values decrease the 'width' while positive values \
increase the 'width'.\n"
};


char stereo_balance_help[] = {
"    The stereo balance controls allow the user to adjust the balance for the \
set of frequencies defined by the crossover locations.  That is, the balance \
for each of the three bands, low, mid, and high, can be shifted left or \
right.   The bands are defined by the crossovers that can be set using the \
crossover faders or the crossover bars in the HDEQ.\n"
};


char limiter_help[] = {
"    The lookahead limiter is a brickwall limiter that will not allow the \
output to exceed the set level.  It \'looks ahead\' by the specified amount \
in order to make a smooth transition as it nears the limit level.\n"
};


char boost_help[] = {
"    Boost help not yet available.\n"
};


char output_help[] = {
"    Allows you to decrease the output level.  The upper level is 0dB.\n"
};


char eq_options_help[] = {
"    These allow you to set the upper and lower gain values for both the HDEQ \
and the GEQ.\n"
};


char spectrum_options_help[] = {
"    This allows you to set the input to the spectrum computation for both \
the Spectrum window and the HDEQ.  The default is Post EQ.  The other options \
are Pre EQ, Post compressor, and Output.\n"
};


char status_label_help[] = {
"    This is information on the system and transport status.\n"
};


char transport_controls_help[] = {
"    Transport controls help not yet available.\n"
};


char bypass_help[] = {
"    This button will bypass all of JAMin's functions.  The keyboard \
accelerator for this button is the 'b' key.\n"
};


char scenes_help[] = {
"    Scenes are used to save and recall an entire group of JAMin parameter \
settings during a session.  After setting all of the parameters for a \
specific section of music (verse, chorus, bridge) you can right click on a \
scene button, use the menu to 'Set' that scene, then use the text box to \
enter a name for that scene (Back Alley Fugue - verse).  You can recall these \
settings by left clicking on the scene button.  A green button means that \
the scene associated with this button is currently loaded.  A bright red \
button means that scene settings are available for this button but not \
currently loaded.  A dull red button means that no settings have been saved \
for this scene button.  A yellow warning button means that this scene was \
active but the settings have been changed.  If you want to save the settings \
you must use the 'Set' entry in the right click menu.  You can clear settings \
from a button using the right click menu and the 'Clear' entry.\n\n\
    The keyboard accelerators for the scene buttons are the number keys, \
1 through 6.  Pressing the 1 key will cause scene 1 to become active.  The \
ALT modifier can be used to assign settings to a scene button (instead of \
using the scene button menus).  Pressing <Alt>-1 will assign the current \
settings to scene button 1 (you still need to set the name).  The CTRL \
modifier can be used to clear a scene button.  Pressing <Ctrl>-1 will clear \
that scene button.\n"
};


char keys_help[] = {
"    Keyboard accelerators are available for many of the functions in \
JAMin:\n\n\
\tb\t\t-\tBypass\n\
\tSpace\t-\tToggle play and pause\n\
\tHome\t-\tPosition transport to beginning\n\
\t<\t\t-\tMove transport backwards 5 sec.\n\
\t>\t\t-\tMove transport forwards 5 sec.\n\
\t1-6\t\t-\tSelect scene 1-6\n\
\t<Ctrl>-o\t-\tOpen session file\n\
\t<Ctrl>-s\t-\tSave to current session file\n\
\t<Ctrl>-a\t-\tSave session file as new session file\n\
\t<Ctrl>-q\t-\tQuit\n\
\t<Ctrl>-u\t-\tUndo\n\
\t<Ctrl>-r\t-\tRedo\n\
\t<Ctrl>-h\t-\tGeneral help\n\
\tF1\t\t-\tSet the notebook tab to HDEQ\n\
\tF2\t\t-\tSet the notebook tab to 30 band EQ\n\
\tF3\t\t-\tSet the notebook tab to Spectrum\n\
\tF4\t\t-\tSet the notebook tab to Compressor \n \t\t\t\tcurves\n\
\tF5\t\t-\tSet the notebook tab to the EQ Options\n\
\t<Ctrl>-k\t-\tKeyboard accelerator help (this screen)\n\
\t<Ctrl>-j\t-\tAbout JAMin\n"
};
