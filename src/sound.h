#ifndef DRAGON_CASTLE_SOUND_H
#define DRAGON_CASTLE_SOUND_H

#ifdef __cplusplus
extern "C" {
#endif

/* Small hardware-PSG music driver. Track 0 = castle, track 1 = battle. */
void music_init(void);
void music_set_track(int track);
void music_update(void);

#ifdef __cplusplus
}
#endif

#endif
