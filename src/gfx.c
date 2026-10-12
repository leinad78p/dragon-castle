/* Dragon Castle - GBA graphics and input layer (C, no libgba dependency). */
#include "gfx.h"

#define SCREEN_W 240
#define SCREEN_H 160
#define REG_DISPCNT (*(volatile uint16_t *)0x04000000)
#define REG_DISPSTAT (*(volatile uint16_t *)0x04000004)
#define REG_KEYINPUT (*(volatile uint16_t *)0x04000130)
#define REG_BG_PALETTE ((volatile uint16_t *)0x05000000)
#define VRAM_PAGE0 ((volatile uint8_t *)0x06000000)
#define VRAM_PAGE1 ((volatile uint8_t *)0x0600A000)

static volatile uint8_t *draw_page = VRAM_PAGE1;
static uint16_t palette_colors[256];
/* Small direct-mapped cache: the GBA only has 32 KB of IWRAM. */
#define COLOR_CACHE_SIZE 1024u
static uint16_t color_cache_keys[COLOR_CACHE_SIZE];
static uint8_t color_cache_values[COLOR_CACHE_SIZE];
static unsigned palette_count = 0;
static int frame_dirty = 0;

static unsigned color_index(GfxColor color) {
    unsigned i;
    unsigned key = (unsigned)color & 0x7FFFu;
    unsigned slot = (key * 2654435761u) >> 22;
    if (color_cache_keys[slot] == key) return color_cache_values[slot];

    for (i = 0; i < palette_count; ++i) {
        if (palette_colors[i] == key) {
            color_cache_keys[slot] = (uint16_t)key;
            color_cache_values[slot] = (uint8_t)i;
            return i;
        }
    }

    if (palette_count < 256) {
        unsigned index = palette_count++;
        palette_colors[index] = (uint16_t)key;
        REG_BG_PALETTE[index] = (uint16_t)key;
        color_cache_keys[slot] = (uint16_t)key;
        color_cache_values[slot] = (uint8_t)index;
        return index;
    }

    /* Remember the nearest palette match too, for frequently used colors. */
    {
        unsigned best = 0;
        unsigned best_distance = 0xFFFFFFFFu;
        int r = key & 31, g = (key >> 5) & 31, b = (key >> 10) & 31;
        for (i = 0; i < 256; ++i) {
            int dr = r - (palette_colors[i] & 31);
            int dg = g - ((palette_colors[i] >> 5) & 31);
            int db = b - ((palette_colors[i] >> 10) & 31);
            unsigned distance = (unsigned)(dr * dr + dg * dg + db * db);
            if (distance < best_distance) {
                best_distance = distance;
                best = i;
            }
        }
        color_cache_keys[slot] = (uint16_t)key;
        color_cache_values[slot] = (uint8_t)best;
        return best;
    }
}
static void put_pixel_index(int x, int y, unsigned index) {
    if ((unsigned)x < SCREEN_W && (unsigned)y < SCREEN_H) {
        draw_page[y * SCREEN_W + x] = (uint8_t)index;
        frame_dirty = 1;
    }
}

