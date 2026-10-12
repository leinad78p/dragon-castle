/*
 * DRAGON CASTLE - new C++ game layer for Game Boy Advance.
 * Plot, animation, party data and turn order live here; raster/input lives in gfx.c.
 * Freestanding-friendly: no STL, heap allocation, exceptions, RTTI or libgba.
 */
#include "gfx.h"

#define SCREEN_W 240
#define SCREEN_H 160

typedef GfxColor Color;

static const Color BLACK       = GFX_RGB(1, 1, 3);
static const Color INK         = GFX_RGB(2, 2, 7);
static const Color NIGHT       = GFX_RGB(2, 4, 12);
static const Color SKY         = GFX_RGB(4, 7, 19);
static const Color SKY_LIGHT   = GFX_RGB(9, 12, 27);
static const Color STONE       = GFX_RGB(13, 15, 25);
static const Color STONE_LIGHT = GFX_RGB(22, 24, 31);
static const Color STONE_DARK  = GFX_RGB(6, 7, 14);
static const Color GOLD        = GFX_RGB(31, 23, 7);
static const Color GOLD_DARK   = GFX_RGB(18, 11, 3);
static const Color WHITE       = GFX_RGB(30, 30, 31);
static const Color MOON        = GFX_RGB(25, 27, 31);
static const Color RED         = GFX_RGB(26, 4, 7);
static const Color RED_LIGHT   = GFX_RGB(31, 9, 8);
static const Color RED_DARK    = GFX_RGB(12, 2, 5);
static const Color ORANGE      = GFX_RGB(31, 14, 2);
static const Color FIRE        = GFX_RGB(31, 25, 5);
static const Color BLUE        = GFX_RGB(7, 15, 29);
static const Color BLUE_LIGHT  = GFX_RGB(13, 24, 31);
static const Color GREEN       = GFX_RGB(5, 19, 10);
static const Color GREEN_LIGHT = GFX_RGB(10, 28, 15);
static const Color PURPLE      = GFX_RGB(14, 5, 24);
static const Color SKIN        = GFX_RGB(28, 17, 10);
static const Color HAIR        = GFX_RGB(5, 3, 4);
static const Color BROWN       = GFX_RGB(15, 7, 3);
static const Color PANEL       = GFX_RGB(2, 3, 10);
static const Color PANEL_EDGE  = GFX_RGB(22, 17, 8);
static const Color HP_GREEN    = GFX_RGB(5, 26, 10);
static const Color MP_BLUE     = GFX_RGB(6, 17, 29);
static const Color GREY        = GFX_RGB(12, 13, 19);

static int hero_x[4] = { 22, 7, -8, -23 };
static const int HERO_FEET = 109;
static int current_scene = 0; /* 0 exterior, 1 hallway, 2 dragon chamber */

static void draw_exterior(void);
static void draw_hallway(void);
static void draw_chamber_background(void);
static void draw_hero(int who, int cx, int foot_y, int step);
static void draw_dragon(int cx, int foot_y, int roaring);
static void draw_scene(void);
static void wait_confirm(void);

/* --- Low-level pixel-art actors. These are original shapes, not placeholders. --- */
static void draw_hero(int who, int cx, int foot_y, int step) {
    int leg = (step & 1) ? 1 : 0;
    Color coat = BLUE, trim = BLUE_LIGHT, hair = HAIR, skin = SKIN;
    Color metal = STONE_LIGHT;
    int head_y = foot_y - 22;

    if (who == 1) { coat = RED; trim = RED_LIGHT; skin = GFX_RGB(22, 10, 7); }
    if (who == 2) { coat = GREEN; trim = GREEN_LIGHT; skin = GFX_RGB(25, 18, 12); }
    if (who == 3) { coat = BROWN; trim = ORANGE; skin = GFX_RGB(27, 16, 9); }

    /* Feet and legs, with a two-frame walking cycle. */
    gfx_rect(cx - 5, foot_y - 7 + leg, 4, 6 - leg, INK);
    gfx_rect(cx + 1, foot_y - 7 + (1 - leg), 4, 6 - (1 - leg), INK);
    gfx_rect(cx - 6, foot_y - 2 + leg, 5, 2, who == 3 ? BROWN : STONE_DARK);
    gfx_rect(cx + 1, foot_y - 2 + (1 - leg), 5, 2, who == 3 ? BROWN : STONE_DARK);

    /* Torso silhouette and colored tunic/armor. */
    gfx_rect(cx - 6, foot_y - 17, 12, 11, INK);
    gfx_rect(cx - 4, foot_y - 16, 8, 9, coat);
    gfx_rect(cx - 3, foot_y - 15, 2, 6, trim);
    gfx_rect(cx + 1, foot_y - 14, 2, 5, metal);
    gfx_line(cx - 5, foot_y - 14, cx - 8, foot_y - 9, INK);
    gfx_line(cx + 5, foot_y - 14, cx + 8, foot_y - 10, INK);
    gfx_line(cx - 5, foot_y - 14, cx - 7, foot_y - 10, skin);
    gfx_line(cx + 5, foot_y - 14, cx + 7, foot_y - 10, skin);

    /* Head, hair and species-specific silhouette. */
    gfx_ellipse(cx, head_y, 5, 6, INK);
    gfx_ellipse(cx, head_y, 4, 5, skin);
    gfx_rect(cx - 4, head_y - 4, 8, 3, hair);
    gfx_rect(cx - 5, head_y - 2, 2, 4, hair);
    gfx_pixel(cx + 2, head_y, INK);

    if (who == 0) { /* Edric: human sword-bearer, silver-blue armor. */
        gfx_rect(cx - 4, foot_y - 16, 8, 2, STONE_LIGHT);
        gfx_line(cx + 6, foot_y - 17, cx + 10, foot_y - 27, WHITE);
        gfx_line(cx + 5, foot_y - 16, cx + 9, foot_y - 26, GOLD);
        gfx_line(cx + 4, foot_y - 15, cx + 8, foot_y - 13, GOLD);
    } else if (who == 1) { /* Zaryndra: dragon-blooded, horns and tail. */
        gfx_triangle(cx - 3, head_y - 4, cx - 6, head_y - 9, cx, head_y - 4, GOLD_DARK);
        gfx_triangle(cx + 2, head_y - 4, cx + 5, head_y - 9, cx + 5, head_y - 3, GOLD_DARK);
        gfx_line(cx - 4, foot_y - 10, cx - 9, foot_y - 7, RED_DARK);
        gfx_line(cx - 9, foot_y - 7, cx - 11, foot_y - 10, RED_LIGHT);
        gfx_pixel(cx + 2, head_y, FIRE);
        gfx_rect(cx - 4, foot_y - 15, 2, 3, GOLD_DARK);
    } else if (who == 2) { /* Vaelis: elf, pointed ears and green mantle. */
        gfx_triangle(cx - 3, head_y - 1, cx - 9, head_y - 2, cx - 4, head_y + 2, skin);
        gfx_triangle(cx + 3, head_y - 1, cx + 9, head_y - 2, cx + 4, head_y + 2, skin);
        gfx_rect(cx - 4, head_y - 5, 8, 2, GFX_RGB(17, 19, 13));
        gfx_line(cx + 6, foot_y - 17, cx + 8, foot_y - 5, GOLD);
        gfx_pixel(cx + 8, foot_y - 6, GREEN_LIGHT);
    } else { /* Kendra: stocky dwarf with beard and a hammer. */
        gfx_rect(cx - 4, head_y + 1, 8, 4, ORANGE);
        gfx_rect(cx - 3, head_y + 1, 6, 2, GFX_RGB(24, 11, 3));
        gfx_rect(cx - 4, foot_y - 13, 8, 4, ORANGE);
        gfx_line(cx + 6, foot_y - 16, cx + 9, foot_y - 10, BROWN);
        gfx_rect(cx + 7, foot_y - 19, 5, 4, STONE_LIGHT);
    }
}

