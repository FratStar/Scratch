#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "graphics.h"
#include "window.h"

#undef NO_STDIO_REDIRECT

arSurface *screen;
arSurface *videobuffer;
arSurface *background;
arSurface *bgimage;
arRect Camera;
arRect Abs_Camera;
arUI32 NOW, DELAY;


arUI32 rmask,gmask,bmask,amask;
ScreenData  S_Data;


void Init_Graphics(int windowed)
{
  Uint32 Vflags = SDL_ANYFORMAT | SDL_SRCALPHA;
    Uint32 HWflag = 0;
    SDL_Surface *temp;
    S_Data.xres = 1024;
    S_Data.yres = 600;
    if(!windowed)Vflags |= SDL_FULLSCREEN;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
    #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
    #endif
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_DOUBLEBUF) < 0 )
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
    if(SDL_VideoModeOK(1024, 600, 32, Vflags | SDL_HWSURFACE))
    {
        S_Data.xres = 1024;
        S_Data.yres = 600;
        S_Data.depth = 32;
		    fprintf(stderr,"32 bits of depth\n");
        HWflag = SDL_HWSURFACE;
    }
    else if(SDL_VideoModeOK(1024, 600, 16, Vflags | SDL_HWSURFACE))
    {
        S_Data.xres = 1024;
        S_Data.yres = 600;
        S_Data.depth = 16;
     		fprintf(stderr,"16 bits of depth\n");
        HWflag = SDL_HWSURFACE;
    }
    else if(SDL_VideoModeOK(1024, 600, 16, Vflags))
    {
        S_Data.xres = 1024;
        S_Data.yres = 600;
        S_Data.depth = 16;
    		fprintf(stderr,"16 bits of depth\n");
        HWflag = SDL_SWSURFACE;
    }
    else                                                         
    {
        fprintf(stderr, "Unable to Use your crap: %s\n Upgrade \n", SDL_GetError());
        exit(1);
    }
    videobuffer = SDL_SetVideoMode(S_Data.xres, S_Data.yres,S_Data.depth, Vflags);
    if ( videobuffer == NULL )
    {
        fprintf(stderr, "Unable to set 1024x600 video: %s\n", SDL_GetError());
        exit(1);
    }
    temp = SDL_CreateRGBSurface(HWflag, S_Data.xres, S_Data.yres, S_Data.depth,rmask, gmask,bmask,amask);
    if(temp == NULL)
	  {
        fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
        exit(1);
	  }
    /* Just to make sure that the surface we create is compatible with the screen*/
    screen = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    temp = SDL_CreateRGBSurface(HWflag, 2048, 2048, S_Data.depth,rmask, gmask,bmask,amask);
    if(temp == NULL)
	  {
        fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
        exit(1);
	  }
    /* Just to make sure that the surface we create is compatible with the screen*/
    background = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
    SDL_ShowCursor(SDL_DISABLE);
    Camera.x = 0;
    Camera.y = 0;
    Camera.w = screen->w;
    Camera.h = screen->h;
	Abs_Camera.x = 0;
    Abs_Camera.y = 0;
    Abs_Camera.w = Camera.w;
    Abs_Camera.h = Camera.h;
	SDL_ShowCursor(SDL_DISABLE);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
    srand(SDL_GetTicks());
}

void NextFrame()
{
	char *text;
	arUI32 Then;
	FrameDelay(60);
	Then = NOW;
	NOW = SDL_GetTicks();
	DELAY = NOW - Then;
	sprintf(text, "Ticks passeed this frame: %i", NOW - Then);
	#if 0
	NewMessage(text,IndexColor(White));
	fprintf(stdout,"Ticks passed this frame: %i\n", NOW - Then);
	fprintf(stdout,"FPS: %f\n", 1000.0 /(NOW - Then));
	#endif
}

/*Calulates the delay for the framerate that you want*/

void FrameDelay(arUI32 frate) 
{

	static arUI32 pass = 1000;
	arUI32 dif;
	dif = SDL_GetTicks() - pass;
	if(dif < 1000/frate)
	{
		SDL_Delay((1000/frate) - dif);
	}
	pass = SDL_GetTicks();
}



void putpixel(arSurface *surface, int x, int y, arUI32 pixel)
{
  arRect point = {0,0,1,1};
  point.x = x;
  point.y = y;
  SDL_FillRect(surface,&point,pixel);
}

