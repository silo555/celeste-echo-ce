#pragma once

extern bool practice_mode;

void practice_init();
bool practice_controls();
void practice_tick();
void practice_apply_pending();
void practice_request_retry(bool keep_result);
void practice_on_load();
void practice_on_complete();
void practice_draw_hud();
void practice_draw_menu();
int practice_get_total_time();
bool practice_menu_active();