static void draw_dragon(int cx, int foot_y, int roaring) {
    Color outline = INK;
    Color wing = RED_DARK;
    Color wing_lit = RED;
    Color scales = GFX_RGB(22, 6, 8);
    Color belly = GFX_RGB(25, 14, 8);
    int top = foot_y - 42;

    /* Vast bat-like wings, each with visible ribs. */
    gfx_triangle(cx - 7, top + 24, cx - 42, top - 1, cx - 32, top + 31, outline);
    gfx_triangle(cx - 7, top + 24, cx - 38, top + 5, cx - 31, top + 27, wing);
    gfx_triangle(cx - 7, top + 24, cx - 31, top + 27, cx - 18, top + 20, wing_lit);
    gfx_line(cx - 7, top + 24, cx - 38, top + 5, scales);
    gfx_line(cx - 7, top + 24, cx - 31, top + 27, scales);
    gfx_line(cx - 7, top + 24, cx - 18, top + 20, scales);

    gfx_triangle(cx + 5, top + 23, cx + 40, top - 3, cx + 32, top + 31, outline);
    gfx_triangle(cx + 5, top + 23, cx + 36, top + 4, cx + 31, top + 27, wing);
    gfx_triangle(cx + 5, top + 23, cx + 31, top + 27, cx + 18, top + 19, wing_lit);
    gfx_line(cx + 5, top + 23, cx + 36, top + 4, scales);
    gfx_line(cx + 5, top + 23, cx + 31, top + 27, scales);
    gfx_line(cx + 5, top + 23, cx + 18, top + 19, scales);

    /* Tail, body and belly. */
    gfx_line(cx + 10, foot_y - 12, cx + 22, foot_y - 8, outline);
    gfx_line(cx + 22, foot_y - 8, cx + 33, foot_y - 13, outline);
    gfx_triangle(cx + 33, foot_y - 13, cx + 40, foot_y - 18, cx + 38, foot_y - 9, wing_lit);
    gfx_ellipse(cx + 1, foot_y - 16, 17, 10, outline);
    gfx_ellipse(cx + 1, foot_y - 17, 14, 8, scales);
    gfx_ellipse(cx - 3, foot_y - 13, 8, 6, belly);

    /* Neck and head turn towards the four heroes on the left. */
    gfx_triangle(cx - 8, foot_y - 18, cx - 22, foot_y - 35, cx + 3, foot_y - 34, outline);
    gfx_triangle(cx - 7, foot_y - 18, cx - 19, foot_y - 33, cx, foot_y - 32, scales);
    gfx_ellipse(cx - 21, foot_y - 34, 9, 6, outline);
    gfx_ellipse(cx - 22, foot_y - 35, 7, 4, scales);
    gfx_triangle(cx - 28, foot_y - 35, cx - 39, foot_y - 32, cx - 28, foot_y - 30, outline);
    gfx_line(cx - 29, foot_y - 33, cx - 37, foot_y - 33, belly);

    /* Horns, back spines, claws and the amber eye. */
    gfx_line(cx - 24, foot_y - 39, cx - 27, foot_y - 46, GOLD);
    gfx_line(cx - 20, foot_y - 39, cx - 18, foot_y - 45, belly);
    gfx_triangle(cx - 12, foot_y - 25, cx - 9, foot_y - 31, cx - 6, foot_y - 25, wing_lit);
    gfx_triangle(cx - 5, foot_y - 24, cx - 2, foot_y - 30, cx + 2, foot_y - 23, wing_lit);
    gfx_triangle(cx + 2, foot_y - 24, cx + 6, foot_y - 29, cx + 9, foot_y - 22, wing_lit);
    gfx_line(cx - 3, foot_y - 9, cx - 6, foot_y - 4, outline);
    gfx_line(cx - 6, foot_y - 4, cx - 12, foot_y - 4, GOLD_DARK);
    gfx_line(cx + 6, foot_y - 9, cx + 9, foot_y - 4, outline);
    gfx_line(cx + 9, foot_y - 4, cx + 14, foot_y - 4, GOLD_DARK);
    gfx_pixel(cx - 24, foot_y - 36, FIRE);
    gfx_pixel(cx - 23, foot_y - 36, FIRE);
    gfx_pixel(cx - 34, foot_y - 32, belly);

    if (roaring) {
        gfx_triangle(cx - 37, foot_y - 32, cx - 49, foot_y - 29, cx - 39, foot_y - 27, RED_LIGHT);
        gfx_triangle(cx - 40, foot_y - 31, cx - 54, foot_y - 27, cx - 42, foot_y - 25, ORANGE);
        gfx_triangle(cx - 43, foot_y - 29, cx - 50, foot_y - 27, cx - 45, foot_y - 24, FIRE);
    }
}

