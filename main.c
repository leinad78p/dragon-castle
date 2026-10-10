/*
 * Dragon Castle - tiny turn-based GBA homebrew
 * Source code in C. Built for Game Boy Advance with devkitARM/devkitPro.
 * The game uses Mode 3 graphics and a small built-in 5x7 bitmap font.
 */
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define SCREEN_W 240
#define SCREEN_H 160
#define MODE3 3
#define BG2_ENABLE (1u << 10)

#define REG_DISPCNT (*(volatile u16 *)0x04000000)
#define REG_VCOUNT  (*(volatile u16 *)0x04000006)
#define REG_KEYINPUT (*(volatile u16 *)0x04000130)
#define VRAM ((volatile u16 *)0x06000000)

#define KEY_A      (1u << 0)
#define KEY_B      (1u << 1)
#define KEY_SELECT (1u << 2)
#define KEY_START  (1u << 3)
#define KEY_RIGHT  (1u << 4)
#define KEY_LEFT   (1u << 5)
#define KEY_UP     (1u << 6)
#define KEY_DOWN   (1u << 7)

#define RGB15(r,g,b) ((u16)(((r) & 31) | (((g) & 31) << 5) | (((b) & 31) << 10)))

#define C_BLACK      RGB15(0, 0, 0)
#define C_WHITE      RGB15(31, 31, 31)
#define C_SKY        RGB15(5, 7, 18)
#define C_SKY2       RGB15(9, 13, 29)
#define C_STONE      RGB15(13, 15, 23)
#define C_STONE2     RGB15(19, 21, 30)
#define C_STONE_DARK RGB15(7, 8, 15)
#define C_GOLD       RGB15(31, 24, 5)
#define C_YELLOW     RGB15(31, 30, 13)
#define C_RED        RGB15(29, 5, 5)
#define C_RED_DARK   RGB15(14, 2, 3)
#define C_GREEN      RGB15(4, 25, 8)
#define C_GREEN2     RGB15(8, 31, 12)
#define C_BLUE       RGB15(6, 13, 30)
#define C_BLUE2      RGB15(10, 22, 31)
#define C_SKIN       RGB15(29, 18, 12)
#define C_HAIR       RGB15(10, 5, 3)
#define C_PURPLE     RGB15(16, 5, 22)
#define C_ORANGE     RGB15(31, 12, 2)
#define C_FIRE       RGB15(31, 25, 3)
#define C_PANEL      RGB15(3, 4, 12)
#define C_BORDER     RGB15(23, 21, 14)

