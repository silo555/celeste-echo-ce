#pragma once
#include <stdint.h>
#include <string.h>

namespace echo {
constexpr unsigned levels = 31;
constexpr uint32_t max_ticks = 30UL * 60 * 60 * 24;
constexpr unsigned save_bytes = 4 + 1 + 4 + 1 + levels * 4 + 4;

struct Records {
    uint8_t last_level = 0;
    uint32_t best[levels]{};
};

struct State {
    Records records;
    unsigned selected = 0;
    bool menu = true;
    bool finished = false;
    bool dirty = false;
    uint32_t ticks = 0;
    uint32_t result_ticks = 0;
    int32_t difference = 0;
    bool had_reference = false;
    bool improved = false;
    uint8_t result_left = 0;

    void select(int step) {
        int next = (int)selected + step;
        while(next < 0) next += levels;
        selected = (unsigned)next % levels;
    }

    void begin(bool keep_result = false) {
        menu = false;
        finished = false;
        ticks = 0;
        if(!keep_result) result_left = 0;
        if(records.last_level != selected) {
            records.last_level = (uint8_t)selected;
            dirty = true;
        }
    }

    void tick() {
        if(menu) return;
        if(result_left) --result_left;
        if(!finished && ticks < max_ticks) ++ticks;
    }

    bool complete() {
        if(menu || finished || selected >= levels || !ticks || ticks >= max_ticks) return false;
        uint32_t old = records.best[selected];
        result_ticks = ticks;
        had_reference = old != 0;
        difference = old ? (int32_t)ticks - (int32_t)old : 0;
        improved = !old || ticks < old;
        if(improved) { records.best[selected] = ticks; dirty = true; }
        finished = true;
        result_left = 30;
        return true;
    }

    void open_menu() { menu = true; finished = false; result_left = 0; }
};

inline void put32(uint8_t *p, uint32_t value) {
    for(unsigned i = 0; i < 4; ++i) { p[i] = (uint8_t)value; value >>= 8; }
}
inline uint32_t get32(const uint8_t *p) {
    uint32_t value = 0;
    for(unsigned i = 0; i < 4; ++i) value |= (uint32_t)p[i] << (8 * i);
    return value;
}
inline uint32_t checksum(const uint8_t *p, unsigned size) {
    uint32_t hash = 2166136261UL;
    for(unsigned i = 0; i < size; ++i) hash = (hash ^ p[i]) * 16777619UL;
    return hash;
}
inline bool newer(uint32_t a, uint32_t b) { return (int32_t)(a - b) > 0; }

inline void encode(const Records &records, uint32_t generation, uint8_t *out) {
    memcpy(out, "CECH", 4); out[4] = 1;
    put32(out + 5, generation);
    out[9] = records.last_level;
    for(unsigned i = 0; i < levels; ++i) put32(out + 10 + 4 * i, records.best[i]);
    put32(out + save_bytes - 4, checksum(out, save_bytes - 4));
}
inline bool decode(const uint8_t *data, unsigned size, Records &out, uint32_t &generation) {
    if(size != save_bytes || memcmp(data, "CECH", 4) || data[4] != 1 || data[9] >= levels) return false;
    if(get32(data + size - 4) != checksum(data, size - 4)) return false;
    Records candidate;
    candidate.last_level = data[9];
    for(unsigned i = 0; i < levels; ++i) {
        candidate.best[i] = get32(data + 10 + 4 * i);
        if(candidate.best[i] >= max_ticks) return false;
    }
    out = candidate;
    generation = get32(data + 5);
    return true;
}
} // namespace echo