/* --- Three hand-built scenes for the entrance cinematic. --- */
static void draw_exterior(void) {
    int i;
    gfx_clear(SKY);
    gfx_rect(0, 0, SCREEN_W, 54, NIGHT);
    gfx_ellipse(198, 25, 11, 11, MOON);
    gfx_ellipse(203, 21, 10, 10, NIGHT);
    for (i = 0; i < 20; ++i) {
        int x = (i * 37 + 13) % SCREEN_W;
        int y = 7 + (i * 19) % 42;
        gfx_pixel(x, y, i % 3 ? WHITE : GOLD);
        if (i % 4 == 0) gfx_pixel(x + 1, y, MOON);
    }
    gfx_triangle(0, 94, 38, 57, 86, 94, GFX_RGB(5, 7, 16));
    gfx_triangle(128, 94, 182, 49, 239, 94, GFX_RGB(5, 7, 16));
    gfx_triangle(0, 99, 69, 75, 111, 99, GFX_RGB(7, 8, 17));
    gfx_triangle(147, 98, 209, 70, 239, 98, GFX_RGB(7, 8, 17));

    /* Main keep, high towers, crenellations and narrow moonlit windows. */
    gfx_rect(67, 46, 107, 61, STONE_DARK);
    gfx_rect(75, 39, 20, 68, STONE);
    gfx_rect(93, 31, 24, 76, STONE_DARK);
    gfx_rect(120, 42, 25, 65, STONE);
    gfx_rect(145, 34, 20, 73, STONE_DARK);
    gfx_rect(58, 58, 18, 49, STONE_DARK);
    gfx_rect(164, 53, 18, 54, STONE_DARK);
    for (i = 0; i < 5; ++i) {
        gfx_rect(76 + i * 4, 35, 3, 7, STONE_LIGHT);
        gfx_rect(96 + i * 5, 27, 4, 8, STONE);
        gfx_rect(147 + i * 3, 30, 3, 7, STONE);
    }
    gfx_rect(82, 62, 7, 13, GFX_RGB(7, 13, 23));
    gfx_rect(103, 54, 6, 14, GFX_RGB(8, 14, 25));
    gfx_rect(130, 60, 6, 14, GFX_RGB(8, 14, 25));
    gfx_rect(151, 55, 6, 14, GFX_RGB(8, 14, 25));
    gfx_rect(109, 71, 25, 38, INK);
    gfx_rect(112, 74, 19, 35, BROWN);
    gfx_rect(118, 82, 7, 27, GOLD_DARK);
    gfx_rect(120, 83, 3, 26, INK);
    gfx_rect(0, 105, SCREEN_W, 8, GFX_RGB(8, 9, 15));
    gfx_line(0, 111, SCREEN_W, 111, STONE);
    for (i = 0; i < 7; ++i) gfx_line(12 + i * 32, 113, 35 + i * 27, 104, STONE_DARK);
    gfx_line(0, 122, 112, 112, STONE_DARK);
    gfx_line(240, 122, 132, 112, STONE_DARK);
}

static void draw_hallway(void) {
    int i;
    gfx_clear(GFX_RGB(4, 4, 10));
    gfx_rect(0, 0, SCREEN_W, 72, GFX_RGB(7, 7, 15));
    gfx_triangle(0, 0, 120, 25, 0, 72, GFX_RGB(10, 10, 20));
    gfx_triangle(240, 0, 120, 25, 240, 72, GFX_RGB(10, 10, 20));
    gfx_rect(95, 22, 50, 56, GFX_RGB(3, 4, 10));
    gfx_rect(103, 28, 34, 48, GFX_RGB(6, 7, 14));

    /* Receding pillars give the corridor perspective. */
    for (i = 0; i < 4; ++i) {
        int y = 24 + i * 10;
        int w = 5 + i * 3;
        int xx = 28 - i * 7;
        gfx_rect(xx, y, w, 66 - y, STONE_DARK);
        gfx_rect(SCREEN_W - xx - w, y, w, 66 - y, STONE_DARK);
        gfx_line(xx, y, xx + w, y, STONE_LIGHT);
        gfx_line(SCREEN_W - xx - w, y, SCREEN_W - xx, y, STONE_LIGHT);
    }
    /* Torches and their warm halos. */
    gfx_ellipse(35, 48, 8, 10, GFX_RGB(12, 6, 3));
    gfx_ellipse(205, 48, 8, 10, GFX_RGB(12, 6, 3));
    gfx_rect(33, 42, 4, 12, BROWN);
    gfx_rect(203, 42, 4, 12, BROWN);
    gfx_triangle(35, 43, 30, 51, 40, 51, ORANGE);
    gfx_triangle(205, 43, 200, 51, 210, 51, ORANGE);
    gfx_pixel(35, 45, FIRE); gfx_pixel(205, 45, FIRE);

    gfx_triangle(0, 67, 120, 49, 240, 67, STONE_DARK);
    gfx_triangle(0, 111, 120, 65, 240, 111, GFX_RGB(8, 8, 14));
    for (i = 0; i < 8; ++i) {
        int y = 75 + i * 5;
        gfx_line(0, y, 240, y, i % 2 ? STONE_DARK : STONE);
    }
    for (i = 0; i < 8; ++i) gfx_line(120, 66, i * 32, 112, STONE_DARK);
    gfx_line(0, 112, 240, 112, STONE);
}

