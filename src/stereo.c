#include "process.h"
#include "state.h"

void stereo_cb(int id, float value);

void bind_stereo()
{
    int i;

    for (i = 0; i < 3; i++) {
	s_set_callback(S_STEREO_WIDTH(i), stereo_cb);
	process_set_stereo_width(i, 0.0f);
    }
}

void stereo_cb(int id, float value)
{
    int band = id - S_STEREO_WIDTH(0);

    process_set_stereo_width(band, value);
}
