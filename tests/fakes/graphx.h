#pragma once
#include <stdint.h>
#include <stdlib.h>
struct gfx_sprite_t { uint8_t width, height, data[1]; };
#define gfx_MallocSprite(w,h) fake_malloc_sprite(w,h)
gfx_sprite_t *fake_malloc_sprite(unsigned w, unsigned h);
void gfx_FillScreen(uint8_t);
void gfx_SetColor(uint8_t);
void gfx_SetTextFGColor(uint8_t);
void gfx_SetTextBGColor(uint8_t);
void gfx_PrintStringXY(const char*, unsigned, unsigned);
void gfx_PrintString(const char*);
void gfx_SetTextXY(unsigned, unsigned);
void gfx_PrintUInt(unsigned, unsigned);
void gfx_PrintChar(char);
void gfx_VertLine_NoClip(unsigned, unsigned, unsigned);
void gfx_FillRectangle_NoClip(unsigned, unsigned, unsigned, unsigned);
void gfx_Sprite_NoClip(const gfx_sprite_t*, unsigned, unsigned);