static void draw_chamber_background(void) {
    int i;
    gfx_clear(GFX_RGB(2, 3, 9));
    gfx_rect(0, 0, 240, 76, GFX_RGB(5, 5, 13));
    /* High arch and ribbed ceiling. */
    gfx_ellipse(120, 20, 65, 43, GFX_RGB(10, 9, 18));
    gfx_ellipse(120, 20, 54, 35, GFX_RGB(3, 4, 10));
    gfx_line(120, 0, 120, 64, STONE_DARK);
    gfx_line(58, 14, 84, 70, STONE_DARK);
    gfx_line(182, 14, 156, 70, STONE_DARK);
    gfx_line(31, 40, 57, 71, STONE_DARK);
    gfx_line(209, 40, 183, 71, STONE_DARK);

    /* A broken stained-glass window glows behind the throne. */
    gfx_rect(99, 10, 42, 48, INK);
    gfx_triangle(101, 55, 120, 13, 120, 55, PURPLE);
    gfx_triangle(120, 13, 139, 55, 120, 55, GFX_RGB(14, 6, 19));
    gfx_line(120, 12, 120, 57, GOLD_DARK);
    gfx_line(100, 36, 140, 36, GOLD_DARK);
    gfx_line(106, 21, 134, 49, STONE_DARK);
    gfx_line(134, 21, 106, 49, STONE_DARK);

    /* Side pillars, faded banners, and the dragon's dais. */
    for (i = 0; i < 3; ++i) {
        int x = 9 + i * 14;
        gfx_rect(x, 28 - i * 2, 7, 47 + i * 2, STONE_DARK);
        gfx_rect(x + 1, 30 - i * 2, 2, 42 + i * 2, STONE);
        gfx_rect(224 - i * 14, 28 - i * 2, 7, 47 + i * 2, STONE_DARK);
        gfx_rect(225 - i * 14, 30 - i * 2, 2, 42 + i * 2, STONE);
    }
    gfx_triangle(140, 80, 180, 57, 220, 80, GFX_RGB(7, 6, 13));
    gfx_rect(149, 78, 62, 10, GFX_RGB(7, 6, 13));
    gfx_line(149, 78, 211, 78, GOLD_DARK);

    gfx_triangle(0, 75, 120, 62, 240, 75, STONE_DARK);
    gfx_rect(0, 75, 240, 38, GFX_RGB(7, 7, 13));
    for (i = 0; i < 7; ++i) {
        int y = 78 + i * 5;
        gfx_line(0, y, 240, y, i % 2 ? STONE_DARK : GFX_RGB(11, 10, 16));
    }
    for (i = 0; i < 8; ++i) gfx_line(120, 76, i * 34, 113, STONE_DARK);
}

static void draw_scene(void) {
    int i;
    if (current_scene == 0) draw_exterior();
    else if (current_scene == 1) draw_hallway();
    else draw_chamber_background();

    for (i = 0; i < 4; ++i) {
        int feet = HERO_FEET;
        if (current_scene == 2) feet = 109;
        draw_hero(i, hero_x[i], feet, i + hero_x[0]);
    }
    if (current_scene == 2) draw_dragon(185, 107, 0);
}

static void draw_dialogue(const char *speaker, const char *line1, const char *line2) {
    draw_scene();
    gfx_rect(0, 113, 240, 47, PANEL);
    gfx_rect(0, 113, 240, 1, PANEL_EDGE);
    gfx_rect(3, 116, 234, 1, STONE_DARK);
    gfx_text(8, 118, speaker, GOLD);
    gfx_text(8, 128, line1, WHITE);
    gfx_text(8, 138, line2, WHITE);
    gfx_text(157, 149, "A CONTINUE", MOON);
}

static void wait_confirm(void) {
    for (;;) {
        uint16_t keys;
        gfx_wait_vblank();
        keys = gfx_poll_keys();
        if (keys & (GFX_KEY_A | GFX_KEY_START)) return;
    }
}

static void dialogue_page(const char *speaker, const char *line1, const char *line2) {
    draw_dialogue(speaker, line1, line2);
    wait_confirm();
}

static void fade_out(void) {
    int i;
    for (i = 0; i < 5; ++i) {
        gfx_wait_frames(2);
        gfx_rect(0, 0, 240, 160, i < 2 ? GFX_RGB(1, 2, 5) : BLACK);
    }
    gfx_clear(BLACK);
    gfx_wait_frames(5);
}

static void title_card(void) {
    int i;
    gfx_clear(NIGHT);
    gfx_ellipse(120, 39, 26, 26, GFX_RGB(4, 6, 15));
    gfx_ellipse(120, 36, 21, 21, MOON);
    gfx_ellipse(129, 29, 19, 19, NIGHT);
    gfx_triangle(35, 111, 85, 59, 126, 111, STONE_DARK);
    gfx_triangle(110, 111, 158, 49, 208, 111, STONE_DARK);
    gfx_rect(68, 82, 105, 39, INK);
    gfx_rect(76, 73, 18, 48, INK);
    gfx_rect(97, 66, 22, 55, INK);
    gfx_rect(143, 77, 20, 44, INK);
    gfx_rect(114, 91, 15, 30, GFX_RGB(15, 9, 4));
    gfx_text(48, 19, "DRAGON CASTLE", GOLD);
    gfx_text(46, 135, "FOUR HEROES - ONE DRAGON", WHITE);
    gfx_text(71, 147, "BETA / TEST 1", MOON);
    for (i = 0; i < 40; ++i) gfx_wait_frames(1);
}

