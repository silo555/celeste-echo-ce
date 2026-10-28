#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <array>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "graphx.h"
#include "keypadc.h"
#include "atlas.h"
#include "atlas.c"
#include "room_data.h"

static const uint8_t font[] = {
#include "font.inc"
};
static std::array<uint8_t,320*240> canvas{};
static uint8_t fg, ink;
static unsigned cx,cy;
static bool graphics_font;
static std::array<bool,6> keys{};
static std::vector<unsigned> loaded;
static std::string output;
static unsigned rows, highlighted;
static bool menu_mode;
static unsigned read16(const uint8_t *p) { return p[0] + 256u*p[1]; }
static unsigned read24(const uint8_t *p) { return read16(p) + 65536u*p[2]; }
static void pixel(unsigned x,unsigned y,uint8_t color) { assert(x<320 && y<240); canvas[y*320+x]=color&15; }
static void glyph(char letter) {
    unsigned ch=(unsigned char)letter;
    assert(ch>=font[3] && ch<font[3]+font[2]);
    unsigned index=ch-font[3], width=font[read24(font+4)+index];
    unsigned bitmap=read16(font+read24(font+7)+index*2);
    unsigned scale=graphics_font?2:1;
    for(unsigned y=0;y<font[1];++y) for(unsigned x=0;x<width;++x) {
        if(font[bitmap+y] & (128u>>x)) for(unsigned sx=0;sx<scale;++sx) pixel(cx+x*scale+sx,cy+y,fg);
    }
    cx+=graphics_font?8:width;
}
static std::string number(unsigned n,unsigned width) { std::ostringstream s; s<<std::setfill('0')<<std::setw((int)width)<<n; return s.str(); }
static void string(const char *s) { output+=s; while(*s) glyph(*s++); }
gfx_sprite_t *fake_malloc_sprite(unsigned w,unsigned h) { auto p=(gfx_sprite_t*)malloc(2+w*h); assert(p); p->width=w; p->height=h; return p; }
void gfx_FillScreen(uint8_t col) { canvas.fill(col); }
void gfx_SetColor(uint8_t col) { ink=col; }
void gfx_SetTextFGColor(uint8_t col) { fg=col; }
void gfx_SetTextBGColor(uint8_t) {}
void gfx_SetTextXY(unsigned x,unsigned y) { cx=x;cy=y;graphics_font=true; }
void gfx_PrintString(const char *s) { string(s); }
void gfx_PrintStringXY(const char *s,unsigned x,unsigned y) { gfx_SetTextXY(x,y);string(s); }
void gfx_PrintUInt(unsigned n,unsigned width) { string(number(n,width).c_str()); }
void gfx_PrintChar(char c) { char s[]={c,0};string(s); }
void gfx_VertLine_NoClip(unsigned x,unsigned y,unsigned h) { for(unsigned i=0;i<h;++i) pixel(x,y+i,ink); }
void gfx_FillRectangle_NoClip(unsigned x,unsigned y,unsigned w,unsigned h) {
    if(x==5 && w==146) { ++rows; if(ink==2) ++highlighted; }
    for(unsigned iy=0;iy<h;++iy) for(unsigned ix=0;ix<w;++ix) pixel(x+ix,y+iy,ink);
}
void gfx_Sprite_NoClip(const gfx_sprite_t *p,unsigned x,unsigned y) {
    for(unsigned iy=0;iy<p->height;++iy) for(unsigned ix=0;ix<p->width;++ix) pixel(x+ix,y+iy,p->data[iy*p->width+ix]);
}
void fontlib_SetForegroundColor(uint8_t col) { fg=col; }
void fontlib_SetCursorPosition(unsigned x,unsigned y) { cx=x;cy=y;graphics_font=false; }
void fontlib_DrawString(const char *s) { string(s); }
void fontlib_DrawUInt(unsigned n,unsigned width) { string(number(n,width).c_str()); }
bool kb_IsDown(unsigned key) { return keys.at(key); }
void practice_reset_room(unsigned index) { assert(index<31); loaded.push_back(index); }
void ui_mode(bool menu) { menu_mode=menu; }
void rectfill(int x0,int y0,int x1,int y1,uint8_t col) { assert(x0>=0 && y0>=0 && x1<128 && y1<128); gfx_SetColor(col);gfx_FillRectangle_NoClip(x0,y0,x1-x0+1,y1-y0+1); }
void print(const char *s) { fontlib_DrawString(s); }
void print(const char *s,int x,int y,uint8_t col) { fg=col;fontlib_SetCursorPosition(x-1,y);print(s); }
void print_int(int n,int width) { assert(n>=0);fontlib_DrawUInt((unsigned)n,(unsigned)width); }
void print_int(int n) { print_int(n,1); }
void print_int(int n,int x,int y,uint8_t col,int width) { fg=col;fontlib_SetCursorPosition(x-1,y);print_int(n,width); }
void print_int(int n,int x,int y,uint8_t col) { print_int(n,x,y,col,1); }