/* 5x7 font: each byte is a vertical column; bits 0..6 run top to bottom. */
static const u8 font[91][5] = {
    ['!'-32] = {0x00,0x00,0x5F,0x00,0x00},
    ['\''-32] = {0x00,0x03,0x00,0x00,0x00},
    [','-32] = {0x00,0x50,0x30,0x00,0x00},
    ['-'-32] = {0x08,0x08,0x08,0x08,0x08},
    ['.'-32] = {0x00,0x60,0x60,0x00,0x00},
    ['/'-32] = {0x20,0x10,0x08,0x04,0x02},
    ['0'-32] = {0x3E,0x51,0x49,0x45,0x3E},
    ['1'-32] = {0x00,0x42,0x7F,0x40,0x00},
    ['2'-32] = {0x42,0x61,0x51,0x49,0x46},
    ['3'-32] = {0x21,0x41,0x45,0x4B,0x31},
    ['4'-32] = {0x18,0x14,0x12,0x7F,0x10},
    ['5'-32] = {0x27,0x45,0x45,0x45,0x39},
    ['6'-32] = {0x3C,0x4A,0x49,0x49,0x30},
    ['7'-32] = {0x01,0x71,0x09,0x05,0x03},
    ['8'-32] = {0x36,0x49,0x49,0x49,0x36},
    ['9'-32] = {0x06,0x49,0x49,0x29,0x1E},
    [':'-32] = {0x00,0x36,0x36,0x00,0x00},
    ['?'-32] = {0x02,0x01,0x51,0x09,0x06},
    ['A'-32] = {0x7E,0x11,0x11,0x11,0x7E},
    ['B'-32] = {0x7F,0x49,0x49,0x49,0x36},
    ['C'-32] = {0x3E,0x41,0x41,0x41,0x22},
    ['D'-32] = {0x7F,0x41,0x41,0x22,0x1C},
    ['E'-32] = {0x7F,0x49,0x49,0x49,0x41},
    ['F'-32] = {0x7F,0x09,0x09,0x09,0x01},
    ['G'-32] = {0x3E,0x41,0x49,0x49,0x7A},
    ['H'-32] = {0x7F,0x08,0x08,0x08,0x7F},
    ['I'-32] = {0x00,0x41,0x7F,0x41,0x00},
    ['J'-32] = {0x20,0x40,0x41,0x3F,0x01},
    ['K'-32] = {0x7F,0x08,0x14,0x22,0x41},
    ['L'-32] = {0x7F,0x40,0x40,0x40,0x40},
    ['M'-32] = {0x7F,0x02,0x0C,0x02,0x7F},
    ['N'-32] = {0x7F,0x04,0x08,0x10,0x7F},
    ['O'-32] = {0x3E,0x41,0x41,0x41,0x3E},
    ['P'-32] = {0x7F,0x09,0x09,0x09,0x06},
    ['Q'-32] = {0x3E,0x41,0x51,0x21,0x5E},
    ['R'-32] = {0x7F,0x09,0x19,0x29,0x46},
    ['S'-32] = {0x46,0x49,0x49,0x49,0x31},
    ['T'-32] = {0x01,0x01,0x7F,0x01,0x01},
    ['U'-32] = {0x3F,0x40,0x40,0x40,0x3F},
    ['V'-32] = {0x1F,0x20,0x40,0x20,0x1F},
    ['W'-32] = {0x3F,0x40,0x38,0x40,0x3F},
    ['X'-32] = {0x63,0x14,0x08,0x14,0x63},
    ['Y'-32] = {0x07,0x08,0x70,0x08,0x07},
    ['Z'-32] = {0x61,0x51,0x49,0x45,0x43},
    ['a'-32] = {0x20,0x54,0x54,0x54,0x78},
    ['b'-32] = {0x7F,0x48,0x44,0x44,0x38},
    ['c'-32] = {0x38,0x44,0x44,0x44,0x20},
    ['d'-32] = {0x38,0x44,0x44,0x48,0x7F},
    ['e'-32] = {0x38,0x54,0x54,0x54,0x18},
    ['f'-32] = {0x08,0x7E,0x09,0x01,0x02},
    ['g'-32] = {0x0C,0x52,0x52,0x52,0x3E},
    ['h'-32] = {0x7F,0x08,0x04,0x04,0x78},
    ['i'-32] = {0x00,0x44,0x7D,0x40,0x00},
    ['j'-32] = {0x20,0x40,0x44,0x3D,0x00},
    ['k'-32] = {0x7F,0x10,0x28,0x44,0x00},
    ['l'-32] = {0x00,0x41,0x7F,0x40,0x00},
    ['m'-32] = {0x7C,0x04,0x18,0x04,0x78},
    ['n'-32] = {0x7C,0x08,0x04,0x04,0x78},
    ['o'-32] = {0x38,0x44,0x44,0x44,0x38},
    ['p'-32] = {0x7C,0x14,0x14,0x14,0x08},
    ['q'-32] = {0x08,0x14,0x14,0x18,0x7C},
    ['r'-32] = {0x7C,0x08,0x04,0x04,0x08},
    ['s'-32] = {0x48,0x54,0x54,0x54,0x20},
    ['t'-32] = {0x04,0x3F,0x44,0x40,0x20},
    ['u'-32] = {0x3C,0x40,0x40,0x20,0x7C},
    ['v'-32] = {0x1C,0x20,0x40,0x20,0x1C},
    ['w'-32] = {0x3C,0x40,0x30,0x40,0x3C},
    ['x'-32] = {0x44,0x28,0x10,0x28,0x44},
    ['y'-32] = {0x0C,0x50,0x50,0x50,0x3C},
    ['z'-32] = {0x44,0x64,0x54,0x4C,0x44}
};

