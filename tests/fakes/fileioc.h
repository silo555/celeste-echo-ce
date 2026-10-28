#pragma once
#include <stdint.h>
#include <stddef.h>
uint8_t ti_Open(const char*, const char*);
int ti_Close(uint8_t);
uint16_t ti_GetSize(uint8_t);
size_t ti_Read(void*,size_t,size_t,uint8_t);
size_t ti_Write(const void*,size_t,size_t,uint8_t);
int ti_SetArchiveStatus(bool,uint8_t);
