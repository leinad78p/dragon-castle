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
enum { BLACK=0, WHITE=0x7fff, RED=0x001f, GREEN=0x03e0, BLUE=0x7c00, GOLD=0x03ff, DARK=0x2108, GREY=0x4210, SKY=0x1084, MOON=0x5294, HILL=0x18c3, GRASS=0x02a5, WOOD=0x0110, LEAF=0x0148, STONE=0x294a, PURPLE=0x4010, ORANGE=0x021f, PALE=0x6f7b, DEEPGREEN=0x0103, BARK=0x1508 };
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
static void small_text(int x,int y,const char*s,Color c){
 while(*s){const Glyph*g=glyph(*s++);int r,k;for(r=0;r<7;r++)for(k=0;k<5;k++)if(g->row[r]&(1u<<(4-k)))pixel(x+k,y+r,c);x+=6;}
}
static void line(int x0,int y0,int x1,int y1,Color c){int dx=x1>x0?x1-x0:x0-x1,sx=x0<x1?1:-1,dy=-(y1>y0?y1-y0:y0-y1),sy=y0<y1?1:-1,err=dx+dy,e;for(;;){pixel(x0,y0,c);if(x0==x1&&y0==y1)break;e=2*err;if(e>=dy){err+=dy;x0+=sx;}if(e<=dx){err+=dx;y0+=sy;}}}

/* Small software-raster primitives for detailed static pixel art. */
#define RGB15(r,g,b) ((Color)((r)|((g)<<5)|((b)<<10)))
static void hline(int x0,int x1,int y,Color c){
 int x,t;if(x0>x1){t=x0;x0=x1;x1=t;}for(x=x0;x<=x1;x++)pixel(x,y,c);
}
static int edge_intersection_x(int x0,int y0,int x1,int y1,int y){
 if(y0==y1)return x0;
 return x0+(y-y0)*(x1-x0)/(y1-y0);
}
static void fill_triangle(int x0,int y0,int x1,int y1,int x2,int y2,Color c){
 int y,miny=y0,maxy=y0,xs[3],n,t,i;
 if(y1<miny)miny=y1;if(y2<miny)miny=y2;
 if(y1>maxy)maxy=y1;if(y2>maxy)maxy=y2;
 for(y=miny;y<=maxy;y++){
  n=0;
  if(y>=((y0<y1)?y0:y1)&&y<=((y0>y1)?y0:y1))xs[n++]=edge_intersection_x(x0,y0,x1,y1,y);
  if(y>=((y1<y2)?y1:y2)&&y<=((y1>y2)?y1:y2))xs[n++]=edge_intersection_x(x1,y1,x2,y2,y);
  if(y>=((y2<y0)?y2:y0)&&y<=((y2>y0)?y2:y0))xs[n++]=edge_intersection_x(x2,y2,x0,y0,y);
  for(i=1;i<n;i++){t=xs[i];int j=i-1;while(j>=0&&xs[j]>t){xs[j+1]=xs[j];j--;}xs[j+1]=t;}
  if(n>=2)hline(xs[0],xs[n-1],y,c);
 }
}
static void fill_oval(int cx,int cy,int rx,int ry,Color c){
 int x,y,xx,yy,limit;
 if(rx<1||ry<1)return;
 limit=rx*rx*ry*ry;
 for(y=-ry;y<=ry;y++)for(x=-rx;x<=rx;x++){
  xx=x*x*ry*ry;yy=y*y*rx*rx;
  if(xx+yy<=limit)pixel(cx+x,cy+y,c);
 }
}
static void dragon_figure(void){
 Color ink=RGB15(3,2,7),wing=RGB15(15,3,9),winglight=RGB15(22,5,10);
 Color scale=RGB15(20,7,9),belly=RGB15(24,13,8),gold=RGB15(31,22,7);
 /* Wings are layered triangles, with visible ribs instead of one flat blob. */
 fill_triangle(124,96,82,54,91,91,ink);
 fill_triangle(121,95,85,59,92,87,wing);
 fill_triangle(121,95,92,87,104,80,wing);
 fill_triangle(121,95,104,80,112,91,winglight);
 fill_triangle(129,95,166,54,158,91,ink);
 fill_triangle(130,94,163,59,157,87,wing);
 fill_triangle(130,94,157,87,145,80,wing);
 fill_triangle(130,94,145,80,139,91,winglight);
 line(123,96,86,57,scale);line(122,96,92,88,scale);line(122,96,105,81,scale);
 line(130,96,164,57,scale);line(130,96,157,88,scale);line(130,96,145,81,scale);
 /* Long tail with a small arrow-shaped tip. */
 line(119,103,110,108,ink);line(110,108,100,108,ink);line(100,108,88,101,ink);
 line(119,102,110,107,scale);line(110,107,100,107,scale);line(100,107,89,101,scale);
 fill_triangle(89,101,78,96,86,105,wing);
 /* Back, belly and neck. */
 fill_oval(126,101,13,9,ink);
 fill_oval(126,100,11,7,scale);
 fill_triangle(128,103,139,87,148,91,ink);
 fill_triangle(130,103,139,90,143,94,belly);
 line(133,106,137,111,ink);line(137,111,145,112,ink);
 line(121,106,119,112,ink);line(119,112,112,114,ink);
 line(145,112,149,110,ink);line(145,112,150,113,ink);
 line(112,114,108,112,ink);line(112,114,109,115,ink);
 /* Head and long snout, facing right. */
 fill_oval(145,86,7,5,ink);
 fill_oval(145,85,5,3,scale);
 fill_triangle(148,84,163,88,149,91,ink);
 line(149,88,160,88,belly);
 pixel(148,84,gold);pixel(149,84,gold);
 /* Horns, dorsal spines and claws catch the moonlight. */
 line(141,82,137,76,gold);line(143,82,141,75,belly);
 line(147,82,151,76,gold);line(148,83,153,79,belly);
 fill_triangle(133,94,135,87,139,93,winglight);
 fill_triangle(138,91,142,84,144,90,winglight);
 fill_triangle(128,94,129,88,133,93,winglight);
 line(122,110,117,115,gold);line(117,115,113,115,gold);
 line(134,109,138,114,gold);line(138,114,143,114,gold);
 /* A few bright scales define the shoulder and cheek. */
 pixel(132,99,belly);pixel(135,97,belly);pixel(138,94,belly);
 pixel(153,86,gold);pixel(155,87,belly);
}