typedef enum {
    SCREEN_TITLE,
    SCREEN_INTRO,
    SCREEN_BATTLE,
    SCREEN_GAME_OVER,
    SCREEN_CREDITS
} ScreenState;

static ScreenState screenState = SCREEN_TITLE;
static int heroHP, dragonHP, potions, menuChoice, guarding, battlePhase;
static u32 randomSeed = 0xC0FFEEu;
static u16 previousKeys = 0;
static char message1[40] = "THE DRAGON AWAITS...";
static char message2[40] = "PRESS A TO CONTINUE";

static void waitVBlank(void) {
    while (REG_VCOUNT >= 160) { }
    while (REG_VCOUNT < 160) { }
}

static u16 keysDown(void) {
    u16 current = (u16)(~REG_KEYINPUT) & 0x03FF;
    u16 down = current & (u16)(~previousKeys);
    previousKeys = current;
    return down;
}

static void pixel(int x, int y, u16 color) {
    if ((unsigned)x < SCREEN_W && (unsigned)y < SCREEN_H)
        VRAM[y * SCREEN_W + x] = color;
}

static void fillRect(int x, int y, int w, int h, u16 color) {
    int yy, xx;
    if (w <= 0 || h <= 0) return;
    for (yy = y; yy < y + h; ++yy)
        for (xx = x; xx < x + w; ++xx)
            pixel(xx, yy, color);
}

static void outlineRect(int x, int y, int w, int h, u16 color) {
    int i;
    for (i = 0; i < w; ++i) {
        pixel(x + i, y, color);
        pixel(x + i, y + h - 1, color);
    }
    for (i = 0; i < h; ++i) {
        pixel(x, y + i, color);
        pixel(x + w - 1, y + i, color);
    }
}

