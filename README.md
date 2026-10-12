# Dragon Castle — reboot C + C++ (GBA)

A fresh playable prototype for Game Boy Advance. The code is deliberately split by responsibility:

- `src/gfx.c` and `src/gfx.h`: Mode 4 double-buffered graphics, pixel-art drawing primitives, 5×7 font, VBlank and edge-triggered button input (C).
- `src/sound.c` and `src/sound.h`: original castle and battle chiptune melodies using the GBA's built-in square-wave sound hardware.
- `src/game.cpp`: cinematic, original character/dragon shapes, party statistics, all 16 skills, combat UI, damage, healing, shields, buffs, stun and turn order (C++).
- `SKILLS.md`: each protagonist's base stats and all skill power, MP cost, accuracy and effects.
- `Makefile`: builds both languages with devkitARM.
- `.github/workflows/build.yml`: compiles the ROM in GitHub Actions and stores it as `dragoncastle-gba`.

## The opening cinematic

The four protagonists walk through the outer gate, cross the torch-lit corridor, and confront the dragon in the throne chamber. The dialogue is advanced with **A** or **START**. An original, looping castle theme plays during the opening; a faster original battle theme starts when combat begins.

## Party and skills

Every living hero gets one action in the fixed order **Edric → Zaryndra → Vaelis → Kendra**. When the party's four turns finish, the dragon gets its turn. A stunned dragon loses that action. On every third round it uses black flame against the whole party; otherwise it attacks one target.

| Hero | Race | HP | MP | Role |
|---|---|---:|---:|---|
| Edric | Human | 125 | 35 | Balanced fighter, self-shield, party attack buff |
| Zaryndra | Dragon | 108 | 48 | High damage, stun, self-shield |
| Vaelis | Elf | 88 | 74 | High MP, binding magic, healing and revival |
| Kendra | Dwarf | 140 | 38 | Durable heavy hitter, stun, party shield |

Each move has its own power, MP cost and accuracy. The combat menu shows `P` (power), `M` (MP cost) and `A` (accuracy); complete numbers and effects are in `SKILLS.md`. Select with **UP/DOWN**, confirm with **A**, and use **B** for a controls reminder. Insufficient MP does not consume the character's turn.

## Build the GBA ROM

Use the devkitPro **devkitARM** environment. From the repository root, run:

```sh
make
```

The result is `dragoncastle.gba`. To remove build files:

```sh
make clean
```

GitHub Actions builds the ROM after a push or on manual dispatch. Download the `dragoncastle-gba` artifact from the successful run.

## Notes

- This is a new implementation. The previous prototype remains in Git history, but its old rendering/game loop is no longer part of the build.
- Graphics are hand-drawn using small C raster primitives for now; the layout keeps the door open to replacing those shapes with authored sprite/tile assets later.
- The seven gods and seven kingdoms are not assigned to individual characters or regions by this prototype; their existing names remain lore canon and are intentionally not remapped here.