static void sound_init(void){REG_SOUNDCNT_X=0x0080;REG_SOUNDCNT_L=0x1177;REG_SND2CNT=0xF2C0;}
static const uint16_t song[]={1547,1462,1371,1216,1371,1462,1547,1784,1547,1462,1216,1371,1462,1547,1462,1784};
static unsigned song_frame=0,song_step=0;
static void music_tick(void){song_frame++;if(song_frame>=30){song_frame=0;REG_SND2FREQ=(uint16_t)(0x8000|song[song_step]);song_step=(song_step+1)%16;}}
static void logo(void){clear(BLACK);rect(114,49,5,5,GOLD);rect(119,44,5,5,GOLD);rect(124,49,5,5,GOLD);rect(119,54,5,5,GOLD);text(42,70,"MOONLIT PIXEL",MOON);text(72,88,"PRESENTS",GREY);}
static void landscape(void){
 Color night=RGB15(2,3,11),far=RGB15(4,6,15),far2=RGB15(6,8,19);
 Color mountain=RGB15(7,9,17),leftface=RGB15(8,11,21),rightface=RGB15(4,5,13);
 Color snow=RGB15(20,22,25),snowlight=RGB15(29,29,31),mist=RGB15(9,12,22);
 int i;
 clear(night);
 /* Moonlit sky: stars, a crescent moon, and thin bands of cloud. */
 fill_oval(195,43,12,12,RGB15(25,25,27));
 fill_oval(201,39,11,11,night);
 pixel(23,48,WHITE);pixel(44,65,RGB15(18,21,29));pixel(70,43,WHITE);
 pixel(153,44,RGB15(18,21,29));pixel(173,58,WHITE);pixel(221,48,WHITE);
 pixel(34,83,RGB15(15,18,27));pixel(211,78,RGB15(15,18,27));
 line(13,54,37,54,mist);line(18,58,31,58,mist);
 line(166,66,190,66,mist);line(178,70,198,70,mist);
 /* Distant mountain range. */
 fill_triangle(0,119,34,75,77,119,far);
 fill_triangle(34,120,84,81,132,120,far2);
 fill_triangle(104,121,169,64,239,121,far);
 fill_triangle(0,127,58,95,102,127,RGB15(5,7,16));
 fill_triangle(157,126,207,87,239,125,RGB15(5,7,16));
 fill_triangle(24,91,34,75,47,91,RGB15(16,18,25));
 fill_triangle(73,91,84,81,94,91,RGB15(16,18,25));
 fill_triangle(151,81,169,64,185,82,RGB15(17,19,26));
 /* The central mountain has two shaded faces and a snowy summit. */
 fill_triangle(10,141,116,53,222,141,mountain);
 fill_triangle(10,141,116,53,116,141,leftface);
 fill_triangle(116,141,116,53,222,141,rightface);
 fill_triangle(99,77,116,53,134,78,snow);
 fill_triangle(108,70,116,53,122,72,snowlight);
 fill_triangle(99,77,108,70,111,79,RGB15(15,18,25));
 fill_triangle(122,72,134,78,127,81,RGB15(13,16,23));
 line(12,140,116,54,RGB15(12,15,23));
 line(116,54,221,140,RGB15(10,12,19));
 /* Rock shelf: the dragon is perched above the valley. */
 fill_triangle(54,132,112,113,179,137,RGB15(3,4,10));
 fill_triangle(83,132,112,113,142,132,RGB15(8,10,17));
 fill_triangle(112,113,179,137,142,132,RGB15(5,7,13));
 line(65,132,112,116,RGB15(13,15,21));
 line(112,116,151,132,RGB15(10,12,18));
 /* A little drifting mist separates the mountain layers. */
 for(i=0;i<3;i++){
  line(8+i*9,119+i*3,49+i*9,119+i*3,RGB15(7,9,17));
  line(178+i*8,112+i*3,226+i*4,112+i*3,RGB15(7,9,17));
 }
 /* The dragon itself is drawn from C primitives directly into GBA VRAM. */
 dragon_figure();
 /* Keep title text readable over the illustration. */
 rect(6,6,169,29,BLACK);
 text(12,12,"DRAGON CASTLE",GOLD);
 rect(7,141,226,15,BLACK);
 text(16,145,"A START",WHITE);text(108,145,"B ADVENTURE",MOON);
}
static void title(void){landscape();}
static void wait_vblank(void){while(REG_DISPSTAT&1){}while(!(REG_DISPSTAT&1)){}}
static uint16_t keys(void){return (uint16_t)(~REG_KEYINPUT)&0x03ff;}


