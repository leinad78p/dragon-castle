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
enum { BLACK=0, WHITE=0x7fff, RED=0x001f, GREEN=0x03e0, BLUE=0x7c00, GOLD=0x03ff, DARK=0x2108, GREY=0x4210 };
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
static void wait_vblank(void){while(REG_DISPSTAT&1){}while(!(REG_DISPSTAT&1)){}}
static uint16_t keys(void){return (uint16_t)(~REG_KEYINPUT)&0x03ff;}
static void title(void){clear(DARK);rect(12,12,216,136,BLUE);rect(16,16,208,128,BLACK);text(36,28,"DRAGON CASTLE",GOLD);text(36,56,"A TINY RPG",WHITE);rect(30,82,180,2,GOLD);text(24,98,"A START BEGIN",WHITE);text(24,122,"BATTLE A DRAGON",GREEN);}
static void battle(int hp,int dr,int pot,int choice){clear(BLACK);text(24,8,"DRAGON BATTLE",GOLD);text(12,28,"HERO HP",WHITE);rect(108,28,100,10,GREY);rect(108,28,hp*100/34,10,GREEN);text(12,46,"DRAGON HP",WHITE);rect(108,46,100,10,GREY);rect(108,46,dr*100/46,10,RED);text(12,64,"POTIONS",WHITE);if(pot>0)rect(108,64,pot*20,8,BLUE);rect(12,82,216,64,DARK);text(24,88,choice==0?"> ATTACK":"  ATTACK",choice==0?GOLD:WHITE);text(24,108,choice==1?"> POTION":"  POTION",choice==1?GOLD:WHITE);text(24,128,choice==2?"> DEFEND":"  DEFEND",choice==2?GOLD:WHITE);}
static void message(const char*t,const char*a,const char*b){clear(BLACK);text(24,28,t,GOLD);text(12,64,a,WHITE);text(12,84,b,WHITE);text(12,124,"A TO CONTINUE",GREEN);}
int main(void){
 int state=0,hp=34,dr=46,pot=3,choice=0,guard=0;uint16_t old=0;
 REG_DISPCNT=0x0403;title();
 for(;;){uint16_t now,pressed;wait_vblank();now=keys();pressed=(uint16_t)(now&(uint16_t)~old);old=now;
 if(state==0){if(pressed&(KEY_A|KEY_START)){state=1;hp=34;dr=46;pot=3;choice=0;guard=0;battle(hp,dr,pot,choice);}}
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