/* Each glyph contains seven rows of five pixels. */
typedef struct { char ch; uint8_t rows[7]; } Glyph;
static const Glyph FONT[] = {
    {' ',{0,0,0,0,0,0,0}},
    {'A',{14,17,17,31,17,17,17}}, {'B',{30,17,17,30,17,17,30}},
    {'C',{14,17,16,16,16,17,14}}, {'D',{30,17,17,17,17,17,30}},
    {'E',{31,16,16,30,16,16,31}}, {'F',{31,16,16,30,16,16,16}},
    {'G',{14,17,16,23,17,17,15}}, {'H',{17,17,17,31,17,17,17}},
    {'I',{14,4,4,4,4,4,14}}, {'J',{7,2,2,2,18,18,12}},
    {'K',{17,18,20,24,20,18,17}}, {'L',{16,16,16,16,16,16,31}},
    {'M',{17,27,21,21,17,17,17}}, {'N',{17,25,21,19,17,17,17}},
    {'O',{14,17,17,17,17,17,14}}, {'P',{30,17,17,30,16,16,16}},
    {'Q',{14,17,17,17,21,18,13}}, {'R',{30,17,17,30,20,18,17}},
    {'S',{15,16,16,14,1,1,30}}, {'T',{31,4,4,4,4,4,4}},
    {'U',{17,17,17,17,17,17,14}}, {'V',{17,17,17,17,17,10,4}},
    {'W',{17,17,17,21,21,21,10}}, {'X',{17,17,10,4,10,17,17}},
    {'Y',{17,17,10,4,4,4,4}}, {'Z',{31,1,2,4,8,16,31}},
    {'0',{14,17,19,21,25,17,14}}, {'1',{4,12,4,4,4,4,14}},
    {'2',{14,17,1,2,4,8,31}}, {'3',{30,1,1,14,1,1,30}},
    {'4',{2,6,10,18,31,2,2}}, {'5',{31,16,16,30,1,1,30}},
    {'6',{14,16,16,30,17,17,14}}, {'7',{31,1,2,4,8,8,8}},
    {'8',{14,17,17,14,17,17,14}}, {'9',{14,17,17,15,1,1,14}},
    {':',{0,4,4,0,4,4,0}}, {'!',{4,4,4,4,4,0,4}},
    {'?',{14,17,1,2,4,0,4}}, {'.',{0,0,0,0,0,12,12}},
    {',',{0,0,0,0,4,4,8}}, {'-',{0,0,0,31,0,0,0}},
    {'+',{0,4,4,31,4,4,0}}, {'/',{1,2,2,4,8,8,16}},
    {'%',{17,2,4,8,17,0,0}}, {'>',{16,8,4,2,4,8,16}},
    {'<',{1,2,4,8,4,2,1}}, {'=',{0,31,0,31,0,0,0}},
    {'(',{2,4,8,8,8,4,2}}, {')',{8,4,2,2,2,4,8}},
    {'\'',{4,4,2,0,0,0,0}}, {'"',{10,10,0,0,0,0,0}},
    {'_',{0,0,0,0,0,0,31}}, {'*',{0,21,14,31,14,21,0}}
};

static uint16_t previous_keys = 0;

void gfx_init(void) {
    unsigned i;
    REG_DISPCNT = 0x0404; /* Mode 4 + BG2; 240x160, indexed color, page 0 visible. */
    draw_page = VRAM_PAGE1;
    palette_count = 0;
    frame_dirty = 0;
    for (i = 0; i < COLOR_CACHE_SIZE; ++i) color_cache_keys[i] = 0xFFFFu;
    for (i = 0; i < 256; ++i) {
        palette_colors[i] = 0;
        REG_BG_PALETTE[i] = 0;
    }
    previous_keys = (uint16_t)((~REG_KEYINPUT) & 0x03FFu);
    gfx_clear(GFX_RGB(1, 2, 8));
}

void gfx_wait_vblank(void) {
    while (REG_DISPSTAT & 1u) { }
    while (!(REG_DISPSTAT & 1u)) { }

    /* Flip only after a complete frame has been drawn to the hidden page. */
    if (frame_dirty) {
        if (draw_page == VRAM_PAGE1) {
            REG_DISPCNT |= 0x0010u;  /* Display page 1. */
            draw_page = VRAM_PAGE0;
        } else {
            REG_DISPCNT &= (uint16_t)~0x0010u; /* Display page 0. */
            draw_page = VRAM_PAGE1;
        }
        frame_dirty = 0;
    }
}

void gfx_wait_frames(unsigned frames) {
    while (frames--) {
        gfx_wait_vblank();
        (void)gfx_poll_keys();
    }
}

uint16_t gfx_poll_keys(void) {
    uint16_t held = (uint16_t)((~REG_KEYINPUT) & 0x03FFu);
    uint16_t pressed = (uint16_t)(held & (uint16_t)~previous_keys);
    previous_keys = held;
    return pressed;
}

void gfx_pixel(int x, int y, GfxColor color) {
    put_pixel_index(x, y, color_index(color));
}