static void walk_group(int scene, int first_start, int first_end, int frames) {
    int t, i;
    current_scene = scene;
    for (t = 0; t <= frames; ++t) {
        int base = first_start + ((first_end - first_start) * t) / frames;
        for (i = 0; i < 4; ++i) hero_x[i] = base - i * 15;
        draw_scene();
        gfx_wait_frames(3);
    }
}

static void run_cinematic(void) {
    title_card();

    /* The four enter through the outer gate. */
    current_scene = 0;
    walk_group(0, 14, 121, 15);
    dialogue_page("ZARYNDRA", "LAS PUERTAS SE ABRIERON SOLAS.", "ESO NUNCA ES UNA BUENA SENAL.");
    dialogue_page("EDRIC", "EL DRAGON NOS ESPERA DENTRO.", "NO NOS IREMOS SIN ENFRENTARLO.");
    dialogue_page("KENDRA", "PUES DEJEMOS DE HABLAR.", "MIS BRAZOS YA PIDEN ACCION.");

    fade_out();
    current_scene = 1;
    walk_group(1, 17, 128, 14);
    dialogue_page("VAELIS", "SUS LLAMAS CORROMPIERON LA PIEDRA.", "HASTA LA MAGIA AQUI ESTA HERIDA.");
    dialogue_page("ZARYNDRA", "PUEDO SENTIR SU FURIA.", "Y EL TAMBIEN DEBE SENTIRNOS.");

    fade_out();
    current_scene = 2;
    walk_group(2, 18, 73, 9);
    dialogue_page("DRAGON", "HABEIS CRUZADO MI UMBRAL.", "VUESTROS REINOS YA ME PERTENECEN.");
    dialogue_page("EDRIC", "NO VENIMOS A NEGOCIAR.", "HEMOS VENIDO A TERMINAR ESTO.");
    dialogue_page("DRAGON", "ENTONCES... QUE ARDA VUESTRA ESPERANZA.", "MOSTRADME DE QUE ESTAIS HECHOS.");
    fade_out();
}

/* --- Turn-based combat model. Each hero owns exactly four abilities. --- */
enum Effect {
    EFFECT_DAMAGE,
    EFFECT_SHIELD_SELF,
    EFFECT_BUFF_PARTY,
    EFFECT_STUN_HIT,
    EFFECT_HEAL,
    EFFECT_SHIELD_PARTY
};

struct Ability {
    const char *name;
    int power;
    int mp_cost;
    int accuracy;
    Effect effect;
    int secondary; /* Buff amount, shield amount, or chance to stun. */
};

struct Hero {
    const char *name;
    const char *race;
    int hp, max_hp;
    int mp, max_mp;
    int attack, defense;
    int shield;
    int attack_buff;
    Ability skills[4];
};

static const Ability EDRIC_SKILLS[4] = {
    {"CORTE REAL", 24, 0, 96, EFFECT_DAMAGE, 0},
    {"ESTOCADA SOLAR", 39, 9, 88, EFFECT_DAMAGE, 0},
    {"GUARDIA FIRME", 30, 5, 100, EFFECT_SHIELD_SELF, 30},
    {"GRITO DE VALOR", 5, 10, 100, EFFECT_BUFF_PARTY, 5}
};
static const Ability ZARYNDRA_SKILLS[4] = {
    {"GARRA DRACONICA", 28, 0, 96, EFFECT_DAMAGE, 0},
    {"ALIENTO CARMESI", 45, 14, 82, EFFECT_DAMAGE, 0},
    {"COLA SISMICA", 23, 8, 90, EFFECT_STUN_HIT, 30},
    {"ESCAMAS ANCESTRALES", 28, 9, 100, EFFECT_SHIELD_SELF, 28}
};
static const Ability VAELIS_SKILLS[4] = {
    {"FLECHA LUNAR", 22, 0, 99, EFFECT_DAMAGE, 0},
    {"LANZA DE CRISTAL", 38, 12, 91, EFFECT_DAMAGE, 0},
    {"RAIZ VINCULANTE", 18, 17, 85, EFFECT_STUN_HIT, 45},
    {"PULSO SANADOR", 40, 15, 100, EFFECT_HEAL, 22}
};
static const Ability KENDRA_SKILLS[4] = {
    {"GOLPE DE RUNA", 30, 0, 94, EFFECT_DAMAGE, 0},
    {"FISURA DE GRANITO", 44, 12, 82, EFFECT_DAMAGE, 0},
    {"MARTILLO PESADO", 22, 8, 89, EFFECT_STUN_HIT, 20},
    {"FORJA PROTECTORA", 14, 10, 100, EFFECT_SHIELD_PARTY, 14}
};

static Hero party[4];
static int dragon_hp = 350;
static const int dragon_max_hp = 350;
static int dragon_stunned = 0;
static int round_number = 1;
static unsigned long random_state = 0xD12A60A7UL;

static int random_range(int upper) {
    random_state = random_state * 1664525UL + 1013904223UL;
    return upper > 0 ? (int)((random_state >> 16) % (unsigned long)upper) : 0;
}

