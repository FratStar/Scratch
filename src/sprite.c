#include "graphics.h"
#include "ltypes.h"
#include "sprite.h"
/*#include <physfs.h>*/


extern arSurface *screen; /*pointer to the draw buffer*/
extern arSurface *background;/*pointer to the background image buffer*/
extern arSurface *bgimage;
extern arSurface *videobuffer;
extern arRect Camera; /*x & y are the coordinates for the background map, w and h are of the screen*/
extern arUI32 NOW;  /*this represents the current time for the game loop.  Things move according to time*/

arUint MaxSprites = 511;
Sprite *__spriteList = NULL;
int numsprites;

void InitSpriteList()
{
	FILE *cfgfile = NULL;
	int x;
	char buff[255];
	numsprites = 0;

	cfgfile = fopen("system/sprite.cfg", "r");

	if(!cfgfile)
	{
		fprintf(stderr, "unable to open file: %s\n",cfgfile);
	}
	while(fscanf(cfgfile, "%s",buff) != EOF)
	{
		if(strcmp(buff,"maxSprites:") == 0)
		{
			fscanf(cfgfile,"%i",&x);
			MaxSprites = x;
		}
	}
	__spriteList = (Sprite *)malloc(sizeof(Sprite) *MaxSprites);
	if(__spriteList == NULL)
	{
		fprintf(stderr, "initspriteList: FATAL: cannot allocate sprite list.\n");
		exit(-1);
		return;
	}
	memset(__spriteList,0,sizeof(Sprite) *MaxSprites);
	for(x = 0; x < MaxSprites; x++)
		__spriteList[x].image = NULL;
}

/**

  Load an Image through PhysFS and SDL_Image

**/
/*
arSurface *LoadImage(char *filename)
{
	arSurface *image = NULL;
	SDL_RWops *rw;
	lpSI64 size;
	lpFile *file;
	char *buff = NULL;
	file = PHYSFS_openRead(filename);

	if(file == NULL)
	{
		fprintf(stderr,"Unable to load file %s\n",filename);
		return NULL;
	}
	size = PHYSFS_fileLength(file);
	buff = (char *)malloc(size);
	if(buff == NULL)
	{
		fprintf(stderr,"Unable to allocate space for loading image: %s\n",filename);
		PHYSFS_close(file);
		return NULL;
	}
	PHYSFS_read(file,buff,size,1);
	rw = SDL_RWFromMem(buff, size);
	if(rw == NULL)
	{
		fprintf(stderr,"Unable to create RW reference to data loaded for %s\n",filename);
		free(buff);
		PHYSFS_close(file);
		return NULL;
	}
	image = IMG_Load_RW(rw,0);
	SDL_FreeRW(rw);
	free(buff);
	PHYSFS_close(file);
	return image;
}*/

void DrawSprite(Sprite *sprite, arSurface *surface, int sx, int sy, int frame)
{
	SDL_Rect src,dest;
    src.x = frame%sprite->framesperline * sprite->w;
    src.y = frame/sprite->framesperline * sprite->h;
    src.w = sprite->w;
    src.h = sprite->h;
    dest.x = sx;
    dest.y = sy;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite->image, &src, surface, &dest);
}

Sprite *LoadSprite(char *filename, int sizex, int sizey)
{
	return LoadSwappedSprite(filename, sizex, sizey, -1, -1, -1);	
}

Sprite *LoadSwappedSprite(char *filename,int sizex, int sizey, int c1, int c2, int c3)
{
	int i, k, j;
	int n;
	arSurface *temp;


	for( i = 0; i < numsprites; i++)
	{
		if((strncmp(filename,__spriteList[i].filename,80) == 0) && (__spriteList[i].loaded == 1) /*&& (__spriteList[i].used >= 1)*/ && (c1 == __spriteList[i].color1) && (c2 == __spriteList[i].color2) && (c3 == __spriteList[i].color3) && (sizex == __spriteList[i].w) && (sizey == __spriteList[i].h))
		{
			__spriteList[i].used++;
			return &__spriteList[i];
		}
	}
	if(numsprites + 1 >= MaxSprites)
	{
		fprintf(stderr, "Maximum number of sprites reached.\n");
		exit(1);
	}
	numsprites++;
	for( i = 0; i <= numsprites; i++)
	{
		if(!__spriteList[i].used)
			break;
	}
	temp = IMG_Load(filename);
	if(temp == NULL)
	{
		fprintf(stderr, "FAILED TO LOAD A VITAL SPRITE");
		exit(1);
	}
	__spriteList[i].image = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);
	SDL_SetColorKey(__spriteList[i].image, SDL_SRCCOLORKEY, SDL_MapRGBA(__spriteList[i].image->format,0,0,0,255));
	SwapSprite(__spriteList[i].image,c1,c2,c3);
	strcpy(__spriteList[i].filename,filename);
	__spriteList[i].framesperline = 5;
	__spriteList[i].w = sizex;
	__spriteList[i].h = sizey;
	__spriteList[i].color1 = c1;
	__spriteList[i].color2 = c2;
	__spriteList[i].color3 = c3;
	__spriteList[i].used++;
	__spriteList[i].loaded = 1;
	return &__spriteList[i];
}

