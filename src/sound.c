/*
 * Dragon Castle - lightweight original chiptune music for Game Boy Advance.
 * Uses the built-in PSG channels; no samples or external libraries.
 */
#include <stdint.h>
#include "sound.h"
#include "piano.h"

#define REG_SOUNDCNT_L (*(volatile uint16_t *)0x04000080)
#define REG_SOUNDCNT_H (*(volatile uint16_t *)0x04000082)
#define REG_SOUNDCNT_X (*(volatile uint16_t *)0x04000084)
#define REG_SND2CNT    (*(volatile uint16_t *)0x04000068)
#define REG_SND2FREQ   (*(volatile uint16_t *)0x0400006C)

typedef struct {
    uint16_t frequency;
    uint8_t frames;
} Note;

/* GBA frequency register values (2048 - 131072 / frequency in Hz).
 * Rests are represented by frequency zero. These melodies are original.
 */
static const Note castle_song[] = {
    {1547,18}, {0,5}, {1709,18}, {1602,18}, {1547,24}, {0,6},
    {1626,18}, {1709,18}, {1797,24}, {1709,18}, {1626,18}, {1547,30},
    {0,8}, {1462,18}, {1547,18}, {1602,18}, {1626,24}, {0,6},
    {1709,18}, {1626,18}, {1602,18}, {1547,24}, {1462,18}, {0,6},
    {1547,18}, {1626,18}, {1709,24}, {1626,18}, {1547,30}, {0,12},
    {1462,18}, {1547,30}
};

static const Note battle_song[] = {
    {1709,12}, {1709,12}, {1797,12}, {1709,12}, {1626,12}, {1547,12},
    {1626,12}, {1709,18}, {0,4}, {1797,12}, {1900,12}, {1797,12},
    {1709,12}, {1626,12}, {1547,12}, {1626,12}, {1709,18}, {0,4},
    {1547,12}, {1547,12}, {1626,12}, {1709,12}, {1797,12}, {1709,12},
    {1626,12}, {1547,18}, {0,4}, {1626,12}, {1709,12}, {1626,12},
    {1547,18}, {0,8}
};

static int active_track = -1;
static unsigned note_index = 0;
static unsigned frame_counter = 0;
static unsigned note_count = 0;
static const Note *active_song = 0;

static void play_note(uint16_t frequency) {
    if (active_track == 0) {
        REG_SND2CNT = 0;
        piano_note_on(frequency);
        return;
    }
    piano_note_off();
    if (frequency == 0) {
        REG_SND2CNT = 0;
        return;
    }
    REG_SND2CNT = 0xF080; /* Channel 2: square wave, full envelope volume. */
    REG_SND2FREQ = (uint16_t)(0x8000u | (frequency & 0x07FFu));
}

void music_init(void) {
    /* Enable sound first; register writes before this can be ignored. */
    REG_SOUNDCNT_X = 0x0080u;
    /* Route PSG channels 2 and 3 to both speakers. */
    REG_SOUNDCNT_L = 0x6677u;
    /* PSG volume ratio: 100%; preserve Direct Sound control bits. */
    REG_SOUNDCNT_H = (uint16_t)((REG_SOUNDCNT_H & (uint16_t)~0x0003u) | 0x0002u);
    REG_SND2CNT = 0;
    piano_init();
    active_track = -1;
    note_index = 0;
    frame_counter = 0;
    note_count = 0;
    active_song = 0;
}

void music_set_track(int track) {
    if (track != 0 && track != 1) track = 0;
    if (active_track == track) return;

    active_track = track;
    note_index = 0;
    frame_counter = 0;
    if (track == 1) {
        active_song = battle_song;
        note_count = sizeof(battle_song) / sizeof(battle_song[0]);
    } else {
        active_song = castle_song;
        note_count = sizeof(castle_song) / sizeof(castle_song[0]);
    }
    play_note(active_song[0].frequency);
}

/* Called once per video frame from the main loop (about 60 times/second). */
void music_update(void) {
    if (!active_song || note_count == 0) return;

    ++frame_counter;
    if (active_track == 0 && active_song[note_index].frequency != 0)
        piano_update(frame_counter);
    if (frame_counter < (unsigned)active_song[note_index].frames) return;
    frame_counter = 0;
    note_index = (note_index + 1u) % note_count;
    play_note(active_song[note_index].frequency);
}
