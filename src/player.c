#include <stdlib.h>
#include <string.h>
#include "player.h"
//#include "projectiles.h"/*the player won't need this when the game is done*/
//#include "particle.h"
#include "sprite.h"
#include "Level.h"
//#include "weapons.h"

#define  JT_Button  0
#define  JT_Axis    1
#define  JT_Inverse 2
#define  Wiggle     6000
#define  AWiggle    -6000

extern arSurface *screen;
arJoystick *joy;
extern arRect Camera;
extern int NumLevels;
extern int CurrentLevel;
int NumLives = 3;
extern int MOUSEMOVE;
int lastx,lasty; /*last mouse coordinates*/

enum PlayerInputs {PI_MovDown,PI_MovDownLeft,PI_MovLeft,PI_MovUpLeft,PI_MovUp,PI_MovUpRight,PI_MovRight,PI_MovDownRight, PI_Fire, PI_Pause, PI_NULL};

int JoyButtons[PI_NULL][2];
Uint32 KeyButtons[PI_NULL];
int PlayerCommands[3]; 
Entity *ThePlayer = NULL;
void UpdatePlayer(Entity *self);
void PlayerThink(Entity *self);
void FinalOutput();
void UpdateInput();

void SpawnPlayer(int x,int y)
{
	Entity *newent = NULL;
	newent = NewEntity();
	if(newent == NULL)
	{
		fprintf(stderr,"Unable to generate player entity; %s",SDL_GetError());
		exit(0);
	}
	strcpy(newent->EntName,"Player\0");
	newent->sprite = LoadSprite("images/playersprites/playersheet.png",55,69);
	SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGBA(newent->sprite->image->format, 0,0,0, 255));
	newent->size.x = newent->sprite->w;
	newent->size.y = newent->sprite->h;
	//newent->update = UpdatePlayer;
	newent->think = PlayerThink;
	newent->UpdateRate = 35;
	newent->ThinkRate = 35;
	newent->takedamage = 1;

	newent->Unit_Type = ET_Player;
	newent->healthmax = 600;
	newent->health = 600;
	newent->frame = 0;
	//	newent->fcount = B_Tiny;
	newent->frate = 1;
	newent->state = ST_IDLE;
	newent->s.x = x;
	newent->s.y = y;
	newent->v.x = 0;
	newent->v.y = 0;
	newent->maxspeed = 4;
	newent->movespeed = 0;
	newent->Boundingbox.x = 10;
	newent->Boundingbox.y = 26;
	newent->Boundingbox.w = 40;
	newent->Boundingbox.h = 67;  
	newent->origin.x = 50;
	newent->origin.y = 50;
	newent->currentweapon = 1;
	ThePlayer = newent;
	atexit(FinalOutput);
}

void PlayerThink(Entity *self)
{
	int my, i;
	float t;
	char *text;
	Uint8 *keys = SDL_GetKeyState(NULL);
	UpdateInput();

	if(self->health > 0)
	{
		if(self->v.y != 0)
		{
			if(self->v.y > 0)self->v.y -= 2;
			else self->v.y += 2;
		}
		SDL_GetMouseState(NULL,&my);
		if(my != lasty)
		{
			lasty = my;
			self->v.y = 0;
			if((my > self->s.y +5 )||(my < self->s.y -5 ))self->v.y = my - self->s.y;
		}
		self->s.x += self->v.x;
		self->s.y += self->v.y;
		if(self->s.y < 0)self->s.y = 0;
		if(self->s.y > (screen->h - 64))self->s.y = (screen->h - 64);
		if(self->s.x > Camera.w)
		{
			/*We have completed a level, load a new one*/
			if(CurrentLevel + 1 < NumLevels)CurrentLevel++;
			else exit(1);
			GenerateLevel(GetCurrentLevel(CurrentLevel));
			self->s.x = 100;
			self->v.x = 0;
		}
	}
	if((self->health <= 0)&&(self->state != ST_DYING)&&(self->state != ST_DEAD))
	{
		self->health = 0;
		self->state = ST_DYING;
		self->frame = 7;
	}
	if((self->state != ST_DEAD) &&(self->state != ST_DYING))
	{
		if(PlayerCommands[0] == PI_MovUp)
		{
			MOUSEMOVE = 0;
			if(self->v.y > -25)self->v.y -= 5;
		}
		if(PlayerCommands[0] == PI_MovDown)
		{
			MOUSEMOVE = 0;
			if(self->v.y < 25)self->v.y += 5;
		}
		if(PlayerCommands[0] == PI_MovLeft)
		{
			MOUSEMOVE = 0;
			if(self->v.x > -15)self->v.x -= 5;
		}
		if(PlayerCommands[0] == PI_MovRight)
		{
			MOUSEMOVE = 0;
			if(self->v.x < 15)self->v.x += 5;
		}

		if(PlayerCommands[0] == PI_MovDownRight)
		{
			MOUSEMOVE = 0;
			if(self->v.y < 25)self->v.y += 5;
			if(self->v.x < 15)self->v.x += 5;
		}
		if(PlayerCommands[0] == PI_MovDownLeft)
		{
			MOUSEMOVE = 0;
			if(self->v.y < 25)self->v.y += 5;
			if(self->v.x > -15)self->v.x -= 5;
		}

		if(PlayerCommands[0] == PI_MovUpLeft)
		{
			MOUSEMOVE = 0;
			if(self->v.y > -25)self->v.y -= 5;
			if(self->v.x > -15)self->v.x -= 5;
		}

		if(PlayerCommands[0] == PI_MovUpRight)
		{
			MOUSEMOVE = 0;
			if(self->v.y > -25)self->v.y -= 5;
			if(self->v.x < 15)self->v.x += 5;
		}

		if(PlayerCommands[1] == PI_Fire)
		{
			if((self->state == ST_IDLE))
			{
				self->state = ST_FIRE;
			}


		}
		switch(self->state)
		{
		case ST_DEAD:
			self->frame = 5;
			self->shown = 0;
			if(NumLives > 0)
			{
				self->health = self->healthmax;
				self->state = ST_IDLE;
				self->shown = 1;
				NumLives--;
			}
			else exit(1);
			break;
		case ST_DYING:
			if(self->frame >= 5)
			{
				self->state = ST_DEAD;
			}
			self->frame++;
			break;
		case ST_IDLE:
			self->frame = 0;
			if(PlayerCommands[1] == PI_Fire)
			{
				self->frame = 0;
				self->state = ST_FIRE;
			}
				break;
		case ST_FIRE:
			break;
		}

	}
}