void gfx_clear(GfxColor color) {
    int i;
    unsigned index = color_index(color);
    for (i = 0; i < SCREEN_W * SCREEN_H; ++i) draw_page[i] = (uint8_t)index;
    frame_dirty = 1;
}

void gfx_rect(int x, int y, int width, int height, GfxColor color) {
    int xx, yy;
    unsigned index = color_index(color);
    int x_end = x + width;
    int y_end = y + height;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x_end > SCREEN_W) x_end = SCREEN_W;
    if (y_end > SCREEN_H) y_end = SCREEN_H;
    if (x >= x_end || y >= y_end) return;
    for (yy = y; yy < y_end; ++yy) {
        volatile uint8_t *row = draw_page + yy * SCREEN_W;
        for (xx = x; xx < x_end; ++xx) row[xx] = (uint8_t)index;
    }
    frame_dirty = 1;
}

void gfx_line(int x0, int y0, int x1, int y1, GfxColor color) {
    int dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int sx = x0 < x1 ? 1 : -1;
    int dy = -(y1 > y0 ? y1 - y0 : y0 - y1);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        gfx_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        {
            int twice = 2 * err;
            if (twice >= dy) { err += dy; x0 += sx; }
            if (twice <= dx) { err += dx; y0 += sy; }
        }
    }
}

static int edge_x(int x0, int y0, int x1, int y1, int y) {
    if (y0 == y1) return x0;
    return x0 + (y - y0) * (x1 - x0) / (y1 - y0);
}

void gfx_triangle(int x0, int y0, int x1, int y1, int x2, int y2, GfxColor color) {
    int y, min_y = y0, max_y = y0;
    if (y1 < min_y) min_y = y1;
    if (y2 < min_y) min_y = y2;
    if (y1 > max_y) max_y = y1;
    if (y2 > max_y) max_y = y2;
    if (min_y < 0) min_y = 0;
    if (max_y >= SCREEN_H) max_y = SCREEN_H - 1;
    for (y = min_y; y <= max_y; ++y) {
        int xs[3], n = 0, i, j;
        if (y >= (y0 < y1 ? y0 : y1) && y <= (y0 > y1 ? y0 : y1)) xs[n++] = edge_x(x0,y0,x1,y1,y);
        if (y >= (y1 < y2 ? y1 : y2) && y <= (y1 > y2 ? y1 : y2)) xs[n++] = edge_x(x1,y1,x2,y2,y);
        if (y >= (y2 < y0 ? y2 : y0) && y <= (y2 > y0 ? y2 : y0)) xs[n++] = edge_x(x2,y2,x0,y0,y);
        for (i = 1; i < n; ++i) {
            int t = xs[i];
            for (j = i - 1; j >= 0 && xs[j] > t; --j) xs[j + 1] = xs[j];
            xs[j + 1] = t;
        }
        if (n >= 2) gfx_line(xs[0], y, xs[n - 1], y, color);
    }
}

void gfx_ellipse(int cx, int cy, int rx, int ry, GfxColor color) {
    int x, y;
    int limit;
    if (rx < 1 || ry < 1) return;
    limit = rx * rx * ry * ry;
    for (y = -ry; y <= ry; ++y)
        for (x = -rx; x <= rx; ++x)
            if (x * x * ry * ry + y * y * rx * rx <= limit)
                gfx_pixel(cx + x, cy + y, color);
}

static const Glyph *find_glyph(char ch) {
    unsigned int i;
    if (ch >= 'a' && ch <= 'z') ch = (char)(ch - 'a' + 'A');
    for (i = 0; i < sizeof(FONT) / sizeof(FONT[0]); ++i)
        if (FONT[i].ch == ch) return &FONT[i];
    return &FONT[0];
}

void gfx_text(int x, int y, const char *text, GfxColor color) {
    unsigned index = color_index(color);
    while (*text) {
        const Glyph *g = find_glyph(*text++);
        int row, col;
        for (row = 0; row < 7; ++row)
            for (col = 0; col < 5; ++col)
                if (g->rows[row] & (uint8_t)(1u << (4 - col)))
                    put_pixel_index(x + col, y + row, index);
        x += 6;
    }
}
