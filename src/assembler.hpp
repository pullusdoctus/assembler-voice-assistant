#pragma once

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

int increase_font_size(int* font_size);
int decrease_font_size(int* font_size);
void toggle_bold(uint8_t* bold_flag);

#ifdef __cplusplus
}
#endif
