# Architecture

## Engine and practice controller

The engine comes from CEleste base commit `538f42e001c0455bfda2f47d82b2550b81c51002`, with the previous timer/sprite-copy improvements. `classic.cpp` retains player movement, jump/dash constants, object collision, and map data. `practice_reset_room` clears transient state and sets the correct one/two-dash starting state for the chosen map.

`practice_loop.cpp` owns the selector, key edges/repeat, attempt clock, result HUD and queued retries. A top exit requests completion through the inherited player hook; the summit uses the flag. A retry is queued during an entity update/draw and applied only after that call stack has returned. This avoids destroying the currently executing object. Death uses the same deferred reload after its normal delay.

`emu.cpp` keeps the original 30 Hz pacing and frame-skip configuration. The selector uses the full 320x240 8 bpp screen so all 31 rows fit. Playing restores the port's original 4 bpp renderer. Both buffers are cleared when changing modes. Menu/retry ticks discard accumulated pacing debt; time spent in the selector or writing archive is not charged to a new attempt.

The preview is generated from the original room tilemap and sprites. Its heap buffer is freed before starting play. Rendering in the selector never advances game entities.

## Timing

`echo_state.h` is the platform-independent state model. Each active simulation tick increments the attempt once. The count includes spawn animation and dash hitstop. Death/retry resets the count; completion captures it, compares it with the previous PB, updates the PB if faster, and keeps a 30-tick result across the automatic reload.

Comparison color is green for improvement and red for slower/tied finishes. The first valid finish establishes a PB. A zero-tick or 24-hour/capped attempt does not establish a record. Completed attempts cannot be counted twice.

## Record journal

`echo_records.cpp` uses `CEchoA` and `CEchoB`; no main-game AppVars are opened. The payload is explicitly byte-encoded, so C++ structure padding and the eZ80's 24-bit `int` do not affect the file format.

| Bytes | Value |
|---|---|
| 0–3 | ASCII `CECH` |
| 4 | Format version, 1 |
| 5–8 | Little-endian uint32 generation |
| 9 | Last played map index, 0–30 |
| 10–133 | 31 little-endian uint32 PBs in simulation ticks; zero means unset |
| 134–137 | FNV-1a uint32 checksum of preceding bytes |

Both slots are validated on launch; the newest valid generation wins. Writes replace the inactive slot, check write completion, and require successful archiving before marking records clean. Failure leaves the previous active archived slot available and dirty records in memory for a retry.

Archive writes occur only on returning to the selector or quitting. This protects short attempts from flash-write pauses. Session improvements can be lost if RAM is reset before either save point.

## Scope

Echo is a separate `CECHO` program with its own records and shell icon. It does not resume a full-game run, import main-game PBs, add alternate physics, or change the simulation rate. Standalone room entry is deliberately repeatable; later rooms start with the upgrade a normal run would already have.