void FreeSprite(Sprite *sprite)
{
	sprite->used--;
	if(sprite->used == 0)
	{
		strcpy(sprite->filename, "\0");
		if(sprite->image != NULL)
			SDL_FreeSurface(sprite->image);
		sprite->image = NULL;
	}
}

void CloseSprites()
{
	int i;
	for( i = 0; i < MaxSprites; i++)
	{
		FreeSprite(&__spriteList[i]);
	}

}

/*
 * and now bringing it all together, we swap the pure colors in the sprite out
 * and put the new colors in.  This maintains any of the artist's shading and
 * detail, but still lets us have that old school palette swapping.
 */
void SwapSprite(arSurface *sprite,int color1,int color2,int color3)
{
    int x, y;
	arSurface *temp;
    arUI32 pixel,pixel2;
    arUI32 Key = sprite->format->colorkey;
   /*First the precautions, that are tedious, but necessary*/
    if(color1 == -1)return;
    if(sprite == NULL)return;
    temp = SDL_DisplayFormat(sprite);
    if ( SDL_LockSurface(temp) < 0 )
    {
        fprintf(stderr, "Can't lock surface: %s\n", SDL_GetError());
        exit(1);
    }
   /*now step through our sprite, pixel by pixel*/
    for(y = 0;y < sprite->h ;y++)
    {
        for(x = 0;x < sprite->w ;x++)
        {
            pixel = getpixel(temp,x,y);/*and swap it*/
			if(pixel != Key)
			{
			  pixel2 = SetColor(pixel,color1,color2,color3);
              putpixel(sprite,x,y,pixel2);
			}
        }
    }
    SDL_UnlockSurface(temp);
	SDL_FreeSurface(temp);
}



arUI32 SetColor(arUI32 color, int newcolor1,int newcolor2, int newcolor3)
{
    arUI8 r,g,b;
    arUI8 intensity;
	int newcolor;
    SDL_GetRGB(color, screen->format, &r, &g, &b);
    if((r == 0) && (g == 0)&&(b !=0))
    {
        intensity = b;
        newcolor = newcolor3;
    }
    else if((r ==0)&&(b == 0)&&(g != 0))
    {
        intensity = g;
        newcolor = newcolor2;
    }
    else if((g == 0)&&(b == 0)&&(r != 0))
    {
        intensity = r;
        newcolor = newcolor1;
    }
    else return color;
    switch(newcolor)
    {
        case Red:
            r = intensity;
            g = 0;
            b = 0;
            break;
        case Green:
            r = 0;
            g = intensity;
            b = 0;
            break;
        case Blue:
            r = 0;
            g = 0;
            b = intensity;
            break;
        case Yellow:
            r = (Uint8)(intensity * 0.7);
            g = (Uint8)(intensity * 0.7);
            b = 0;
            break;
        case Orange:
            r = (Uint8)(intensity * 0.9);
            g = (Uint8)(intensity * 0.4);
            b = (Uint8)(intensity * 0.1);
            break;
        case Violet:
            r = (Uint8)(intensity * 0.7);
            g = 0;
            b = (Uint8)(intensity * 0.7);
            break;
        case Brown:
            r = (Uint8)(intensity * 0.6);
            g = (Uint8)(intensity * 0.3);
            b = (Uint8)(intensity * 0.15);
            break;
        case Grey:
            r = (Uint8)(intensity * 0.5);
            g = (Uint8)(intensity * 0.5);
            b = (Uint8)(intensity * 0.5);
            break;
        case DarkRed:
            r = (Uint8)(intensity * 0.5);
            g = 0;
            b = 0;
            break;
        case DarkGreen:
            r = 0;
            g = (Uint8)(intensity * 0.5);
            b = 0;
            break;
        case DarkBlue:
            r = 0;
            g = 0;
            b = (Uint8)(intensity * 0.5);
            break;
        case DarkYellow:
            r = (Uint8)(intensity * 0.4);
            g = (Uint8)(intensity * 0.4);
            b = 0;
            break;
        case DarkOrange:
            r = (Uint8)(intensity * 0.6);
            g = (Uint8)(intensity * 0.2);
            b = (Uint8)(intensity * 0.1);
            break;
        case DarkViolet:
            r = (Uint8)(intensity * 0.4);
            g = 0;
            b = (Uint8)(intensity * 0.4);
            break;
        case DarkBrown:
            r = (Uint8)(intensity * 0.2);
            g = (Uint8)(intensity * 0.1);
            b = (Uint8)(intensity * 0.05);
            break;
        case DarkGrey:
            r = (Uint8)(intensity * 0.3);
            g = (Uint8)(intensity * 0.3);
            b = (Uint8)(intensity * 0.3);
            break;
        case LightRed:
            r = intensity;
            g = (Uint8)(intensity * 0.45);
            b = (Uint8)(intensity * 0.45);
            break;
        case LightGreen:
            r = (Uint8)(intensity * 0.45);
            g = intensity;
            b = (Uint8)(intensity * 0.45);
            break;
        case LightBlue:
            r = (Uint8)(intensity * 0.45);
            b = intensity;
            g = (Uint8)(intensity * 0.45);
            break;
        case LightYellow:
            r = intensity;
            g = intensity;
            b = (Uint8)(intensity * 0.45);
            break;
        case LightOrange:
            r = intensity;
            g = (Uint8)(intensity * 0.75);
            b = (Uint8)(intensity * 0.35);
            break;
        case LightViolet:
            r = intensity;
            g = (Uint8)(intensity * 0.45);
            b = intensity;
            break;
        case LightBrown:
            r = intensity;
            g = (Uint8)(intensity * 0.85);
            b = (Uint8)(intensity * 0.45);
            break;
        case LightGrey:
            r = (Uint8)(intensity * 0.85);
            g = (Uint8)(intensity * 0.85);
            b = (Uint8)(intensity * 0.85);
            break;
        case Black:
            r = (Uint8)(intensity * 0.15);
            g = (Uint8)(intensity * 0.15);
            b = (Uint8)(intensity * 0.15);
            break;
        case White:
            r = intensity;
            g = intensity;
            b = intensity;
            break;
        case Tan:
            r = intensity;
            g = (Uint8)(intensity * 0.9);
            b = (Uint8)(intensity * 0.6);
            break;
        case Gold:
            r = (Uint8)(intensity * 0.8);
            g = (Uint8)(intensity * 0.7);
            b = (Uint8)(intensity * 0.2);
            break;
        case Silver:
            r = (Uint8)(intensity * 0.95);
            g = (Uint8)(intensity * 0.95);
            b = intensity;
            break;
        case YellowGreen:
            r = (Uint8)(intensity * 0.45);
            g = (Uint8)(intensity * 0.75);
            b = (Uint8)(intensity * 0.2);
            break;
        case Cyan:
            r = 0;
            g = (Uint8)(intensity * 0.85);
            b = (Uint8)(intensity * 0.85);
            break;
        case Magenta:
            r = (Uint8)(intensity * 0.7);
            g = 0;
            b = (Uint8)(intensity * 0.7);
            break;
		default:
            r = 0;
            g = (Uint8)(intensity * 0.85);
            b = (Uint8)(intensity * 0.85);

			break;
    }
	color = SDL_MapRGB(screen->format,r,g,b);
//    fprintf(stderr,"newcolor: %d, asked for: %d,%d,%d \n",color,newcolor1,newcolor2,newcolor3); 
    return color;
}