static void setup_party(void) {
    int i;
    party[0].name = "EDRIC"; party[0].race = "HUMAN";
    party[0].max_hp = party[0].hp = 125; party[0].max_mp = party[0].mp = 35;
    party[0].attack = 7; party[0].defense = 8; party[0].shield = 0; party[0].attack_buff = 0;
    party[1].name = "ZARYNDRA"; party[1].race = "DRAGON";
    party[1].max_hp = party[1].hp = 108; party[1].max_mp = party[1].mp = 48;
    party[1].attack = 10; party[1].defense = 5; party[1].shield = 0; party[1].attack_buff = 0;
    party[2].name = "VAELIS"; party[2].race = "ELF";
    party[2].max_hp = party[2].hp = 88; party[2].max_mp = party[2].mp = 74;
    party[2].attack = 5; party[2].defense = 4; party[2].shield = 0; party[2].attack_buff = 0;
    party[3].name = "KENDRA"; party[3].race = "DWARF";
    party[3].max_hp = party[3].hp = 140; party[3].max_mp = party[3].mp = 38;
    party[3].attack = 9; party[3].defense = 9; party[3].shield = 0; party[3].attack_buff = 0;

    for (i = 0; i < 4; ++i) {
        const Ability *skills = i == 0 ? EDRIC_SKILLS : i == 1 ? ZARYNDRA_SKILLS : i == 2 ? VAELIS_SKILLS : KENDRA_SKILLS;
        int j;
        for (j = 0; j < 4; ++j) party[i].skills[j] = skills[j];
    }
    dragon_hp = dragon_max_hp;
    dragon_stunned = 0;
    round_number = 1;
}

static int living_party(void) {
    int i, count = 0;
    for (i = 0; i < 4; ++i) if (party[i].hp > 0) ++count;
    return count;
}

static void draw_health_bar(int x, int y, int width, int height, int value, int maximum, Color fill) {
    int filled;
    if (maximum <= 0) maximum = 1;
    filled = width * value / maximum;
    gfx_rect(x, y, width, height, INK);
    gfx_rect(x + 1, y + 1, width - 2, height - 2, GREY);
    if (filled > 2) gfx_rect(x + 1, y + 1, filled - 1, height - 2, fill);
    gfx_rect(x, y, width, 1, STONE_LIGHT);
}

static void draw_battle_field(int roaring) {
    static const int x_positions[4] = { 23, 47, 71, 95 };
    int i;
    draw_chamber_background();
    gfx_text(7, 3, "DRAGON HP", GOLD);
    {
        char hp_text[8];
        int value = dragon_hp;
        hp_text[0] = (char)('0' + (value / 100) % 10);
        hp_text[1] = (char)('0' + (value / 10) % 10);
        hp_text[2] = (char)('0' + value % 10);
        hp_text[3] = '/';
        hp_text[4] = (char)('0' + (dragon_max_hp / 100) % 10);
        hp_text[5] = (char)('0' + (dragon_max_hp / 10) % 10);
        hp_text[6] = (char)('0' + dragon_max_hp % 10);
        hp_text[7] = 0;
        gfx_text(66, 3, hp_text, MOON);
    }
    draw_health_bar(7, 13, 113, 5, dragon_hp, dragon_max_hp, RED_LIGHT);
    gfx_text(176, 3, "ROUND", WHITE);
    if (round_number < 10) {
        char digit[2]; digit[0] = (char)('0' + round_number); digit[1] = 0;
        gfx_text(217, 3, digit, GOLD);
    } else gfx_text(217, 3, "9", GOLD);

    for (i = 0; i < 4; ++i) {
        if (party[i].hp > 0) {
            draw_hero(i, x_positions[i], 91, (i + round_number) & 1);
            if (party[i].shield > 0) gfx_pixel(x_positions[i], 70, BLUE_LIGHT);
        }
    }
    if (dragon_hp > 0) draw_dragon(184, 91, roaring);
}

static void draw_menu(int hero_index, int selected) {
    Hero *h = &party[hero_index];
    int i;
    draw_battle_field(0);
    gfx_rect(0, 95, 240, 65, PANEL);
    gfx_rect(0, 95, 240, 1, PANEL_EDGE);
    {
        char status[40];
        int p = 0, n, divisor;
        const char *name = h->name;
        while (*name && p < 35) status[p++] = *name++;
        status[p++] = ' '; status[p++] = 'H'; status[p++] = 'P'; status[p++] = ' ';
        for (n = 0, divisor = 100; divisor >= 1; divisor /= 10) {
            int digit = h->hp / divisor;
            if (digit > 9) digit %= 10;
            if (n || digit || divisor == 1) status[p++] = (char)('0' + digit);
            n = 1;
        }
        status[p++] = '/';
        for (n = 0, divisor = 100; divisor >= 1; divisor /= 10) {
            int digit = h->max_hp / divisor;
            if (digit > 9) digit %= 10;
            if (n || digit || divisor == 1) status[p++] = (char)('0' + digit);
            n = 1;
        }
        status[p++] = ' '; status[p++] = 'M'; status[p++] = 'P'; status[p++] = ' ';
        for (n = 0, divisor = 100; divisor >= 1; divisor /= 10) {
            int digit = h->mp / divisor;
            if (digit > 9) digit %= 10;
            if (n || digit || divisor == 1) status[p++] = (char)('0' + digit);
            n = 1;
        }
        status[p++] = '/';
        for (n = 0, divisor = 100; divisor >= 1; divisor /= 10) {
            int digit = h->max_mp / divisor;
            if (digit > 9) digit %= 10;
            if (n || digit || divisor == 1) status[p++] = (char)('0' + digit);
            n = 1;
        }
        status[p] = 0;
        gfx_text(6, 98, status, GOLD);
    }
    gfx_text(5, 108, "HP", HP_GREEN);
    draw_health_bar(18, 109, 86, 4, h->hp, h->max_hp, HP_GREEN);
    gfx_text(119, 108, "MP", MP_BLUE);
    draw_health_bar(132, 109, 94, 4, h->mp, h->max_mp, MP_BLUE);
    for (i = 0; i < 4; ++i) {
        int y = 117 + i * 10;
        Ability *a = &h->skills[i];
        Color text_color = a->mp_cost > h->mp ? GFX_RGB(12, 12, 14) : WHITE;
        if (i == selected) {
            gfx_rect(4, y - 1, 232, 9, GFX_RGB(8, 11, 21));
            gfx_text(7, y, ">", GOLD);
        }
        gfx_text(16, y, a->name, text_color);
        {
            char number[3];
            gfx_text(139, y, "P", MOON);
            number[0] = (char)('0' + (a->power / 10) % 10);
            number[1] = (char)('0' + a->power % 10); number[2] = 0;
            gfx_text(145, y, number, text_color);
            gfx_text(165, y, "M", MOON);
            number[0] = (char)('0' + (a->mp_cost / 10) % 10);
            number[1] = (char)('0' + a->mp_cost % 10); number[2] = 0;
            gfx_text(171, y, number, text_color);
            gfx_text(191, y, "A", MOON);
            number[0] = (char)('0' + (a->accuracy / 10) % 10);
            number[1] = (char)('0' + a->accuracy % 10); number[2] = 0;
            gfx_text(197, y, number, text_color);
        }
    }
}

