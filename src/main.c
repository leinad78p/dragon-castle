/*
 * Dragon Castle - beta/test 0
 * Self-contained GBA version: no libgba headers or libraries required.
 * Copyright (c) 2026 leinad78p
 */
#include <stdint.h>
#define REG_DISPCNT (*(volatile uint16_t *)0x04000000)
#define REG_DISPSTAT (*(volatile uint16_t *)0x04000004)
#define REG_KEYINPUT (*(volatile uint16_t *)0x04000130)
#define VRAM ((volatile uint16_t *)0x06000000)
#define W 240
#define H 160
#define KEY_A 1
#define KEY_B 2
#define KEY_START 8
#define KEY_UP 64
#define KEY_DOWN 128
typedef uint16_t Color;
enum { BLACK=0, WHITE=0x7fff, RED=0x001f, GREEN=0x03e0, BLUE=0x7c00, GOLD=0x03ff, DARK=0x2108, GREY=0x4210, SKY=0x1084, MOON=0x5294, HILL=0x18c3, GRASS=0x02a5, WOOD=0x0110, LEAF=0x0148, STONE=0x294a };
#define REG_SOUNDCNT_L (*(volatile uint16_t *)0x04000080)
#define REG_SOUNDCNT_X (*(volatile uint16_t *)0x04000084)
#define REG_SND2CNT (*(volatile uint16_t *)0x04000068)
#define REG_SND2FREQ (*(volatile uint16_t *)0x0400006C)
typedef struct { char c; uint8_t row[7]; } Glyph;
static const Glyph font[] = {
 {' ',{0,0,0,0,0,0,0}}, {'A',{14,17,17,31,17,17,17}}, {'B',{30,17,17,30,17,17,30}}, {'C',{14,17,16,16,16,17,14}},
 {'D',{30,17,17,17,17,17,30}}, {'E',{31,16,16,30,16,16,31}}, {'F',{31,16,16,30,16,16,16}}, {'G',{14,17,16,23,17,17,15}},
 {'H',{17,17,17,31,17,17,17}}, {'I',{14,4,4,4,4,4,14}}, {'J',{7,2,2,2,18,18,12}}, {'K',{17,18,20,24,20,18,17}},
 {'L',{16,16,16,16,16,16,31}}, {'M',{17,27,21,21,17,17,17}}, {'N',{17,25,21,19,17,17,17}}, {'O',{14,17,17,17,17,17,14}},
 {'P',{30,17,17,30,16,16,16}}, {'Q',{14,17,17,17,21,18,13}}, {'R',{30,17,17,30,20,18,17}}, {'S',{15,16,16,14,1,1,30}},
 {'T',{31,4,4,4,4,4,4}}, {'U',{17,17,17,17,17,17,14}}, {'V',{17,17,17,17,17,10,4}}, {'W',{17,17,17,21,21,21,10}},
 {'X',{17,17,10,4,10,17,17}}, {'Y',{17,17,10,4,4,4,4}}, {'Z',{31,1,2,4,8,16,31}},
 {'0',{14,17,19,21,25,17,14}}, {'1',{4,12,4,4,4,4,14}}, {'2',{14,17,1,2,4,8,31}}, {'3',{30,1,1,14,1,1,30}},
 {'4',{2,6,10,18,31,2,2}}, {'5',{31,16,16,30,1,1,30}}, {'6',{14,16,16,30,17,17,14}}, {'7',{31,1,2,4,8,8,8}},
 {'8',{14,17,17,14,17,17,14}}, {'9',{14,17,17,15,1,1,14}}, {'!',{4,4,4,4,4,0,4}}, {':',{0,4,4,0,4,4,0}},
 {'/',{1,2,2,4,8,8,16}}, {'-',{0,0,0,31,0,0,0}}, {'>',{16,8,4,2,4,8,16}}, {'.',{0,0,0,0,0,12,12}}
};
static void pixel(int x,int y,Color c){if(x>=0&&x<W&&y>=0&&y<H)VRAM[y*W+x]=c;}
static void rect(int x,int y,int w,int h,Color c){int a,b;for(b=y;b<y+h;b++)for(a=x;a<x+w;a++)pixel(a,b,c);}
static const Glyph *glyph(char c){unsigned i;if(c>='a'&&c<='z')c=(char)(c-'a'+'A');for(i=0;i<sizeof(font)/sizeof(font[0]);i++)if(font[i].c==c)return &font[i];return &font[0];}
static void text(int x,int y,const char*s,Color c){while(*s){const Glyph*g=glyph(*s++);int r,k;for(r=0;r<7;r++)for(k=0;k<5;k++)if(g->row[r]&(1u<<(4-k))){pixel(x+k*2,y+r*2,c);pixel(x+k*2+1,y+r*2,c);pixel(x+k*2,y+r*2+1,c);pixel(x+k*2+1,y+r*2+1,c);}x+=12;}}
static void clear(Color c){rect(0,0,W,H,c);}
static void line(int x0,int y0,int x1,int y1,Color c){int dx=x1>x0?x1-x0:x0-x1,sx=x0<x1?1:-1,dy=-(y1>y0?y1-y0:y0-y1),sy=y0<y1?1:-1,err=dx+dy,e;for(;;){pixel(x0,y0,c);if(x0==x1&&y0==y1)break;e=2*err;if(e>=dy){err+=dy;x0+=sx;}if(e<=dx){err+=dx;y0+=sy;}}}
static void sound_init(void){REG_SOUNDCNT_X=0x0080;REG_SOUNDCNT_L=0x1177;REG_SND2CNT=0x00F2;}
static const uint16_t song[]={1547,1462,1371,1216,1371,1462,1547,1784,1547,1462,1216,1371,1462,1547,1462,1784};
static unsigned song_frame=0,song_step=0;
static void music_tick(void){song_frame++;if(song_frame>=30){song_frame=0;REG_SND2FREQ=(uint16_t)(0x8000|song[song_step]);song_step=(song_step+1)%16;}}
static void logo(void){clear(BLACK);rect(114,49,5,5,GOLD);rect(119,44,5,5,GOLD);rect(124,49,5,5,GOLD);rect(119,54,5,5,GOLD);text(60,70,"MOONLIT PIXEL",MOON);text(84,88,"PRESENTS",GREY);}
static void landscape(void){
 int x,y;
 clear(SKY);
 rect(24,20,2,2,WHITE);rect(53,39,2,2,WHITE);rect(107,16,2,2,WHITE);rect(207,32,2,2,WHITE);rect(179,16,15,15,MOON);rect(185,12,13,13,SKY);
 for(x=0;x<W;x++){int ridge=111-(x<95?(95-x)/8:(x-95)/13);for(y=ridge;y<H;y++)pixel(x,y,HILL);}
 for(x=0;x<W;x++){int ridge=132-(x<140?(140-x)/12:(x-140)/7);for(y=ridge;y<H;y++)pixel(x,y,GRASS);}
 /* Castle silhouette */
 rect(137,73,53,42,BLACK);rect(131,64,14,51,BLACK);rect(149,56,13,59,BLACK);rect(176,62,18,53,BLACK);rect(193,75,8,40,BLACK);
 rect(133,59,3,5,BLACK);rect(139,59,3,5,BLACK);rect(151,50,3,6,BLACK);rect(157,50,3,6,BLACK);rect(179,56,4,6,BLACK);rect(187,56,4,6,BLACK);
 rect(145,83,5,8,MOON);rect(160,83,5,8,MOON);rect(179,82,5,8,MOON);rect(158,99,12,16,WOOD);
 /* Tree */
 line(40,91,40,137,WOOD);rect(36,104,5,26,WOOD);rect(41,113,5,19,WOOD);
 line(40,49,18,94,BLACK);line(40,49,62,94,BLACK);line(18,94,62,94,BLACK);line(40,62,21,105,BLACK);line(40,62,59,105,BLACK);line(21,105,59,105,BLACK);line(40,74,25,115,BLACK);line(40,74,55,115,BLACK);line(25,115,55,115,BLACK);
 rect(31,81,5,5,LEAF);rect(44,73,5,5,LEAF);rect(25,94,5,5,LEAF);rect(49,96,5,5,LEAF);
 /* Sword resting against the trunk */
 line(69,132,91,105,WHITE);line(70,133,92,106,MOON);line(66,128,75,136,GOLD);line(85,100,98,113,GOLD);line(88,97,101,110,WOOD);
 rect(8,7,112,29,BLACK);text(14,12,"DRAGON CASTLE",GOLD);
 rect(7,141,226,15,BLACK);text(16,145,"A START",WHITE);text(108,145,"B ADVENTURE",MOON);
}
static void title(void){landscape();}
static void wait_vblank(void){while(REG_DISPSTAT&1){}while(!(REG_DISPSTAT&1)){}}
static uint16_t keys(void){return (uint16_t)(~REG_KEYINPUT)&0x03ff;}
static void title(void){clear(DARK);rect(12,12,216,136,BLUE);rect(16,16,208,128,BLACK);text(36,28,"DRAGON CASTLE",GOLD);text(36,56,"A TINY RPG",WHITE);rect(30,82,180,2,GOLD);text(24,98,"A START BEGIN",WHITE);text(24,122,"BATTLE A DRAGON",GREEN);}
static void battle(int hp,int dr,int pot,int choice){clear(BLACK);text(24,8,"DRAGON BATTLE",GOLD);text(12,28,"HERO HP",WHITE);rect(108,28,100,10,GREY);rect(108,28,hp*100/34,10,GREEN);text(12,46,"DRAGON HP",WHITE);rect(108,46,100,10,GREY);rect(108,46,dr*100/46,10,RED);text(12,64,"POTIONS",WHITE);if(pot>0)rect(108,64,pot*20,8,BLUE);rect(12,82,216,64,DARK);text(24,88,choice==0?"> ATTACK":"  ATTACK",choice==0?GOLD:WHITE);text(24,108,choice==1?"> POTION":"  POTION",choice==1?GOLD:WHITE);text(24,128,choice==2?"> DEFEND":"  DEFEND",choice==2?GOLD:WHITE);}
static void message(const char*t,const char*a,const char*b){clear(BLACK);text(24,28,t,GOLD);text(12,64,a,WHITE);text(12,84,b,WHITE);text(12,124,"A TO CONTINUE",GREEN);}
int main(void){
 int state=0,hp=34,dr=46,pot=3,choice=0,guard=0,boot;uint16_t old=0;
 REG_DISPCNT=0x0403;sound_init();clear(BLACK);
 for(boot=0;boot<35;boot++)wait_vblank();
 logo();for(boot=0;boot<105;boot++)wait_vblank();
 title();
 for(;;){uint16_t now,pressed;wait_vblank();now=keys();pressed=(uint16_t)(now&(uint16_t)~old);old=now;
 if(state==0){music_tick();if(pressed&(KEY_A|KEY_START)){state=1;hp=34;dr=46;pot=3;choice=0;guard=0;battle(hp,dr,pot,choice);}}
 else if(state==1){if(pressed&KEY_B){state=0;title();}
 else if(pressed&KEY_UP){choice=(choice+2)%3;battle(hp,dr,pot,choice);}
 else if(pressed&KEY_DOWN){choice=(choice+1)%3;battle(hp,dr,pot,choice);}
 else if(pressed&KEY_A){if(choice==0){int d=7+(hp+dr+pot)%6;dr-=d;if(dr<0)dr=0;}
 else if(choice==1){if(pot>0&&hp<34){hp+=12;if(hp>34)hp=34;pot--;}}
 else guard=1;
 if(dr<=0){state=2;message("VICTORY!","DRAGON DEFEATED","CASTLE IS SAFE");}
 else{int d=5+(dr+hp)%6;if(guard){d=(d+1)/2;guard=0;}hp-=d;if(hp<=0){state=3;message("GAME OVER","DRAGON WINS","TRY AGAIN");}else battle(hp,dr,pot,choice);}}}
 else if(pressed&KEY_A){state=0;title();}
 }
 return 0;
}