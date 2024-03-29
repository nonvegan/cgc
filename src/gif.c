#include "gif.h"

#include <assert.h>
#include <string.h>
#include <errno.h>

#include "y4m2.h"

int gif_dump_frame_to_canvas(GifFileType *gif_file, size_t index, Canvas *canvas)
{
    assert(index < gif_file->ImageCount);
    SavedImage *image = &gif_file->SavedImages[index];
    ColorMapObject *local_color_map = image->ImageDesc.ColorMap;

    ColorMapObject *color_map = local_color_map ? local_color_map : gif_file->SColorMap;
    assert(color_map && "Missing color map");
    assert(color_map->BitsPerPixel == 8 && "Unsupported GIF format");

    for(size_t x = 0; x < gif_file->SWidth && x < canvas->width; x++)
        for(size_t y = 0; y < gif_file->SHeight && y < canvas->height; y++) {
            GifColorType px = color_map->Colors[image->RasterBits[y * gif_file->SWidth + x]];
            canvas_fill_px(canvas, x, y, RGB(px.Red, px.Green, px.Blue));
        }
}

int gif_render_y4m2_video(const char *input_file_name,
                          const char *output_file_name,
                          float video_duration_sec,
                          size_t gif_fps)
{
    int error = 0;
    GifFileType *gif_file = DGifOpenFileName(input_file_name, &error);
    if(error || gif_file == NULL) {
        fprintf(stderr, "ERROR: Could not open GIF file %s: %s\n", input_file_name, GifErrorString(error));
        return GIF_ERROR;
    }

    if(DGifSlurp(gif_file) != GIF_OK) {
        fprintf(stderr, "ERROR: Could not load GIF file %s: %s\n", input_file_name, GifErrorString(gif_file->Error));
        return GIF_ERROR;
    }

    printf("INFO: Loaded %s GIF file, %dx%d, %d images, %d bpp\n",
            input_file_name,
            gif_file->SWidth,
            gif_file->SHeight,
            gif_file->ImageCount,
            gif_file->SColorMap->BitsPerPixel);

    // NOTE: A whole lot of assumptions to derive gif fps
    GraphicsControlBlock gcb = {0};
    assert(gif_file->ImageCount > 1 && "Can't derive fps");
    DGifExtensionToGCB(gif_file->SavedImages[1].ExtensionBlocks->ByteCount,
                       gif_file->SavedImages[1].ExtensionBlocks->Bytes,
                       &gcb);

    if(gif_fps == GIF_KEEP_FPS) {
        assert(gcb.DelayTime && "Division by zero");
        gif_fps = 100.0f / gcb.DelayTime;
    }

    if(video_duration_sec == GIF_KEEP_DURATION)
        video_duration_sec = (float) gif_file->ImageCount / gif_fps;

    Y4m2 *y4m2_handle = y4m2_open_video(output_file_name, gif_file->SWidth, gif_file->SHeight, gif_fps);
    if(y4m2_handle == NULL) {
        fprintf(stderr, "ERROR: Could not open file %s: %s\n", output_file_name, strerror(errno));
        return GIF_ERROR;
    }

    Canvas *canvas = canvas_create(gif_file->SWidth, gif_file->SHeight);

    size_t frame_count = 0;
    size_t target_frame_count = video_duration_sec * gif_fps;
    assert(target_frame_count && "Non positive frame count");

    while(frame_count < target_frame_count) {
        for(size_t i = 0; i < gif_file->ImageCount; i++) {
            gif_dump_frame_to_canvas(gif_file, i, canvas);
            y4m2_dump_canvas_frame(y4m2_handle, canvas, 1);
            if(++frame_count >= target_frame_count) break;
        }
    }

    canvas_free(canvas);
    y4m2_close_video(y4m2_handle);

    return GIF_OK;
}