arUI32 getpixel(arSurface *surface, int x, int y)
{
    /* Here p is the address to the pixel we want to retrieve*/
    arUI8 *p = (arUI8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
    if((x < 0)||(x >= surface->w)||(y < 0)||(y >= surface->h))return -1;
    switch(surface->format->BytesPerPixel)
    {
    case 1:
        return *p;

    case 2:
        return *(arUI16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(arUI32 *)p;

    default:
        return 0;       /*shouldn't happen, but avoids warnings*/
    }
}

/*draws an elipse at the location specified*/
void DrawElipse(int ox,int oy, int radius, Uint32 Color, arSurface *surface)
{
  int r2 = radius * radius;
  int x,y;
  for(x = radius * -1;x <= radius;x++)
  {
    y = (int) (sqrt(r2 - x*x) * 0.6);
    putpixel(surface, x + ox, oy + y, Color);
    putpixel(surface, x + ox, oy - y, Color);
  }
}

/*draws an rectangle outline at the coordinates of the width and height*/
void DrawRect(int sx,int sy, int sw, int sh, Uint32 Color, arSurface *surface)
{
  arRect box;
    box.x = sx;
    box.y = sy;
    box.w = sw;
    box.h = 1;                                        
    SDL_FillRect(surface,&box,Color);
    box.y = sy + sh;
    SDL_FillRect(surface,&box,Color);
    box.y = sy;
    box.w = 1;
    box.h = sh;
    SDL_FillRect(surface,&box,Color);
    box.x = sx + sw;
    SDL_FillRect(surface,&box,Color);
}

/*draws a filled rect at the coordinates, in the color, on the surface specified*/
void DrawFilledRect(int sx,int sy, int sw, int sh, Uint32 Color, arSurface *surface)
{
  arRect dst;
  dst.x = sx;
  dst.y = sy;
  dst.w = sw;
  dst.h = sh;
  SDL_FillRect(surface,&dst,Color);
}

void DrawThickLine(int sx,int sy,int dx, int dy,int width,Uint32 Color,arSurface *surface)
{
  arRect box;
  int deltax,deltay;
  int x,y,curpixel;
  int den,num,numadd,numpixels;
  int xinc1,xinc2,yinc1,yinc2;
  box.w = width;
  box.h = width;
  deltax = fabs(dx - sx);        // The difference between the x's
  deltay = fabs(dy - sy);        // The difference between the y's
  x = sx;                       // Start x off at the first pixel
  y = sy;                       // Start y off at the first pixel

  if (dx >= sx)                 // The x-values are increasing
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          // The x-values are decreasing
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (dy >= sy)                 // The y-values are increasing
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          // The y-values are decreasing
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         // There is at least one x-value for every y-value
  {
    xinc1 = 0;                  // Don't change the x when numerator >= denominator
    yinc2 = 0;                  // Don't change the y for every iteration
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         // There are more x-values than y-values
  }
  else                          // There is at least one y-value for every x-value
  {
    xinc2 = 0;                  // Don't change the x for every iteration
    yinc1 = 0;                  // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         // There are more y-values than x-values
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    box.x = x;
    box.y = y;
    SDL_FillRect(surface,&box,Color);    
    num += numadd;              // Increase the numerator by the top of the fraction
    if (num >= den)             // Check if numerator >= denominator
    {
      num -= den;               // Calculate the new numerator value
      x += xinc1;               // Change the x as appropriate
      y += yinc1;               // Change the y as appropriate
    }
    x += xinc2;                 // Change the x as appropriate
    y += yinc2;                 // Change the y as appropriate
  }
}

void DrawAnyLine(int sx,int sy,int dx, int dy,Uint32 Color,arSurface *surface)
{
  int deltax,deltay;
  int x,y,curpixel;
  int den,num,numadd,numpixels;
  int xinc1,xinc2,yinc1,yinc2;
  deltax = fabs(dx - sx);        // The difference between the x's
  deltay = fabs(dy - sy);        // The difference between the y's
  x = sx;                       // Start x off at the first pixel
  y = sy;                       // Start y off at the first pixel

  if (dx >= sx)                 // The x-values are increasing
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          // The x-values are decreasing
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (dy >= sy)                 // The y-values are increasing
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          // The y-values are decreasing
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         // There is at least one x-value for every y-value
  {
    xinc1 = 0;                  // Don't change the x when numerator >= denominator
    yinc2 = 0;                  // Don't change the y for every iteration
    den = deltax;
    num = deltax >> 1;
    numadd = deltay;
    numpixels = deltax;         // There are more x-values than y-values
  }
  else                          // There is at least one y-value for every x-value
  {
    xinc2 = 0;                  // Don't change the x for every iteration
    yinc1 = 0;                  // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay >> 1;
    numadd = deltax;
    numpixels = deltay;         // There are more y-values than x-values
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    putpixel(surface,x, y,Color);             // Draw the current pixel
    num += numadd;              // Increase the numerator by the top of the fraction
    if (num >= den)             // Check if numerator >= denominator
    {
      num -= den;               // Calculate the new numerator value
      x += xinc1;               // Change the x as appropriate
      y += yinc1;               // Change the y as appropriate
    }
    x += xinc2;                 // Change the x as appropriate
    y += yinc2;                 // Change the y as appropriate
  }
}


/*sets an sdl surface to all color.*/

void BlankScreen(arSurface *buf,Uint32 color)
{
    SDL_LockSurface(buf);
    memset(buf->pixels, (Uint8)color,buf->format->BytesPerPixel * buf->w *buf->h);
    SDL_UnlockSurface(buf);
}

void ResetBuffer()
{
  /*Blit BGimage to background and then blit the tile map to that. for paralax*/
  SDL_BlitSurface(bgimage,&Camera,screen,NULL);
  SDL_BlitSurface(background,&Camera,screen,NULL);
}