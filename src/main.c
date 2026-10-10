/*
 * Dragon Castle - beta/test 0
 * Turn-based Game Boy Advance homebrew.
 * Generated with ChatGPT; published by leinad78p.
 */
#include <gba.h>
#include <stdio.h>

typedef enum { TITLE, INTRO, BATTLE, OVER, CREDITS } State;

int main(void) {
    State state = TITLE;
    int hero = 34, dragon = 46, potions = 3, choice = 0;
    int guarding = 0, turnMessage = 0;
    irqInit();
    irqEnable(IRQ_VBLANK);
    consoleDemoInit();

    while (1) {
        scanKeys();
        u16 keys = keysDown();

        if (state == TITLE) {
            consoleClear();
            iprintf("   DRAGON CASTLE\n\n");
            iprintf(" A tiny turn-based RPG\n\n");
            iprintf(" A / START: Begin\n");
            while (1) {
                scanKeys();
                if (keysDown() & (KEY_A | KEY_START)) break;
                VBlankIntrWait();
            }
            state = INTRO;
        } else if (state == INTRO) {
            consoleClear();
            iprintf(" THE OLD CASTLE\n\n");
            iprintf("A young boy enters an\n");
            iprintf("ancient castle. A dragon\n");
            iprintf("guards the hall!\n\n");
            iprintf("A: Begin the battle\n");
            while (1) {
                scanKeys();
                if (keysDown() & KEY_A) break;
                VBlankIntrWait();
            }
            hero = 34; dragon = 46; potions = 3; choice = 0;
            state = BATTLE;
        } else if (state == BATTLE) {
            consoleClear();
            iprintf("   DRAGON BATTLE\n\n");
            iprintf("BOY HP: %d/34\n", hero);
            iprintf("DRAGON HP: %d/46\n", dragon);
            iprintf("POTIONS: %d\n\n", potions);
            iprintf("%s Attack\n", choice == 0 ? ">" : " ");
            iprintf("%s Potion\n", choice == 1 ? ">" : " ");
            iprintf("%s Defend\n\n", choice == 2 ? ">" : " ");
            iprintf("UP/DOWN select, A choose\n");
            iprintf("B: return to title\n");

            while (1) {
                scanKeys();
                keys = keysDown();
                if (keys & KEY_B) { state = TITLE; break; }
                if (keys & KEY_UP) { choice = (choice + 2) % 3; break; }
                if (keys & KEY_DOWN) { choice = (choice + 1) % 3; break; }
                if (keys & KEY_A) {
                    if (choice == 0) {
                        int damage = 7 + (hero + dragon + potions) % 6;
                        dragon -= damage;
                        if (dragon < 0) dragon = 0;
                        turnMessage = 1;
                    } else if (choice == 1) {
                        if (potions > 0 && hero < 34) {
                            hero += 12;
                            if (hero > 34) hero = 34;
                            --potions;
                        }
                        turnMessage = 2;
                    } else {
                        guarding = 1;
                        turnMessage = 3;
                    }

                    if (dragon <= 0) { state = CREDITS; break; }
                    {
                        int damage = 5 + (dragon + hero) % 6;
                        if (guarding) { damage = (damage + 1) / 2; guarding = 0; }
                        hero -= damage;
                        if (hero <= 0) { hero = 0; state = OVER; break; }
                    }
                    break;
                }
                VBlankIntrWait();
            }
        } else if (state == OVER) {
            consoleClear();
            iprintf("     GAME OVER\n\n");
            iprintf("The dragon wins this time.\n");
            iprintf("Press A to try again.\n");
            while (1) {
                scanKeys();
                if (keysDown() & KEY_A) break;
                VBlankIntrWait();
            }
            state = TITLE;
        } else if (state == CREDITS) {
            consoleClear();
            iprintf("      VICTORY!\n\n");
            iprintf("The dragon is defeated.\n\n");
            iprintf("       CREDITS\n\n");
            iprintf("juego hecho con chatGPT\n");
            iprintf("beta/test 0\n\n");
            iprintf("Thanks for playing!\n");
            iprintf("Press A to return.\n");
            while (1) {
                scanKeys();
                if (keysDown() & KEY_A) break;
                VBlankIntrWait();
            }
            state = TITLE;
        }
        VBlankIntrWait();
    }
    return 0;
}
