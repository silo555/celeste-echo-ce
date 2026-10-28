#include "echo_records.h"
#include <fileioc.h>

echo::State echo_state;
static const char *const names[] = {"CEchoA", "CEchoB"};
static uint32_t serial;
static uint8_t current_slot;
static bool failed;

void echo_records_load() {
    for(uint8_t slot = 0; slot < 2; ++slot) {
        uint8_t file = ti_Open(names[slot], "r");
        if(!file) continue;
        uint8_t bytes[echo::save_bytes];
        echo::Records records;
        uint32_t generation;
        bool valid = ti_GetSize(file) == sizeof bytes &&
            ti_Read(bytes, sizeof bytes, 1, file) == 1 &&
            echo::decode(bytes, sizeof bytes, records, generation);
        ti_Close(file);
        if(valid && (!serial || echo::newer(generation, serial))) {
            echo_state.records = records;
            serial = generation;
            current_slot = slot;
        }
    }
    echo_state.selected = echo_state.records.last_level;
}

bool echo_records_save() {
    if(!echo_state.dirty) return !failed;
    uint8_t bytes[echo::save_bytes];
    uint32_t next = serial + 1;
    if(!next) next = 1;
    echo::encode(echo_state.records, next, bytes);
    uint8_t slot = 1 - current_slot;
    uint8_t file = ti_Open(names[slot], "w");
    if(!file) { failed = true; return false; }
    bool ok = ti_Write(bytes, sizeof bytes, 1, file) == 1;
    if(ok) ok = ti_SetArchiveStatus(true, file) != 0;
    ti_Close(file);
    failed = !ok;
    if(ok) {
        serial = next;
        current_slot = slot;
        echo_state.dirty = false;
    }
    return ok;
}

bool echo_records_save_failed() { return failed; }
