#include <stdlib.h>
#include "SDL.h"
#include "graphics.h"
#include "enemy.h"
#include "spawn.h"
#include "sprite.h"
#include "player.h"
#include "projectile.h"
#include "weapons.h"
#include "window.h"
#include "font.h"
#include "entity.h"
#include "Level.h"
#include "audio.h"
#include "ltypes.h"

extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Rect Camera;
extern Entity *Mouse;

void Init_All();

#define MAXSTATE 1


extern arSurface *screen;
extern arSurface *bgimage;
extern arSurface *background;
extern arRect Camera;
extern Entity *ThePlayer;
extern Level level;
extern arSurface *clipmask;
int lvl = 0;
int drawents = 1,drawboxes = 0;
int windowed = 1;
int mapeditmode = 0;
int bucketdraw = 0;
Sound *gamesounds[4];
SDL_Joystick *joy;
arUI8 *keys;

void Init_All();
void Update_ALL();
int Think_ALL();
void Draw_ALL();
void DrawSplashScreen();
void GiveInfo();
void PopUpWindow(Sprite *sprite,char *text,arUI32 Color,int pwait);
void UpdateCamera();
void UpdateMapCamera();

int main(int argc, char *argv[])
{

	FILE *file = NULL;
	int done = 0;
	int i;
	char string[40];
	SDLMod mod;
	string[0] = '\0';
	for(i = 1;i < argc;i++)
	{
		if(strcmp("-nodraw",argv[i])== 0)
		{
			drawents = 0;
			drawboxes = 1;
		}
		else if(strcmp("-windowed",argv[i])== 0)
		{
			windowed = 1;
		}
		else if(strcmp("-drawall",argv[i])== 0)
		{
			drawents = 1;
			drawboxes = 1;
		}
		else if(strcmp("-mapedit",argv[i])== 0)
		{
			mapeditmode = 1;
		}
		else if(strcmp("-buckets",argv[i])== 0)
		{
			bucketdraw = 1;
		}
	}
	Init_All();
	if(lvl==1)printf("woot");
	GiveInfo();
	if(!mapeditmode)
	{
		SpawnAll(1);
		if(ThePlayer == NULL)SpawnPlayer(128,128);
	}
	do
	{
		ResetBuffer();
		SDL_PumpEvents();
		keys = SDL_GetKeyState(NULL);    
		mod = SDL_GetModState();
		Draw_ALL();
		if(keys[SDLK_ESCAPE] == 1)done = 1;
		Update_ALL();
		if(keys[SDLK_b] == 1)
		{
			SDL_SaveBMP(screen,"screen0.bmp");
			NewMessage("Screen Shot Saved",IndexColor(LightBlue));
		}
		NextFrame();
	}while(!done);

	exit(0);
	return 0;
}

void CleanUpAll()
{
	CloseSprites();
	ClearEntities();
	ClearSoundList();
	SDL_FreeSurface(clipmask);
	/*any other cleanup functions can be added here*/ 
}

void Init_All()
{
	Init_Graphics(windowed);
	InitSpriteList();
	Init_Audio();
	InitSoundList();
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	atexit(CleanUpAll);
	LoadFonts();
	InitMessages();
	InitEntityList();
/*	LoadHUD();*/
	GenerateLevel(1);
	LoadKeyConfig();
	DrawLevel();
	PrecacheWeaponSounds();
}
void GiveInfo()
{
	NewMessage("Press Esc to Quit",IndexColor(White));
}


void Update_ALL()
{
	int i;
	Uint8 *keys;
	if(!mapeditmode)
	{
		UpdateEntities();
		UpdateCamera();
	}
	else
	{
		UpdateMouse();
		UpdateMapCamera();
	}
}

/*calls all of the think function for everything*/
int Think_ALL()
{
	int done = 0;
	ThinkEntities();
	return done;
}

/*calls all of the draw functions for everything*/
void Draw_ALL()
{
	if(drawents)
	{
		DrawEntities();
		if((!mapeditmode)&&(ThePlayer != NULL))DrawEntity(ThePlayer);
	}
	if(drawboxes)return;
	DrawMessages();
	if(!mapeditmode)return;
	else
	{
		DrawSpawnPoints();
		DrawMouse();
	}
}



void UpdateMapCamera()
{
	int mx,my;
	SDL_GetMouseState(&mx,&my);
	if((mx < 10)&&(Camera.x >= 10))  Camera.x -= 10;
	if((mx > screen->w - 10)&&(Camera.x <= (background->w - screen->w) - 10))Camera.x += 10;
	if((my < 10)&&(Camera.y >= 10))  Camera.y -= 10;
	if((my > screen->h - 10)&&(Camera.y <= (background->h - (screen->h - 100)) - 10))Camera.y += 10;
}

void UpdateCamera()
{
	Camera.x = (int)ThePlayer->s.x - (Camera.w >> 1);
	Camera.y = (int)ThePlayer->s.y - (Camera.h >> 1);
	if(Camera.x > background->w - Camera.w)Camera.x = background->w - Camera.w;
	if(Camera.x < 0)Camera.x = 0;
	if(Camera.y > background->h - Camera.h)Camera.y = background->h - Camera.h;
	if(Camera.y < 0)Camera.y = 0;

}