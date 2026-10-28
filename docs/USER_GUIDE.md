# User guide

## Pick and repeat a room

Launch `CECHO`. The selector shows every room in one left-hand list. Up and Down move the highlight, including wrapping between the first room and the summit. Holding a direction repeats after a short delay. The preview and PERSONAL BEST on the right follow the highlighted room. `--:--.---` means there is no saved finish yet.

Enter or 2nd starts the room. Reach the room's normal top exit to finish it. At the summit, touch the flag. The app reloads the same selected room instead of advancing. Enter retries immediately; Graph or Window returns to the selector. Neither action clears a PB.

## Read the result

| Result | Meaning |
|---|---|
| Green `first pb` | First recorded completion of this room |
| Green `-0.500s` | Finished half a second faster than the previous PB |
| Red `+0.333s` | Finished about a third of a second slower than the PB |
| Red `+0.000s` | Matched the PB |

The result stays for 30 simulation ticks, about one second, beneath the timer as the next attempt starts. The comparison is calculated **before** updating the PB, so a new record shows its actual improvement. The selector shows the latest PB afterward.

Times use simulation ticks, not an independent wall clock. At 30 Hz the app can report differences such as 0.033, 0.066, or 0.100 seconds. It cannot distinguish a true 20 ms difference. Slow rendering is handled by the inherited frame-skipping loop; the physics clock has not been sped up.

The attempt timer starts with the room's spawn animation. Dash hitstop counts. A death abandons that attempt and restarts the timer once the room reloads. Manual retries also start at zero. Pausing in the selector does not add time.

## Complete room list

Room numbers refer to the selector, starting at 1. Map indices in the code start at 0.

| Room | Label | Starting dash count |
|---|---|---|
| 01 | 100 m | 1 |
| 02 | 200 m | 1 |
| 03 | 300 m | 1 |
| 04 | 400 m | 1 |
| 05 | 500 m | 1 |
| 06 | 600 m | 1 |
| 07 | 700 m | 1 |
| 08 | 800 m | 1 |
| 09 | 900 m | 1 |
| 10 | 1000 m | 1 |
| 11 | 1100 m | 1 |
| 12 | Old Site | 1 |
| 13 | 1300 m | 1 |
| 14 | 1400 m | 1 |
| 15 | 1500 m | 1 |
| 16 | 1600 m | 1 |
| 17 | 1700 m | 1 |
| 18 | 1800 m | 1 |
| 19 | 1900 m | 1 |
| 20 | 2000 m | 1 |
| 21 | 2100 m | 1 |
| 22 | 2200 m, big chest | 1; obtain the upgrade in the room |
| 23 | 2300 m | 2 |
| 24 | 2400 m | 2 |
| 25 | 2500 m | 2 |
| 26 | 2600 m | 2 |
| 27 | 2700 m | 2 |
| 28 | 2800 m | 2 |
| 29 | 2900 m | 2 |
| 30 | 3000 m | 2 |
| 31 | Summit | 2; touch the flag to finish |

## Keep records

Return to the selector or quit with Clear/Del after practice. These are the save points. The app intentionally does not archive after each short repetition.

`CEchoA` and `CEchoB` contain this app's records only. The last played room is remembered, but every launch still opens the selector. New records do not import PBs from another CEleste program because the timing and entry conditions may differ.

If the selector reports a save failure, free archive space and quit to retry. The previous valid archived copy remains available if the new write fails. A checksummed corrupt copy is ignored at startup. Deleting both AppVars or clearing archive deletes the records; an ordinary RAM reset preserves already archived copies.

## Troubleshooting

- **Missing library:** transfer the included official `clibs.8xg` through TI Connect CE.
- **Invalid program / ASM unavailable:** use your existing compatible assembly launcher. See the official [arTIfiCE instructions](https://yvantt.github.io/arTIfiCE/) if your calculator OS requires it.
- **No shell icon:** the custom icon is for compatible shells; the TI-OS program menu may show only `CECHO`.
- **An improvement disappeared after a RAM reset:** only records saved by returning to the selector or quitting are archived.
- **PB looks different from a previous app:** Echo includes room entry animation, uses the normal starting dash count, and has independent records.

Installing this package does not require replacing the main `CELESTE` program or its save files.