arUI32 IndexColor(int color)
{
    switch(color)
    {
    case Red:
        return SDL_MapRGB(screen->format,138,0,0);;
    case Green:
        return SDL_MapRGB(screen->format,0,138,0);;
    case Blue:
        return SDL_MapRGB(screen->format,0,0,138);;
    case Yellow:
        return SDL_MapRGB(screen->format,128,128,0);;
    case Orange:
        return SDL_MapRGB(screen->format,148,118,0);;
    case Violet:
        return SDL_MapRGB(screen->format,128,0,128);
    case Brown:
        return SDL_MapRGB(screen->format,100,64,4);
    case Grey:
        return SDL_MapRGB(screen->format,128,128,128);
    case DarkRed:
        return SDL_MapRGB(screen->format,64,0,0);
    case DarkGreen:
        return SDL_MapRGB(screen->format,0,64,0);
    case DarkBlue:
        return SDL_MapRGB(screen->format,0,0,64);
    case DarkYellow:
        return SDL_MapRGB(screen->format,60,60,0);
    case DarkOrange:
        return SDL_MapRGB(screen->format,64,56,0);
    case DarkViolet:
        return SDL_MapRGB(screen->format,60,0,60);
    case DarkBrown:
        return SDL_MapRGB(screen->format,56,32,2);
    case DarkGrey:
        return SDL_MapRGB(screen->format,64,64,64);
    case LightRed:
        return SDL_MapRGB(screen->format,255,32,32);
    case LightGreen:
        return SDL_MapRGB(screen->format,32,255,32);
    case LightBlue:
        return SDL_MapRGB(screen->format,32,32,255);
    case LightYellow:
        return SDL_MapRGB(screen->format,250,250,60);
    case LightOrange:
        return SDL_MapRGB(screen->format,255,234,30);
    case LightViolet:
        return SDL_MapRGB(screen->format,250,30,250);
    case LightBrown:
        return SDL_MapRGB(screen->format,200,100,32);
    case LightGrey:
        return SDL_MapRGB(screen->format,196,196,196);
    case Black:
        return SDL_MapRGB(screen->format,0,0,0);
    case White:
        return SDL_MapRGB(screen->format,255,255,255);
    case Tan:
        return SDL_MapRGB(screen->format,255,128,64);
    case Gold:
        return SDL_MapRGB(screen->format,255,245,30);
    case Silver:
        return SDL_MapRGB(screen->format,206,206,226);
    case YellowGreen:
        return SDL_MapRGB(screen->format,196,255,30);
    case Cyan:
        return SDL_MapRGB(screen->format,0,255,255);;
    case Magenta:
        return SDL_MapRGB(screen->format,255,0,255);
    }
    return SDL_MapRGB(screen->format,0,0,0);
}