int CheckCommand(int i)
{
	Uint8 *keys = SDL_GetKeyState(NULL);
	if(((JoyButtons[i][0] == JT_Axis)&&(SDL_JoystickGetAxis(joy, JoyButtons[i][1]) > Wiggle))
		||((JoyButtons[i][0] == JT_Inverse)&&(SDL_JoystickGetAxis(joy, JoyButtons[i][1]) < AWiggle))
		||((JoyButtons[i][0] == JT_Button)&&(SDL_JoystickGetButton(joy, JoyButtons[i][1])))
		||(keys[KeyButtons[i]]))
		return 1;
	return 0;
}


	void FinalOutput()
	{
		SDL_SaveBMP(screen,"finale.bmp");
	}

void UpdateInput()
{
	int i;
	for(i = 0;i < 3;i++)PlayerCommands[i] = -1;

	i = PI_MovDown;
	if(CheckCommand(i))
		PlayerCommands[0] = i;

	i = PI_MovUp;
	if(CheckCommand(i))
		PlayerCommands[0] = i;

	i = PI_MovLeft;
	if(CheckCommand(i))
	{
		if(PlayerCommands[0] == PI_MovUp)
		{
			PlayerCommands[0] = PI_MovUpLeft; 
		}
		else if(PlayerCommands[0] == PI_MovDown)
		{
			PlayerCommands[0] = PI_MovDownLeft; 
		}
		else PlayerCommands[0] = i;
	}

	i = PI_MovRight;
	if(CheckCommand(i))
	{
		if(PlayerCommands[0] == PI_MovUp)
		{
			PlayerCommands[0] = PI_MovUpRight; 
		}
		else if(PlayerCommands[0] == PI_MovDown)
		{
			PlayerCommands[0] = PI_MovDownRight; 
		}
		else PlayerCommands[0] = i;
	}

	i = PI_Fire;
	if(CheckCommand(i))
	{
		PlayerCommands[1] = i;
	}


}

void DefaultConfig()
{

	KeyButtons[PI_MovDown] = SDLK_DOWN;
	KeyButtons[PI_MovLeft] = SDLK_LEFT;
	KeyButtons[PI_MovUp] = SDLK_UP;
	KeyButtons[PI_MovRight] = SDLK_RIGHT;
	KeyButtons[PI_Fire] = SDLK_s;
	KeyButtons[PI_Pause] = SDLK_ESCAPE;
}



void LoadKeyConfig()
{
	FILE *file;
	file = fopen("saves/config.cfg","r");
	if(file == NULL)
	{
		fprintf(stderr,"unable to load key configuration, falling back on default\n");
		DefaultConfig();
		SaveKeyConfig();
		return;
	}
	if(fread(KeyButtons, sizeof(KeyButtons), 1, file)!= 1)
	{
		fprintf(stderr,"Unable to write config data to file: %s",SDL_GetError());
		return;
	}
	if(fread(JoyButtons, sizeof(JoyButtons), 1, file)!= 1)
	{
		fprintf(stderr,"Unable to write config data to file: %s",SDL_GetError());
		return;
	}
	fclose(file);
}


void SaveKeyConfig()
{
	FILE *file;
	file = fopen("saves/config.cfg","w");
	if(file == NULL)
	{
		fprintf(stderr,"unable to save key configuration, error!\n");
		return;
	}
	if(fwrite(KeyButtons, sizeof(KeyButtons), 1, file)!= 1)
	{
		fprintf(stderr,"Unable to write config data to file: %s",SDL_GetError());
		return;
	}
	if(fwrite(JoyButtons, sizeof(JoyButtons), 1, file)!= 1)
	{
		fprintf(stderr,"Unable to write config data to file: %s",SDL_GetError());
		return;
	}
	fclose(file);
}


