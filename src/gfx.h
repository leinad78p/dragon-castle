#ifndef DRAGON_CASTLE_GFX_H
#define DRAGON_CASTLE_GFX_H

#include <stdint.h>

typedef uint16_t GfxColor;

#define GFX_RGB(r, g, b) ((GfxColor)(((r) & 31u) | (((g) & 31u) << 5) | (((b) & 31u) << 10)))

#define GFX_KEY_A     (1u << 0)
#define GFX_KEY_B     (1u << 1)
#define GFX_KEY_SELECT (1u << 2)
#define GFX_KEY_START (1u << 3)
#define GFX_KEY_RIGHT (1u << 4)
#define GFX_KEY_LEFT  (1u << 5)
#define GFX_KEY_UP    (1u << 6)
#define GFX_KEY_DOWN  (1u << 7)
#define GFX_KEY_R     (1u << 8)
#define GFX_KEY_L     (1u << 9)

#ifdef __cplusplus
extern "C" {
#endif

void gfx_init(void);
void gfx_wait_vblank(void);
void gfx_wait_frames(unsigned frames);
uint16_t gfx_poll_keys(void);
void gfx_pixel(int x, int y, GfxColor color);
void gfx_clear(GfxColor color);
void gfx_rect(int x, int y, int width, int height, GfxColor color);
void gfx_line(int x0, int y0, int x1, int y1, GfxColor color);
void gfx_triangle(int x0, int y0, int x1, int y1, int x2, int y2, GfxColor color);
void gfx_ellipse(int cx, int cy, int rx, int ry, GfxColor color);
void gfx_text(int x, int y, const char *text, GfxColor color);

#ifdef __cplusplus
}
#endif

#endif
