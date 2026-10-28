#include <keypadc.h>
#include <graphx.h>
#include <tice.h>
#include "emu.h"
#include "echo_records.h"

int main(void)
{
    gfx_Begin();
    init();
    timer_Enable(1, TIMER_32K, TIMER_NOINT, TIMER_UP);
    do {
        update();
    } while(!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyDel));
    echo_records_save();
    gfx_End();
    return 0;
}