static void forest_tree(int x,int y,int size){
 int w=size,h=size+13;
 rect(x+size/3,y+size/2,w/3,h,BARK);
 rect(x,y+size/3,size,size/2,DEEPGREEN);
 rect(x+size/5,y+size/8,size*3/5,size/2,LEAF);
 rect(x+size/3,y,size/3,size/2,DEEPGREEN);
 rect(x+size/4,y+size/4,3,3,MOON);
}
static void forest_background(void){
 int x,y;
 clear(0x08a2);
 /* Misty canopy and layered forest floor */
 rect(0,0,W,54,0x10c4);
 rect(0,48,W,44,0x0842);
 rect(0,90,W,70,0x0144);
 for(x=0;x<W;x+=24){
  int top=56+(x%5)*3;
  for(y=top;y<112;y++)if(((x+y)%4)!=0)pixel(x,y,0x0123);
 }
 /* A narrow trail disappears into the woods */
 for(y=48;y<H;y++){
  int half=2+(y-48)/5;
  int cx=124+(y/13%3-1)*3;
  for(x=cx-half;x<=cx+half;x++)pixel(x,y,(y<90?0x18a5:0x20c6));
 }
 forest_tree(4,14,34);forest_tree(47,31,25);forest_tree(84,7,33);
 forest_tree(157,24,29);forest_tree(198,5,39);forest_tree(215,53,22);
 forest_tree(8,90,28);forest_tree(54,79,20);forest_tree(174,86,25);
 forest_tree(207,101,30);
 /* Pale motes in the fog */
 rect(72,54,2,2,MOON);rect(143,69,2,2,WHITE);rect(111,34,2,2,MOON);
 rect(186,57,2,2,MOON);rect(35,66,2,2,WHITE);
}
static void hero_sprite(int x,int y,int who,int look){
 Color tunic=GREY,skin=PALE;
 if(who==0)tunic=BLUE;       /* Edric */
 if(who==1)tunic=RED;        /* Zaryndra */
 if(who==2)tunic=PURPLE;     /* Vaelis */
 if(who==3)tunic=ORANGE;     /* Kendra */
 /* Feet, body and head: intentionally tiny placeholder sprites */
 rect(x+3,y+14,4,4,WOOD);rect(x+10,y+14,4,4,WOOD);
 rect(x+2,y+7,14,9,tunic);
 rect(x+5,y+1,9,8,skin);
 rect(x+4+(look<0?-2:look>0?2:0),y+4,2,2,BLACK);
 rect(x+11+(look<0?-2:look>0?2:0),y+4,2,2,BLACK);
 if(who==0){rect(x+4,y,10,3,WOOD);rect(x+7,y-2,4,3,MOON);} /* Edric hair */
 if(who==1){rect(x+4,y-3,3,5,RED);rect(x+12,y-3,3,5,RED);rect(x+1,y+8,3,5,RED);rect(x+14,y+8,3,5,RED);} /* dragon horns */
 if(who==2){rect(x+2,y+3,4,3,WHITE);rect(x+13,y+3,4,3,WHITE);rect(x+4,y-2,10,3,PURPLE);} /* elf ears */
 if(who==3){rect(x+2,y,13,4,WOOD);rect(x+1,y+2,3,3,WOOD);rect(x+13,y+2,3,3,WOOD);} /* dwarf helmet */
}
static void dialogue_box(int page){
 const char*speaker="";
 const char*line1="";
 const char*line2="";
 if(page==0){speaker="ZARYNDRA";line1="TE DIJE QUE NOS PERDIMOS";}
 else if(page==1){speaker="EDRIC";line1="NO ESTAMOS PERDIDOS";}
 else if(page==2){speaker="ZARYNDRA";line1="SI QUE ESTAMOS PERDIDOS";}
 else if(page==3){speaker="VAELIS";line1="NO ESTAMOS PERDIDOS CUANDO DEJAMOS";line2="QUE LA VIDA GUIE NUESTRO CAMINO";}
 else if(page==4){speaker="ZARYNDRA / EDRIC";line1="TU CALLATE!";}
 else {speaker="KENDRA";line1="NOS COMERAN LAS BESTIAS";line2="SALVAJES!";}
 rect(4,99,232,57,BLACK);rect(6,101,228,53,0x2108);rect(6,101,228,1,GOLD);
 text(12,105,speaker,WHITE);
 small_text(12,123,line1,WHITE);
 if(line2[0])small_text(12,136,line2,WHITE);
 small_text(174,146,"A: SIGUIENTE",MOON);
}
static Color scene_backup[90*34];
static void forest_scene_init(void){
 int x,y;
 forest_background();
 /* Keep a clean copy of the small area where the party walks. */
 for(y=68;y<102;y++)for(x=0;x<90;x++)scene_backup[(y-68)*90+x]=VRAM[y*W+x];
}
static void forest_scene(int frame,int page){
 int base,look=0,x,y;
 /* Restore only the characters' area; the forest itself is static. */
 for(y=68;y<102;y++)for(x=0;x<90;x++)pixel(x,y,scene_backup[(y-68)*90+x]);
 /* The party enters from the left, then stops to look around. */
 base=frame<105 ? -25+frame/3 : 10;
 if(frame>=105)look=((frame/24)%2==0)?-1:1;
 hero_sprite(base,75,0,look);
 hero_sprite(base+19,77,1,-look);
 hero_sprite(base+38,74,2,look);
 hero_sprite(base+57,78,3,-look);
 if(page>=0)dialogue_box(page);
 else if(frame==0){
  rect(4,139,232,17,BLACK);
  small_text(13,145,"THE FOREST OF VALDORIA",MOON);
 }
}

