#ifndef __GRAPHICS__
#define __GRAPHICS__

#include <SDL.h>
#include <SDL_image.h>
#include "ltypes.h"


typedef struct
{
    int xres,yres,depth;
}ScreenData;


/*the basics*/
void Init_Graphics(int windowed);
void DrawPixel(arSurface *screen, arUI8 R, arUI8 G, arUI8 B, int x, int y);
void DrawSquareLine(arSurface *screen,arUI32 color,int sx,int sy,int gx,int gy);
void ShowImage(arSurface *image, arSurface *screen, int x, int y);
arUI32 getpixel(arSurface *surface, int x, int y);
void putpixel(arSurface *surface, int x, int y, arUI32 pixel);
void DrawFilledRect(int sx,int sy, int sw, int sh, arUI32 Color, arSurface *surface);
void DrawRect(int sx,int sy, int sw, int sh, arUI32 Color, arSurface *surface);
void DrawElipse(int ox,int oy, int radius, arUI32 Color, arSurface *surface);
void DrawAnyLine(int sx,int sy,int dx, int dy,arUI32 Color,arSurface *surface);
void DrawThickLine(int sx,int sy,int dx, int dy,int width,arUI32 Color,arSurface *surface);

/*frame handling functions*/
void BlankScreen(arSurface *buf,arUI32 color);
void FrameDelay(arUI32 delay);
void ResetBuffer();
void NextFrame();


#endif