static void draw_battle_message(const char *line1, const char *line2, const char *line3, int roaring) {
    draw_battle_field(roaring);
    gfx_rect(0, 95, 240, 65, PANEL);
    gfx_rect(0, 95, 240, 1, PANEL_EDGE);
    gfx_text(7, 99, line1, GOLD);
    gfx_text(7, 113, line2, WHITE);
    gfx_text(7, 125, line3, MOON);
    gfx_text(170, 146, "A CONTINUE", GOLD);
}

static void show_action(const char *line1, const char *line2, const char *line3, int roaring) {
    draw_battle_message(line1, line2, line3, roaring);
    wait_confirm();
}

static void damage_dragon(int damage) {
    dragon_hp -= damage;
    if (dragon_hp < 0) dragon_hp = 0;
}

static int damage_hero(int index, int damage) {
    int absorbed = 0;
    if (party[index].shield > 0) {
        absorbed = party[index].shield < damage ? party[index].shield : damage;
        party[index].shield -= absorbed;
        damage -= absorbed;
    }
    if (damage > 0) party[index].hp -= damage;
    if (party[index].hp < 0) party[index].hp = 0;
    return absorbed;
}

static int choose_heal_target(void) {
    int i;
    /* Revive a fallen ally first. */
    for (i = 0; i < 4; ++i) if (party[i].hp == 0) return i;
    {
        int target = 0;
        for (i = 1; i < 4; ++i) {
            if (party[i].hp * party[target].max_hp < party[target].hp * party[i].max_hp) target = i;
        }
        return target;
    }
}

static void use_ability(int hi, int ai) {
    Hero *h = &party[hi];
    Ability *a = &h->skills[ai];
    char line2[40];
    int damage, target, amount;

    h->mp -= a->mp_cost;
    if (h->mp < 0) h->mp = 0;
    line2[0] = 0;

    if (a->effect == EFFECT_DAMAGE || a->effect == EFFECT_STUN_HIT) {
        /* Accuracy is a visible gameplay stat; MP is spent even on a miss. */
        if (random_range(100) >= a->accuracy) {
            show_action(h->name, a->name, "THE ATTACK MISSED!", 0);
            return;
        }
        damage = a->power + h->attack / 2 + h->attack_buff - 4;
        if (damage < 1) damage = 1;
        damage_dragon(damage);
        if (a->effect == EFFECT_STUN_HIT && dragon_hp > 0 && random_range(100) < a->secondary) {
            dragon_stunned = 1;
            show_action(h->name, a->name, "THE DRAGON IS BOUND IN PLACE.", 0);
        } else {
            /* Name is shown separately; keep the second line readable on the GBA font. */
            if (damage >= 100) {
                show_action(h->name, a->name, "THE DRAGON TAKES 99+ DAMAGE.", 0);
            } else {
                line2[0] = 'D'; line2[1] = 'A'; line2[2] = 'M'; line2[3] = 'A'; line2[4] = 'G'; line2[5] = 'E'; line2[6] = ':'; line2[7] = ' '; line2[8] = (char)('0' + (damage / 10) % 10); line2[9] = (char)('0' + damage % 10); line2[10] = 0;
                show_action(h->name, a->name, line2, 0);
            }
        }
    } else if (a->effect == EFFECT_SHIELD_SELF) {
        h->shield += a->power;
        line2[0] = 'S'; line2[1] = 'H'; line2[2] = 'I'; line2[3] = 'E'; line2[4] = 'L'; line2[5] = 'D'; line2[6] = ' '; line2[7] = '+'; line2[8] = (char)('0' + a->power / 10); line2[9] = (char)('0' + a->power % 10); line2[10] = 0;
        show_action(h->name, a->name, line2, 0);
    } else if (a->effect == EFFECT_BUFF_PARTY) {
        int i;
        for (i = 0; i < 4; ++i) if (party[i].hp > 0) party[i].attack_buff = a->secondary;
        show_action(h->name, a->name, "THE PARTY GAINS +5 ATTACK.", 0);
    } else if (a->effect == EFFECT_HEAL) {
        target = choose_heal_target();
        if (party[target].hp == 0) {
            party[target].hp = a->secondary;
            show_action(h->name, a->name, "A FALLEN ALLY RETURNS WITH 22 HP.", 0);
        } else {
            amount = a->power;
            party[target].hp += amount;
            if (party[target].hp > party[target].max_hp) party[target].hp = party[target].max_hp;
            show_action(h->name, a->name, "THE WEAKEST ALLY RECOVERS HP.", 0);
        }
    } else if (a->effect == EFFECT_SHIELD_PARTY) {
        int i;
        for (i = 0; i < 4; ++i) if (party[i].hp > 0) party[i].shield += a->secondary;
        show_action(h->name, a->name, "ALL ALLIES GAIN 14 SHIELD.", 0);
    }
}

