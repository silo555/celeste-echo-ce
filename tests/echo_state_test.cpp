#include "echo_state.h"
#include <assert.h>
#include <stdio.h>

int main() {
    echo::State s;
    assert(s.menu && s.selected == 0);
    s.select(-1); assert(s.selected == 30);
    s.select(1); assert(s.selected == 0);
    for(unsigned i = 0; i < 31; ++i) { assert(s.selected == i); s.select(1); }
    assert(s.selected == 0);
    s.tick(); assert(!s.ticks);
    puts("PASS all 31 rooms, wraparound, and no menu time");

    s.selected = 11; s.begin();
    for(unsigned i = 0; i < 60; ++i) s.tick();
    assert(s.complete() && s.records.best[11] == 60 && !s.had_reference && s.improved);
    assert(!s.complete());
    s.begin(true);
    assert(!s.ticks && s.result_left == 30 && s.records.last_level == 11);
    for(unsigned i = 0; i < 45; ++i) s.tick();
    assert(s.complete() && s.difference == -15 && s.had_reference && s.improved);
    assert(s.records.best[11] == 45 && s.records.best[12] == 0);
    s.begin(true);
    for(unsigned i = 0; i < 29; ++i) s.tick();
    assert(s.result_left == 1);
    s.tick(); assert(!s.result_left);
    for(unsigned i = 0; i < 30; ++i) s.tick();
    assert(s.complete() && s.difference == 15 && !s.improved && s.records.best[11] == 45);
    s.begin();
    for(unsigned i = 0; i < 45; ++i) s.tick();
    assert(s.complete() && !s.improved && !s.difference);
    s.begin(); assert(!s.ticks && !s.result_left && s.records.best[11] == 45);
    puts("PASS first/faster/slower/tied PB, comparison before replacement, loop/reset and 30-tick popup");

    s.selected = 30; s.begin();
    s.tick(); assert(s.complete() && s.records.best[30] == 1);
    s.open_menu(); assert(!s.complete());
    uint8_t bytes[echo::save_bytes];
    echo::encode(s.records, 123, bytes);
    echo::Records restored;
    uint32_t generation = 0;
    assert(echo::decode(bytes, sizeof bytes, restored, generation));
    assert(generation == 123 && restored.last_level == 30 && restored.best[11] == 45 && restored.best[30] == 1);
    for(unsigned i = 0; i < sizeof bytes; ++i) {
        for(unsigned bit = 0; bit < 8; ++bit) {
            bytes[i] ^= 1u << bit;
            assert(!echo::decode(bytes, sizeof bytes, restored, generation));
            assert(generation == 123 && restored.best[11] == 45);
            bytes[i] ^= 1u << bit;
        }
        assert(!echo::decode(bytes, i, restored, generation));
    }
    assert(echo::newer(1, 0xffffffffUL) && !echo::newer(5, 6));
    puts("PASS summit, independent rooms, save roundtrip, every single-bit corruption, truncation and serial wrap");
}
