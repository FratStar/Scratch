#ifndef __SPRITE__
#define __SPRITE__

#include "graphics.h"
#include "ltypes.h"


enum colors {Red = 1,Green = 2,Blue = 3,Yellow = 4,Orange = 5,Violet = 6,Brown = 7,Grey = 8,DarkRed = 9,DarkGreen = 10,
               DarkBlue = 11,DarkYellow = 12,DarkOrange = 13,DarkViolet = 14,DarkBrown = 15,DarkGrey = 16,LightRed =17,
               LightGreen = 18,LightBlue = 19,LightYellow = 20,LightOrange = 21,LightViolet = 22,LightBrown = 23,LightGrey = 24,
               Black = 25,White = 26,Tan = 27,Gold = 28,Silver = 29,YellowGreen = 30,Cyan = 31,Magenta = 32};

typedef struct Sprite_T
{
	arSurface *image;
	char *filename;
	int w, h;
	int framesperline;
	int color1,color2,color3;
	int used;
	int loaded;

}Sprite;

/*Sprite functions */
arSurface *LoadImage(char *filename);
void InitSpriteList();
void FreeSprite(Sprite *img);
Sprite *LoadSprite(char *filename, int sizex, int sizey);
void DrawSprite(Sprite *sprite,arSurface *surface,int sx,int sy, int frame);
void CloseSprites();

/*'palette swapping functions*/
Sprite *LoadSwappedSprite(char *filename,int sizex, int sizey, int c1, int c2, int c3);
arUI32 SetColor(arUI32 color, int newcolor1,int newcolor2, int newcolor3);
void SwapSprite(arSurface *sprite,int color1,int color2,int color3);
arUI32 IndexColor(int color);
#endif