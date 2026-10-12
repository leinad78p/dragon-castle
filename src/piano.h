#ifndef DRAGON_CASTLE_PIANO_H
#define DRAGON_CASTLE_PIANO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void piano_init(void);
void piano_note_on(uint16_t square_frequency);
void piano_note_off(void);
void piano_update(unsigned age);

#ifdef __cplusplus
}
#endif

#endif /* DRAGON_CASTLE_PIANO_H */
