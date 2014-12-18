#ifndef __FONT__
#define __FONT__

#include <SDL.h>
#include "SDL_ttf.h"
#include "ltypes.h"


enum FONTS {F_Small, F_Medium, F_Large};

/*font stuff*/
void LoadFonts();
void LoadFont(char filename[40],int ptsize,int type);
void DrawText(char *text,arSurface *surface,int sx,int sy,arUI32 color,int size);
void DrawTextCentered(char *text,arSurface *surface,int sx,int sy,arUI32 color,int size);
void DrawMessages();
void InitMessages();
void NewMessage(char *text,arUI32 color);


#endif