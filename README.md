# CEleste Echo

Practice one Celeste Classic room at a time on a TI-84 Plus CE. Pick a room, chase its PB, and loop it for as long as you like.

**Calculator program:** `CECHO` · **Game:** all 30 climbing rooms plus the summit · **Save files:** `CEchoA` and `CEchoB`

![Echo icon](docs/icon-preview.png)

The selector opens every time you launch the app. All 31 rooms are listed on the left. Move the highlight with Up and Down; the right side shows that room's preview and saved PB. Press Enter or 2nd to play. A successful finish automatically restarts the selected room.

During play, a small timer stays in the upper left. Finishing a room briefly shows your difference from its previous PB: green `-0.500s` for a faster finish, or red `+0.333s` for a slower one. A tie is red `+0.000s`. Your first finish establishes the PB.

## Install

Download the install ZIP from this private repository's Releases, extract it, and send **`CECHO.8xp` and `clibs.8xg`** with TI Connect CE. Launch `CECHO` with the same assembly-program launcher you already use for CEleste. The app has a separate mint-and-purple mountain/loop icon.

This app can coexist with `CELESTE`. It does not read or write `CelesteS`, `CelPbA`, or `CelPbB`.

## Controls

| Screen | Key | Action |
|---|---|---|
| Selector | Up / Down | Highlight a room; the list wraps |
| Selector | Enter / 2nd | Start the highlighted room |
| In game | Arrows | Move |
| In game | 2nd | Jump |
| In game | Alpha | Dash |
| In game | Enter | Retry this room immediately |
| In game | Graph / Window | Return to the selector and save PBs |
| Either | Clear / Del | Quit and save PBs |

Holding Enter retries only once. The Enter used to launch the app is ignored until released.

## Timing and saves

Movement and the 30 Hz simulation are inherited from CEleste. Milliseconds advance in roughly 33/34 ms steps; the app does not invent 1 ms precision. Each attempt includes the room's spawn animation and hitstop. Death starts a fresh timed attempt after the normal death delay. Late rooms begin with two dashes; the big-chest room begins with one, so its upgrade sequence remains playable.

PBs are kept in memory while you loop a room. They are archived when you return to the selector or quit, which avoids a flash write between timed attempts. Two small checksummed AppVars keep alternating copies. Keep `CEchoA` and `CEchoB` to retain your records. A RAM reset during unsaved practice can lose the current session's improvements; return to the selector or quit first.

## Documentation

- [User guide and complete room list](docs/USER_GUIDE.md)
- [Build instructions](docs/BUILDING.md)
- [Architecture and persistence format](docs/ARCHITECTURE.md)
- [Tests, evidence and remaining limits](docs/TESTING.md)
- [Credits and licenses](THIRD_PARTY.md)

This is a fan-made practice program based on [CEleste](https://github.com/commandz0/CEleste) by John “commandblockguy” Cesarz, itself a port of Celeste Classic by Maddy Thorson and Noel Berry. It is not an official Celeste release.