static void battle(int hp,int dr,int pot,int choice){clear(BLACK);text(24,8,"DRAGON BATTLE",GOLD);text(12,28,"HERO HP",WHITE);rect(108,28,100,10,GREY);rect(108,28,hp*100/34,10,GREEN);text(12,46,"DRAGON HP",WHITE);rect(108,46,100,10,GREY);rect(108,46,dr*100/46,10,RED);text(12,64,"POTIONS",WHITE);if(pot>0)rect(108,64,pot*20,8,BLUE);rect(12,82,216,64,DARK);text(24,88,choice==0?"> ATTACK":"  ATTACK",choice==0?GOLD:WHITE);text(24,108,choice==1?"> POTION":"  POTION",choice==1?GOLD:WHITE);text(24,128,choice==2?"> DEFEND":"  DEFEND",choice==2?GOLD:WHITE);}
static void message(const char*t,const char*a,const char*b){clear(BLACK);text(24,28,t,GOLD);text(12,64,a,WHITE);text(12,84,b,WHITE);text(12,124,"A TO CONTINUE",GREEN);}
int main(void){
 int state=0,hp=34,dr=46,pot=3,choice=0,guard=0,boot,scene_frame=0,dialogue_page=-1;uint16_t old=0;
 REG_DISPCNT=0x0403;sound_init();clear(BLACK);
 for(boot=0;boot<35;boot++)wait_vblank();
 logo();for(boot=0;boot<180;boot++){wait_vblank();if(keys()!=0)break;}
 title();old=keys();
 for(;;){uint16_t now,pressed;wait_vblank();now=keys();pressed=(uint16_t)(now&(uint16_t)~old);old=now;
 if(state==0){music_tick();if(pressed&(KEY_A|KEY_START)){state=4;scene_frame=0;dialogue_page=-1;forest_scene_init();forest_scene(scene_frame,dialogue_page);}}
 else if(state==4){
  if(scene_frame<150){scene_frame++;forest_scene(scene_frame,-1);}
  else if(dialogue_page<0){dialogue_page=0;forest_scene(scene_frame,dialogue_page);}
  else if(pressed&KEY_A){
   if(dialogue_page<5){dialogue_page++;forest_scene(scene_frame,dialogue_page);}
   else{state=5;hp=34;dr=46;pot=3;choice=0;guard=0;message("DANGER!","SOMETHING MOVES","IN THE FOREST");}
  }
 }
 else if(state==5){if(pressed&KEY_A){state=1;battle(hp,dr,pot,choice);}}
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