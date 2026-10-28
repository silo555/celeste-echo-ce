# Verification

The automated workflow records the toolchain version and runs these checks before packaging:

- Native C++ state tests: all 31 selections, wraparound, menu time exclusion, first/faster/slower/tied PBs, prior-PB comparison, reset and automatic loop state, exact 30-tick popup, independent rooms and summit, save roundtrip, every single-bit corruption, truncated payloads and generation wrap.
- Integration tests of the actual `practice_loop.cpp` and `echo_records.cpp`: launch-key latch, manual retry without key repeat, selectors and PB text, green/red result strings, safe queued reloads, starting/completing every room including summit, deferred archive writes, separate AppVars, failed writes/open/archive calls, retry and corrupt-newest recovery.
- Draw-bound assertions and a host-rendered 320x240 selector layout built from actual draw calls, source font and room pixels.
- Native eZ80 compilation/linking, TI program signature/length/checksum, original room-map hash, and unchanged player/collision/entity physics section hashes.
- ZIP CRC and SHA256 manifest validation.

## Limits

Host tests simulate the keypad, drawing API, room reset and TI AppVar filesystem. They do not emulate eZ80 execution, LCD registers, the real flash/archive implementation, or the game's full physics interaction with the display loop. A host layout preview is not proof that screen-mode transitions work on hardware.

No calculator or calculator-ROM emulator play test has been performed during this work. The install package and release notes must keep that limitation visible. Compilation and host-test passes must not be described as successful on-calculator gameplay.

## Hardware / emulator checklist

1. Launch with Enter held; verify the selector remains until a fresh press.
2. Highlight all rooms, confirm the PB panel and preview follow, and wrap from first to last.
3. Start early, big-chest, late two-dash and summit rooms; verify normal controls and entry state.
4. Finish each type: normal exit or summit flag must reload the selected room, once, safely.
5. Verify timer through spawn, dash hitstop and death; death/manual retry starts a fresh count.
6. Beat, tie and miss a room PB. Check sign, color, milliseconds, one-second lifetime, and no stale pixels on either buffer.
7. Return to the selector and quit/relaunch; confirm PBs and last-played selection persist.
8. In disposable emulator state, test RAM reset after a save and corrupt/full archive conditions.
9. Compare repeatable input behavior with the original engine. Check both 8 bpp selector and 4 bpp gameplay modes for flicker, clipping and palette issues.

Use a user-owned calculator ROM for CEmu. No ROM is included in this repository.