struct Var { std::vector<uint8_t> bytes; bool archived=false; };
struct Handle { std::string name; size_t pos; };
static std::map<std::string,Var> vars;
static std::map<uint8_t,Handle> handles;
static uint8_t next_id=1;
static unsigned writes,archives;
static bool fail_open,fail_write,fail_archive;
uint8_t ti_Open(const char *name,const char *mode) {
    assert(std::string(name)=="CEchoA" || std::string(name)=="CEchoB");
    if(mode[0]=='w') { if(fail_open)return 0;vars[name]=Var{}; }
    else if(!vars.count(name))return 0;
    uint8_t id=next_id++;if(!next_id)next_id=1;assert(!handles.count(id));handles[id]={name,0};return id;
}
int ti_Close(uint8_t id) { return (int)handles.erase(id); }
uint16_t ti_GetSize(uint8_t id) { return (uint16_t)vars[handles.at(id).name].bytes.size(); }
size_t ti_Read(void *data,size_t size,size_t count,uint8_t id) {
    auto &h=handles.at(id);auto &v=vars.at(h.name).bytes;
    size_t n=std::min(count,(v.size()-h.pos)/size);
    if(n) { memcpy(data,v.data()+h.pos,n*size); }
    h.pos+=n*size;
    return n;
}
size_t ti_Write(const void *data,size_t size,size_t count,uint8_t id) {
    ++writes;auto &h=handles.at(id);auto &v=vars.at(h.name).bytes;
    size_t n=fail_write?size*count/2:size*count;v.resize(h.pos+n);memcpy(v.data()+h.pos,data,n);h.pos+=n;return fail_write?0:count;
}
int ti_SetArchiveStatus(bool archive,uint8_t id) { ++archives;if(fail_archive)return 0;vars[handles.at(id).name].archived=archive;return 1; }

#include "echo_records.cpp"
#include "practice_loop.cpp"

static void release() { keys.fill(false);practice_controls(); }
static void press(unsigned key) { keys[key]=true;practice_controls(); }
static void complete(unsigned ticks) { for(unsigned i=0;i<ticks;++i)practice_tick();practice_on_complete();practice_apply_pending(); }
static void reboot() {
    assert(handles.empty());
    for(auto i=vars.begin();i!=vars.end();) { if(!i->second.archived)i=vars.erase(i);else ++i; }
    echo_state=echo::State{};serial=0;current_slot=0;failed=false;echo_records_load();
}
static void screenshot() {
    static const uint8_t palette[16][3]={{0,0,0},{29,43,83},{126,37,83},{0,135,81},{171,82,54},{95,87,79},{194,195,199},{255,241,232},{255,0,77},{255,163,0},{255,236,39},{0,228,54},{41,173,255},{131,118,156},{255,119,168},{255,204,170}};
    FILE *file=fopen("build/selector-layout.ppm","wb");assert(file);fputs("P6\n320 240\n255\n",file);
    for(auto color:canvas) { fwrite(palette[color],3,1,file); }
    fclose(file);
}
int main() {
    keys[kb_KeyEnter]=true;practice_init();practice_controls();assert(echo_state.menu && loaded.empty());
    release();press(kb_KeyUp);assert(echo_state.selected==30);release();press(kb_KeyDown);assert(echo_state.selected==0);
    release();echo_state.selected=11;press(kb_KeyEnter);assert(!echo_state.menu && !menu_mode && loaded.back()==11);
    practice_controls();assert(loaded.size()==1);release();
    complete(30);assert(echo_state.records.best[11]==30 && !echo_state.had_reference && !echo_state.ticks && loaded.back()==11);
    complete(45);output.clear();practice_draw_hud();assert(output.find("+0.500s")!=std::string::npos && fg==8);
    complete(15);output.clear();practice_draw_hud();assert(output.find("-0.500s")!=std::string::npos && fg==11 && echo_state.records.best[11]==15);
    complete(15);output.clear();practice_draw_hud();assert(output.find("+0.000s")!=std::string::npos && fg==8);
    for(unsigned i=0;i<30;++i) { practice_tick(); }
    output.clear();practice_draw_hud();assert(output.find('s')==std::string::npos);
    unsigned resets=(unsigned)loaded.size();release();press(kb_KeyEnter);practice_controls();assert(loaded.size()==resets+1 && !echo_state.ticks);
    assert(writes==0 && archives==0);
    release();press(kb_KeyGraph);assert(echo_state.menu && menu_mode && writes==1 && archives==1);
    rows=highlighted=0;output.clear();practice_draw_menu();assert(rows==31 && highlighted==1 && output.find("00:00.500")!=std::string::npos);screenshot();
    puts("PASS actual startup latch, all-room selector layout/PB, safe room loop, retries, first/faster/slower/tied HUD and 30-tick expiry");
    puts("PASS no flash writes between attempts; Graph opens selector and archives records");

    reboot();assert(echo_state.records.best[11]==15 && echo_state.selected==11);
    echo_state.records.best[11]=10;echo_state.dirty=true;fail_write=true;assert(!echo_records_save());fail_write=false;reboot();assert(echo_state.records.best[11]==15);
    echo_state.records.best[11]=10;echo_state.dirty=true;fail_archive=true;assert(!echo_records_save());fail_archive=false;reboot();assert(echo_state.records.best[11]==15);
    echo_state.records.best[11]=10;echo_state.dirty=true;fail_open=true;assert(!echo_records_save());fail_open=false;assert(echo_records_save());
    vars.at("CEchoA").bytes[20]^=1;reboot();assert(echo_state.records.best[11]==15);
    puts("PASS actual isolated AppVars, archived PB reboot, partial write/archive/open failure, retry and corrupt-newest recovery");
    unsigned old_writes=writes;
    for(unsigned i=0;i<31;++i) {
        echo_state.open_menu();release();echo_state.selected=i;press(kb_KeyEnter);assert(loaded.back()==i);
        release();practice_tick();practice_on_complete();practice_apply_pending();assert(loaded.back()==i && !echo_state.menu && !echo_state.ticks);
    }
    assert(writes==old_writes && handles.empty());
    puts("PASS starting and completing every room including summit loops the selected room without archive stalls");
    free(preview);preview=nullptr;
}
