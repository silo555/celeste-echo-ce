#pragma once
#include <stdint.h>
void fontlib_SetForegroundColor(uint8_t);
void fontlib_SetCursorPosition(unsigned, unsigned);
void fontlib_DrawString(const char*);
void fontlib_DrawUInt(unsigned, unsigned);
