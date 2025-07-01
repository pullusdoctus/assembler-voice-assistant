#pragma once

#include <cstdint>
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

int increase_font_size(int* font_size);
int decrease_font_size(int* font_size);
void toggle(uint8_t* flag);
void _invert_color(const GdkRGBA* input, GdkRGBA* output);

#ifdef __cplusplus
}
#endif
