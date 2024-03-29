#ifndef GIF_H_
#define GIF_H_

#include <stdlib.h>
#include <gif_lib.h>

#include "canvas.h"

#define GIF_KEEP_DURATION 0
#define GIF_KEEP_FPS 0

int gif_dump_frame_to_canvas(GifFileType *gif_file, size_t index, Canvas *canvas);

int gif_render_y4m2_video(const char *input_file_name,
                          const char *output_file_name,
                          float video_duration_sec,
                          size_t gif_fps);

#endif // GIF_H_