static void take_player_turn(int hi) {
    int selected = 0;
    for (;;) {
        uint16_t keys;
        if (party[hi].hp <= 0 || dragon_hp <= 0) return;
        draw_menu(hi, selected);
        gfx_wait_vblank();
        keys = gfx_poll_keys();
        if (keys & GFX_KEY_UP) selected = (selected + 3) & 3;
        if (keys & GFX_KEY_DOWN) selected = (selected + 1) & 3;
        if (keys & GFX_KEY_B) {
            draw_battle_message("SELECT A SKILL", "UP / DOWN CHANGES THE SKILL.", "A CONFIRMS YOUR CHOICE.", 0);
            wait_confirm();
        }
        if (keys & (GFX_KEY_A | GFX_KEY_START)) {
            if (party[hi].skills[selected].mp_cost > party[hi].mp) {
                draw_battle_message("NOT ENOUGH MP!", "CHOOSE A CHEAPER SKILL.", "YOUR TURN HAS NOT BEEN USED.", 0);
                wait_confirm();
            } else {
                use_ability(hi, selected);
                return;
            }
        }
    }
}

static int dragon_turn(void) {
    int target, damage, absorbed;
    char line2[40];
    if (dragon_hp <= 0 || living_party() == 0) return 0;
    if (dragon_stunned) {
        dragon_stunned = 0;
        show_action("THE DRAGON STAGGERS!", "IT CANNOT ATTACK THIS TURN.", "THE PARTY HOLDS ITS GROUND.", 0);
        ++round_number;
        return 1;
    }

    if (round_number % 3 == 0) {
        int i;
        for (i = 0; i < 4; ++i) {
            if (party[i].hp > 0) {
                damage = 14 + random_range(7) - party[i].defense / 4;
                if (damage < 6) damage = 6;
                (void)damage_hero(i, damage);
            }
        }
        show_action("THE DRAGON USES BLACK FLAME!", "FIRE SWEEPS ACROSS THE PARTY.", "CHECK EACH ALLY'S HP NEXT TURN.", 1);
    } else {
        int tries = 0;
        do { target = random_range(4); ++tries; } while (party[target].hp <= 0 && tries < 12);
        if (party[target].hp <= 0) {
            for (target = 0; target < 4 && party[target].hp <= 0; ++target) { }
            if (target >= 4) return 0;
        }
        damage = 22 + random_range(8) - party[target].defense / 3;
        if (damage < 7) damage = 7;
        absorbed = damage_hero(target, damage);
        line2[0] = 'T'; line2[1] = 'A'; line2[2] = 'R'; line2[3] = 'G'; line2[4] = 'E'; line2[5] = 'T'; line2[6] = ':'; line2[7] = ' '; 
        {
            int p = 8, k = 0;
            while (party[target].name[k] && p < 30) line2[p++] = party[target].name[k++];
            line2[p++] = ' '; line2[p++] = '-'; line2[p++] = ' '; line2[p++] = (char)('0' + (damage / 10) % 10); line2[p++] = (char)('0' + damage % 10); line2[p++] = ' '; line2[p++] = 'D'; line2[p++] = 'M'; line2[p++] = 'G'; line2[p] = 0;
        }
        show_action("THE DRAGON STRIKES!", line2, absorbed > 0 ? "A SHIELD SOAKS PART OF THE HIT." : "", 1);
    }
    ++round_number;
    return living_party() > 0;
}

static void victory_screen(void) {
    int i;
    draw_chamber_background();
    for (i = 0; i < 4; ++i) if (party[i].hp > 0) draw_hero(i, 36 + i * 24, 106, 0);
    /* The fallen dragon rests at the far end of the hall. */
    draw_dragon(185, 107, 0);
    gfx_rect(0, 113, 240, 47, PANEL);
    gfx_rect(0, 113, 240, 1, GOLD);
    gfx_text(8, 119, "THE DRAGON HAS FALLEN.", GOLD);
    gfx_text(8, 131, "THE CASTLE FALLS SILENT AGAIN.", WHITE);
    gfx_text(8, 143, "DRAGON CASTLE - BETA / TEST 1", MOON);
    wait_confirm();
}

static void defeat_screen(void) {
    draw_battle_field(0);
    gfx_rect(0, 95, 240, 65, PANEL);
    gfx_rect(0, 95, 240, 1, RED_LIGHT);
    gfx_text(8, 107, "THE PARTY HAS FALLEN...", RED_LIGHT);
    gfx_text(8, 121, "THE DRAGON STILL RULES THIS KEEP.", WHITE);
    gfx_text(8, 139, "A: TRY AGAIN", GOLD);
    wait_confirm();
}

static int run_battle(void) {
    int round_alive;
    setup_party();
    show_action("THE BATTLE BEGINS!", "FOUR ALLIES, FOUR TURNS.", "THEN THE DRAGON WILL STRIKE.", 0);

    while (dragon_hp > 0 && living_party() > 0) {
        int i;
        round_alive = 1;
        for (i = 0; i < 4; ++i) {
            if (party[i].hp > 0 && dragon_hp > 0) take_player_turn(i);
            if (dragon_hp <= 0) break;
        }
        if (dragon_hp <= 0 || living_party() == 0) break;
        round_alive = dragon_turn();
        for (i = 0; i < 4; ++i) if (party[i].attack_buff > 0) party[i].attack_buff = 0;
        if (!round_alive) break;
    }

    if (dragon_hp <= 0) { victory_screen(); return 1; }
    defeat_screen();
    return 0;
}

extern "C" int main(void) {
    gfx_init();
    for (;;) {
        run_cinematic();
        (void)run_battle();
        /* A confirmation after the result starts the tale from its opening. */
    }
    return 0;
}
