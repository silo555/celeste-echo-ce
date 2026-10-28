#include "practice.h"
#include "echo_records.h"
#include "classic.h"
#include "emu.h"
#include <graphx.h>
#include <fontlibc.h>
#include <keypadc.h>
#include <cstdlib>

bool practice_mode = true;
static bool pending_retry;
static bool pending_keep_result;
static bool launch_latch = true;
static bool previous_enter = true;
static bool previous_jump = true;
static bool previous_menu = true;
static int previous_direction;
static unsigned repeat_ticks;
static gfx_sprite_t *preview;
static unsigned preview_level = echo::levels;

void practice_init() {
    echo_records_load();
    echo_state.open_menu();
    ui_mode(true);
}
bool practice_menu_active() { return echo_state.menu; }
void practice_request_retry(bool keep_result) {
    pending_retry = true;
    pending_keep_result = keep_result;
}
void practice_apply_pending() {
    if(!pending_retry) return;
    pending_retry = false;
    echo_state.begin(pending_keep_result);
    ui_mode(false);
    // Room destruction happens outside every object's update/draw stack.
    practice_reset_room(echo_state.selected);
}
bool practice_controls() {
    bool enter = kb_IsDown(kb_KeyEnter);
    bool jump = kb_IsDown(kb_Key2nd);
    bool menu = kb_IsDown(kb_KeyGraph) || kb_IsDown(kb_KeyWindow);
    bool up = kb_IsDown(kb_KeyUp), down = kb_IsDown(kb_KeyDown);
    bool enter_pressed = enter && !previous_enter;
    bool jump_pressed = jump && !previous_jump;
    bool menu_pressed = menu && !previous_menu;
    previous_enter = enter;
    previous_jump = jump;
    previous_menu = menu;
    if(launch_latch) {
        if(enter || jump || menu || up || down) return true;
        launch_latch = false;
    }
    if(echo_state.menu) {
        int direction = up == down ? 0 : up ? -1 : 1;
        if(direction != previous_direction) repeat_ticks = 0;
        if(direction && (!repeat_ticks || (repeat_ticks >= 10 && (repeat_ticks - 10) % 3 == 0))) echo_state.select(direction);
        previous_direction = direction;
        if(direction) ++repeat_ticks; else repeat_ticks = 0;
        if(enter_pressed || jump_pressed) {
            free(preview); preview = nullptr; preview_level = echo::levels;
            practice_request_retry(false);
            practice_apply_pending();
        }
        return true;
    }
    if(menu_pressed) {
        echo_state.open_menu();
        ui_mode(true);
        echo_records_save();
        return true;
    }
    if(enter_pressed) {
        practice_request_retry(false);
        practice_apply_pending();
        return true;
    }
    return false;
}
void practice_tick() { echo_state.tick(); }
void practice_on_load() {}
void practice_on_complete() {
    if(echo_state.complete()) practice_request_retry(true);
}
static void print_attempt_time(uint32_t ticks, int x, int y, uint8_t col) {
    uint32_t seconds = ticks / 30;
    print_int((int)(seconds / 60), x, y, col, 2);
    print(":"); print_int((int)(seconds % 60), 2);
    print("."); print_int((int)(ticks % 30) * 1000 / 30, 3);
}
void practice_draw_hud() {
    rectfill(4, 4, 42, 10, 0);
    print_attempt_time(echo_state.ticks, 5, 5, 7);
    if(!echo_state.result_left) return;
    if(!echo_state.had_reference) {
        rectfill(4, 12, 37, 18, 0);
        print("first pb", 5, 13, 11);
        return;
    }
    int32_t difference = echo_state.difference;
    uint32_t magnitude = (uint32_t)(difference < 0 ? -difference : difference);
    uint32_t whole = magnitude / 30;
    unsigned chars = whole < 10 ? 7 : whole < 60 ? 8 : 10;
    for(uint32_t mins = whole / 60; mins >= 10; mins /= 10) ++chars;
    rectfill(4, 12, 5 + (int)chars * 4, 18, 0);
    uint8_t col = difference < 0 ? 11 : 8;
    print(difference < 0 ? "-" : "+", 5, 13, col);
    if(whole >= 60) {
        print_int((int)(whole / 60)); print(":"); print_int((int)(whole % 60), 2);
    } else print_int((int)whole);
    print("."); print_int((int)(magnitude % 30) * 1000 / 30, 3); print("s");
}
static void make_preview() {
    if(!preview) preview = gfx_MallocSprite(128, 128);
    if(!preview || preview_level == echo_state.selected) return;
    unsigned cell_x = (echo_state.selected % 8) * 16;
    unsigned cell_y = (echo_state.selected / 8) * 16;
    for(unsigned ty = 0; ty < 16; ++ty) {
        for(unsigned tx = 0; tx < 16; ++tx) {
            uint8_t tile = mget(cell_x + tx, cell_y + ty);
            const gfx_sprite_t *sprite = atlas_tiles[tile];
            for(unsigned y = 0; y < 8; ++y) {
                for(unsigned x = 0; x < 8; ++x) preview->data[(ty * 8 + y) * 128 + tx * 8 + x] = sprite->data[y * 8 + x] & 15;
            }
        }
    }
    preview_level = echo_state.selected;
}
void practice_draw_menu() {
    gfx_FillScreen(0);
    gfx_SetTextFGColor(7); gfx_SetTextBGColor(0);
    gfx_PrintStringXY("CEleste Echo", 10, 7);
    gfx_SetColor(5); gfx_VertLine_NoClip(156, 25, 190);
    fontlib_SetForegroundColor(6); fontlib_SetCursorPosition(8, 20);
    fontlib_DrawString("select a room");
    for(unsigned i = 0; i < echo::levels; ++i) {
        unsigned y = 28 + i * 6;
        gfx_SetColor(i == echo_state.selected ? 2 : 0);
        gfx_FillRectangle_NoClip(5, y - 1, 146, 6);
        fontlib_SetForegroundColor(i == echo_state.selected ? 7 : 6);
        fontlib_SetCursorPosition(9, y);
        fontlib_DrawUInt(i + 1, 2); fontlib_DrawString("  ");
        if(i == 30) fontlib_DrawString("summit");
        else if(i == 11) fontlib_DrawString("old site");
        else { fontlib_DrawUInt((i + 1) * 100, 1); fontlib_DrawString(" m"); }
    }
    make_preview();
    if(preview) gfx_Sprite_NoClip(preview, 176, 28);
    gfx_SetTextFGColor(11); gfx_PrintStringXY("PERSONAL BEST", 170, 169);
    gfx_SetTextFGColor(7); gfx_SetTextXY(170, 184);
    uint32_t best = echo_state.records.best[echo_state.selected];
    if(best) {
        gfx_PrintUInt((unsigned)(best / (30 * 60)), 2); gfx_PrintChar(':');
        gfx_PrintUInt((unsigned)(best / 30 % 60), 2); gfx_PrintChar('.');
        gfx_PrintUInt((unsigned)(best % 30) * 1000 / 30, 3);
    } else gfx_PrintString("--:--.---");
    fontlib_SetForegroundColor(6); fontlib_SetCursorPosition(170, 202);
    fontlib_DrawString("beat a room to save its pb");
    fontlib_SetCursorPosition(9, 223);
    fontlib_DrawString("up/down: select    enter/2nd: play    clear: quit");
    fontlib_SetCursorPosition(9, 232);
    if(echo_records_save_failed()) {
        fontlib_SetForegroundColor(8); fontlib_DrawString("save failed: free archive space, then retry or quit");
    } else fontlib_DrawString("in game: enter retries    graph/window opens selector");
}
int practice_get_total_time() { return 0; }