static void line(int x0, int y0, int x1, int y1, u16 color) {
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = (y1 > y0) ? (y0 - y1) : (y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        {
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
}

static void circle(int cx, int cy, int r, u16 color) {
    int x, y;
    for (y = -r; y <= r; ++y)
        for (x = -r; x <= r; ++x)
            if (x*x + y*y <= r*r) pixel(cx + x, cy + y, color);
}

static int textLength(const char *s) {
    int n = 0;
    while (s[n]) ++n;
    return n;
}

static void drawText(int x, int y, const char *s, u16 color, int scale) {
    while (*s) {
        unsigned char ch = (unsigned char)*s++;
        int col, row;
        if (ch < 32 || ch > 122) ch = ' ';
        for (col = 0; col < 5; ++col) {
            u8 bits = font[ch - 32][col];
            for (row = 0; row < 7; ++row)
                if (bits & (1u << row))
                    fillRect(x + col * scale, y + row * scale, scale, scale, color);
        }
        x += 6 * scale;
    }
}

static void centeredText(int y, const char *s, u16 color, int scale) {
    int width = textLength(s) * 6 * scale - scale;
    drawText((SCREEN_W - width) / 2, y, s, color, scale);
}

static void panel(int x, int y, int w, int h) {
    fillRect(x, y, w, h, C_PANEL);
    outlineRect(x, y, w, h, C_BORDER);
}

static void drawBar(int x, int y, int w, int h, int current, int maximum, u16 color) {
    int inner = w - 2;
    int filled = (maximum > 0) ? (inner * current) / maximum : 0;
    fillRect(x, y, w, h, C_RED_DARK);
    if (filled > 0) fillRect(x + 1, y + 1, filled, h - 2, color);
    outlineRect(x, y, w, h, C_WHITE);
}

static void drawCastle(int x, int y, int scale) {
    int s = scale;
    u16 stone = C_STONE2;
    fillRect(x + 13*s, y + 20*s, 42*s, 31*s, stone);
    fillRect(x + 5*s, y + 4*s, 15*s, 47*s, stone);
    fillRect(x + 48*s, y + 4*s, 15*s, 47*s, stone);
    fillRect(x + 3*s, y, 19*s, 7*s, stone);
    fillRect(x + 46*s, y, 19*s, 7*s, stone);
    fillRect(x + 10*s, y + 15*s, 5*s, 10*s, C_STONE_DARK);
    fillRect(x + 53*s, y + 15*s, 5*s, 10*s, C_STONE_DARK);
    fillRect(x + 29*s, y + 32*s, 11*s, 19*s, C_STONE_DARK);
    fillRect(x + 0*s, y + 51*s, 68*s, 4*s, C_STONE_DARK);
    line(x + 13*s, y + 20*s, x + 55*s, y + 20*s, C_WHITE);
}

static void drawBoy(int x, int y, int s) {
    /* Hair, face, tunic, hands, boots and a little bright sword. */
    fillRect(x + 5*s, y, 7*s, 3*s, C_HAIR);
    fillRect(x + 4*s, y + 2*s, 2*s, 5*s, C_HAIR);
    fillRect(x + 5*s, y + 3*s, 7*s, 5*s, C_SKIN);
    fillRect(x + 10*s, y + 4*s, 1*s, 1*s, C_BLACK);
    fillRect(x + 4*s, y + 7*s, 9*s, 2*s, C_HAIR);
    fillRect(x + 4*s, y + 9*s, 9*s, 8*s, C_BLUE);
    fillRect(x + 2*s, y + 10*s, 3*s, 6*s, C_BLUE2);
    fillRect(x + 12*s, y + 10*s, 3*s, 5*s, C_BLUE2);
    fillRect(x + 3*s, y + 15*s, 3*s, 3*s, C_SKIN);
    fillRect(x + 12*s, y + 14*s, 3*s, 3*s, C_SKIN);
    fillRect(x + 5*s, y + 17*s, 3*s, 7*s, C_STONE_DARK);
    fillRect(x + 10*s, y + 17*s, 3*s, 7*s, C_STONE_DARK);
    fillRect(x + 4*s, y + 23*s, 5*s, 2*s, C_HAIR);
    fillRect(x + 9*s, y + 23*s, 5*s, 2*s, C_HAIR);
    line(x + 15*s, y + 15*s, x + 20*s, y + 9*s, C_WHITE);
    line(x + 14*s, y + 14*s, x + 19*s, y + 8*s, C_GOLD);
}

static void drawDragon(int x, int y, int s) {
    int i;
    /* Wings and tail behind the body. */
    line(x + 13*s, y + 14*s, x + 1*s, y + 1*s, C_PURPLE);
    line(x + 1*s, y + 1*s, x + 5*s, y + 18*s, C_PURPLE);
    line(x + 5*s, y + 18*s, x + 13*s, y + 14*s, C_PURPLE);
    line(x + 19*s, y + 14*s, x + 31*s, y + 2*s, C_PURPLE);
    line(x + 31*s, y + 2*s, x + 27*s, y + 20*s, C_PURPLE);
    line(x + 27*s, y + 20*s, x + 19*s, y + 14*s, C_PURPLE);
    line(x + 9*s, y + 20*s, x + 1*s, y + 27*s, C_GREEN);
    line(x + 1*s, y + 27*s, x + 8*s, y + 27*s, C_GREEN);
    fillRect(x + 8*s, y + 12*s, 17*s, 12*s, C_GREEN);
    fillRect(x + 20*s, y + 6*s, 8*s, 13*s, C_GREEN2);
    fillRect(x + 24*s, y + 3*s, 11*s, 8*s, C_GREEN2);
    fillRect(x + 31*s, y + 6*s, 7*s, 4*s, C_GREEN2);
    fillRect(x + 25*s, y, 2*s, 4*s, C_GOLD);
    fillRect(x + 31*s, y + 1*s, 2*s, 4*s, C_GOLD);
    fillRect(x + 29*s, y + 5*s, 2*s, 2*s, C_WHITE);
    fillRect(x + 30*s, y + 5*s, 1*s, 1*s, C_BLACK);
    fillRect(x + 12*s, y + 22*s, 5*s, 6*s, C_GREEN);
    fillRect(x + 22*s, y + 22*s, 5*s, 6*s, C_GREEN);
    fillRect(x + 11*s, y + 27*s, 7*s, 2*s, C_STONE_DARK);
    fillRect(x + 21*s, y + 27*s, 7*s, 2*s, C_STONE_DARK);
    for (i = 0; i < 4; ++i)
        pixel(x + (13 + i*3)*s, y + 11*s, C_YELLOW);
}

static void drawNightBackground(void) {
    int y;
    fillRect(0, 0, SCREEN_W, SCREEN_H, C_SKY);
    for (y = 0; y < 45; y += 8)
        fillRect(0, y, SCREEN_W, 4, C_SKY2);
    circle(204, 23, 9, C_YELLOW);
    circle(208, 20, 8, C_SKY);
    pixel(20, 18, C_WHITE); pixel(46, 28, C_WHITE);
    pixel(62, 12, C_WHITE); pixel(137, 25, C_WHITE);
    pixel(185, 43, C_WHITE); pixel(226, 50, C_WHITE);
    fillRect(0, 111, SCREEN_W, 49, C_STONE_DARK);
    drawCastle(85, 39, 1);
}

static void drawDungeonBackground(void) {
    int y, x;
    fillRect(0, 0, SCREEN_W, SCREEN_H, C_STONE_DARK);
    for (y = 0; y < 78; y += 12) {
        fillRect(0, y, SCREEN_W, 1, C_STONE);
        for (x = (y / 12 % 2) ? 0 : 12; x < SCREEN_W; x += 24)
            fillRect(x, y, 1, 12, C_STONE);
    }
    fillRect(0, 65, SCREEN_W, 14, C_STONE);
    fillRect(0, 79, SCREEN_W, 2, C_STONE_DARK);
    fillRect(0, 80, SCREEN_W, 80, C_BLACK);
    /* Faint torch lights on the walls. */
    fillRect(8, 22, 3, 12, C_GOLD);
    fillRect(7, 18, 5, 5, C_ORANGE);
    fillRect(229, 22, 3, 12, C_GOLD);
    fillRect(228, 18, 5, 5, C_ORANGE);
}

static void copyString(char *dst, const char *src) {
    int i = 0;
    while (src[i] && i < 38) { dst[i] = src[i]; ++i; }
    dst[i] = '\0';
}

static void setMessage(const char *a, const char *b) {
    copyString(message1, a);
    copyString(message2, b);
}

static void appendNumber(char *dst, int number) {
    char reverse[8];
    int n = 0, i;
    if (number == 0) { dst[0] = '0'; dst[1] = '\0'; return; }
    while (number > 0 && n < 7) {
        reverse[n++] = (char)('0' + (number % 10));
        number /= 10;
    }
    for (i = 0; i < n; ++i) dst[i] = reverse[n - i - 1];
    dst[n] = '\0';
}

static void numberMessage(const char *prefix, int number, const char *suffix, char *out) {
    int i = 0, j;
    char digits[8];
    while (prefix[i] && i < 24) { out[i] = prefix[i]; ++i; }
    appendNumber(digits, number);
    for (j = 0; digits[j] && i < 32; ++j) out[i++] = digits[j];
    j = 0;
    while (suffix[j] && i < 38) out[i++] = suffix[j++];
    out[i] = '\0';
}

static int randomRange(int minimum, int maximum) {
    randomSeed = randomSeed * 1664525u + 1013904223u;
    return minimum + (int)((randomSeed >> 16) % (u32)(maximum - minimum + 1));
}

static void startBattle(void) {
    heroHP = 34;
    dragonHP = 46;
    potions = 3;
    menuChoice = 0;
    guarding = 0;
    battlePhase = 0;
    setMessage("THE DRAGON BLOCKS THE WAY!", "CHOOSE YOUR MOVE.");
    screenState = SCREEN_BATTLE;
}

static void enemyTurn(void) {
    int damage = randomRange(5, 10);
    if (guarding) {
        damage = (damage + 1) / 2;
        guarding = 0;
    }
    heroHP -= damage;
    if (heroHP < 0) heroHP = 0;
    numberMessage("DRAGON STRIKES! ", damage, " HP LOST.", message1);
    if (heroHP == 0) {
        copyString(message2, "YOU HAVE FALLEN.");
        battlePhase = 3;
    } else {
        copyString(message2, "PRESS A TO CONTINUE");
        battlePhase = 2;
    }
}

static void chooseBattleAction(void) {
    int amount;
    if (menuChoice == 0) {
        amount = randomRange(7, 12);
        dragonHP -= amount;
        if (dragonHP < 0) dragonHP = 0;
        numberMessage("YOU HIT DRAGON: ", amount, " DAMAGE!", message1);
        if (dragonHP == 0) copyString(message2, "THE DRAGON FALLS!");
        else copyString(message2, "PRESS A TO CONTINUE");
    } else if (menuChoice == 1) {
        if (potions <= 0) {
            setMessage("NO POTIONS LEFT!", "TRY ANOTHER MOVE.");
        } else if (heroHP >= 34) {
            setMessage("YOUR HP IS ALREADY FULL.", "THE DRAGON STILL MOVES.");
        } else {
            amount = 12;
            heroHP += amount;
            if (heroHP > 34) heroHP = 34;
            --potions;
            setMessage("YOU DRINK A POTION.", "YOU RECOVER HP!");
        }
    } else {
        guarding = 1;
        setMessage("YOU RAISE YOUR GUARD.", "DAMAGE WILL BE REDUCED.");
    }
    battlePhase = 1;
}

static void updateGame(u16 keys) {
    if (screenState == SCREEN_TITLE) {
        if (keys & (KEY_A | KEY_START)) screenState = SCREEN_INTRO;
    } else if (screenState == SCREEN_INTRO) {
        if (keys & (KEY_A | KEY_START)) startBattle();
    } else if (screenState == SCREEN_BATTLE) {
        if (battlePhase == 0) {
            if (keys & KEY_UP) menuChoice = (menuChoice + 2) % 3;
            if (keys & KEY_DOWN) menuChoice = (menuChoice + 1) % 3;
            if (keys & KEY_A) chooseBattleAction();
            if (keys & KEY_B) setMessage("A BRAVE BOY STANDS FIRM.", "CHOOSE ATTACK, POTION OR DEFEND.");
        } else if (keys & (KEY_A | KEY_START)) {
            if (battlePhase == 1) {
                if (dragonHP <= 0) {
                    screenState = SCREEN_CREDITS;
                } else {
                    enemyTurn();
                }
            } else if (battlePhase == 2) {
                battlePhase = 0;
                setMessage("THE DRAGON WATCHES YOU.", "WHAT WILL YOU DO?");
            } else {
                screenState = SCREEN_GAME_OVER;
            }
        }
    } else if (screenState == SCREEN_GAME_OVER) {
        if (keys & (KEY_A | KEY_START)) screenState = SCREEN_TITLE;
    } else if (screenState == SCREEN_CREDITS) {
        if (keys & (KEY_A | KEY_START)) screenState = SCREEN_TITLE;
    }
}

static void renderTitle(void) {
    drawNightBackground();
    drawBoy(37, 82, 1);
    drawDragon(161, 65, 1);
    panel(12, 7, 216, 37);
    centeredText(13, "THE DRAGON CASTLE", C_GOLD, 2);
    centeredText(34, "A TINY TURN-BASED RPG", C_WHITE, 1);
    panel(20, 119, 200, 34);
    centeredText(126, "ENTER THE CASTLE.", C_WHITE, 1);
    centeredText(140, "PRESS START OR A", C_YELLOW, 1);
}

static void renderIntro(void) {
    drawNightBackground();
    panel(13, 12, 214, 137);
    centeredText(22, "THE OLD CASTLE", C_GOLD, 2);
    centeredText(49, "A YOUNG BOY ENTERS", C_WHITE, 1);
    centeredText(60, "AN ANCIENT CASTLE...", C_WHITE, 1);
    centeredText(77, "A DRAGON GUARDS THE HALL.", C_WHITE, 1);
    centeredText(91, "YOUR ADVENTURE STARTS NOW.", C_WHITE, 1);
    drawBoy(39, 100, 1);
    drawDragon(169, 101, 1);
    centeredText(136, "PRESS A TO BEGIN", C_YELLOW, 1);
}

static void renderBattle(void) {
    char hpLabel[12];
    char dragonLabel[12];
    drawDungeonBackground();
    drawBoy(31, 15, 2);
    drawDragon(157, 13, 2);

    numberMessage("", heroHP, "/34", hpLabel);
    numberMessage("", dragonHP, "/46", dragonLabel);

    panel(3, 65, 113, 36);
    drawText(8, 69, "BOY HP", C_WHITE, 1);
    drawText(54, 69, hpLabel, C_GOLD, 1);
    drawBar(8, 81, 101, 8, heroHP, 34, C_GREEN2);
    drawText(8, 91, "POTIONS", C_GOLD, 1);
    drawText(56, 91, (potions == 3) ? "3" : (potions == 2) ? "2" : (potions == 1) ? "1" : "0", C_WHITE, 1);

    panel(123, 65, 114, 36);
    drawText(128, 69, "DRAGON HP", C_WHITE, 1);
    drawText(187, 69, dragonLabel, C_GOLD, 1);
    drawBar(128, 81, 103, 8, dragonHP, 46, C_RED);
    drawText(128, 91, "FIRE BREATH", C_ORANGE, 1);

    panel(3, 104, 234, 25);
    drawText(8, 109, message1, C_WHITE, 1);
    drawText(8, 119, message2, C_YELLOW, 1);

    panel(3, 132, 234, 25);
    if (battlePhase == 0) {
        int px = (menuChoice == 0) ? 8 : (menuChoice == 1) ? 83 : 158;
        drawText(px, 142, ">", C_GOLD, 1);
        drawText(17, 142, "ATTACK", (menuChoice == 0) ? C_GOLD : C_WHITE, 1);
        drawText(92, 142, "POTION", (menuChoice == 1) ? C_GOLD : C_WHITE, 1);
        drawText(128, 142, (potions == 3) ? "3" : (potions == 2) ? "2" : (potions == 1) ? "1" : "0", C_WHITE, 1);
        drawText(167, 142, "DEFEND", (menuChoice == 2) ? C_GOLD : C_WHITE, 1);
    } else {
        centeredText(142, "PRESS A TO CONTINUE", C_GOLD, 1);
    }
}

static void renderGameOver(void) {
    drawDungeonBackground();
    panel(14, 22, 212, 115);
    centeredText(38, "GAME OVER", C_RED, 2);
    centeredText(70, "THE DRAGON WINS THIS TIME.", C_WHITE, 1);
    centeredText(87, "TRY AGAIN, BRAVE HERO.", C_WHITE, 1);
    centeredText(116, "PRESS A TO RETURN", C_YELLOW, 1);
}

static void renderCredits(void) {
    drawNightBackground();
    panel(8, 8, 224, 144);
    centeredText(17, "VICTORY!", C_GOLD, 2);
    centeredText(43, "THE DRAGON IS DEFEATED.", C_WHITE, 1);
    centeredText(64, "CREDITS", C_GOLD, 2);
    centeredText(91, "juego hecho con chatGPT", C_WHITE, 1);
    centeredText(103, "beta/test 0", C_YELLOW, 1);
    centeredText(124, "THANKS FOR PLAYING", C_WHITE, 1);
    centeredText(141, "PRESS A TO RETURN", C_GOLD, 1);
}

static void render(void) {
    switch (screenState) {
        case SCREEN_TITLE:    renderTitle(); break;
        case SCREEN_INTRO:    renderIntro(); break;
        case SCREEN_BATTLE:   renderBattle(); break;
        case SCREEN_GAME_OVER:renderGameOver(); break;
        case SCREEN_CREDITS:  renderCredits(); break;
        default:              renderTitle(); break;
    }
}

int main(void) {
    REG_DISPCNT = MODE3 | BG2_ENABLE;
    previousKeys = (u16)(~REG_KEYINPUT) & 0x03FF;
    render();

    for (;;) {
        u16 keys;
        waitVBlank();
        keys = keysDown();
        if (keys != 0) {
            updateGame(keys);
            render();
        }
    }
    return 0;
}
