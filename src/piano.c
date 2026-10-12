/*
 * Dragon Castle piano-like instrument.
 * PSG channel 3 plays a custom 32-sample waveform; software steps its volume
 * down to imitate the short decay of a struck/plucked keyboard note.
 */
#include <stdint.h>
#include "piano.h"

#define REG_SOUNDCNT_L (*(volatile uint16_t *)0x04000080)
#define REG_SND3CNT_L  (*(volatile uint16_t *)0x04000070)
#define REG_SND3CNT_H  (*(volatile uint16_t *)0x04000072)
#define REG_SND3FREQ   (*(volatile uint16_t *)0x04000074)
#define REG_WAVE_RAM   ((volatile uint16_t *)0x04000090)

#define VOL_100 0x2000u
#define VOL_50  0x4000u
#define VOL_25  0x6000u

static const uint8_t piano_wave[32] = {
    8,11,13,14,13,11,9,7, 5,4,3,4,6,8,10,11,
    12,11,9,7,5,4,4,5, 7,9,10,10,9,8,7,7
};

static int playing = 0;

void piano_init(void) {
    unsigned i;

    /* Preserve the other channels' routing; add channel 3 to both speakers. */
    REG_SOUNDCNT_L |= 0x4400u;

    /* Disable output, select bank 0 for writing, then enable playback. */
    REG_SND3CNT_L = 0x0040u;
    for (i = 0; i < 8; ++i) {
        uint16_t a = piano_wave[i * 4u];
        uint16_t b = piano_wave[i * 4u + 1u];
        uint16_t c = piano_wave[i * 4u + 2u];
        uint16_t d = piano_wave[i * 4u + 3u];
        /* Keep the established GBA wave-RAM nibble packing order. */
        REG_WAVE_RAM[i] = (uint16_t)((c << 12) | (d << 8) | (a << 4) | b);
    }
    REG_SND3CNT_L = 0x0080u;
    REG_SND3CNT_H = 0;
    playing = 0;
}

void piano_note_on(uint16_t square_frequency) {
    uint16_t wave_frequency;

    if (square_frequency == 0) {
        piano_note_off();
        return;
    }

    /* Convert from square-channel tuning to wave-channel tuning. */
    wave_frequency = (uint16_t)((2048u + square_frequency) / 2u);
    playing = 1;
    REG_SND3CNT_L = 0x0080u;
    REG_SND3CNT_H = VOL_100;
    REG_SND3FREQ = (uint16_t)(0x8000u | (wave_frequency & 0x07FFu));
}

void piano_note_off(void) {
    playing = 0;
    REG_SND3CNT_H = 0;
}

void piano_update(unsigned age) {
    if (!playing) return;

    /* Channel 3 has no hardware envelope; approximate a struck-key decay. */
    if (age < 3u) {
        REG_SND3CNT_H = VOL_100;
    } else if (age < 7u) {
        REG_SND3CNT_H = VOL_50;
    } else if (age < 14u) {
        REG_SND3CNT_H = VOL_25;
    } else {
        piano_note_off();
    }
}
