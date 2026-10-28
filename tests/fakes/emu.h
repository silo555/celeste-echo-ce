#pragma once
#include "atlas.h"
extern const uint8_t tilemap[8192];
#define mget(x,y) tilemap[(x)+(y)*128]
void ui_mode(bool);
void rectfill(int,int,int,int,uint8_t);
void print(const char*);
void print(const char*,int,int,uint8_t);
void print_int(int);
void print_int(int,int);
void print_int(int,int,int,uint8_t);
void print_int(int,int,int,uint8_t,int);
