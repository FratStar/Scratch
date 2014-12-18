#include <string.h>
#include <stdlib.h>
//#include "particle.h"
#include "weapons.h"
#include "enemy.h"
#include "player.h"

extern Entity *ThePlayer;
extern arSurface *screen;


enemy *__EnemyList = NULL;
enemy *Evil;
int NumEnemies;
void HeavyThink(Entity *self);
void MinorThink(Entity *self);

void SpawnMinor(Entity *owner, int x, int y, int weapon, int group)
{
	Entity *newent = NULL;
	newent = NewEntity();
	{
		fprintf(stderr,"Unable to generate enemy entity; %s",SDL_GetError());
		return;
	}
	strcpy(newent->EntName,"MinorShip\0");
	newent->sprite = LoadSprite("images/enemies/rtashipsheet.png",48,48);
	SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->sprite->image->format, 0,0,0));
	newent->owner = owner;
	//	newent->update = UpdateEnemy;
	newent->think = MinorThink;
	newent->UpdateRate = 45;
	newent->ThinkRate = 45;
	newent->Unit_Type = group;
	newent->frame = 0;
	newent->fcount = 0;
	newent->frate = 1;
	newent->state = ST_IDLE;
	newent->healthmax = 20;
	newent->health = 20;
	newent->takedamage = 1;
	newent->s.x = x;
	newent->s.y = y;
	newent->currentweapon = 0;
	newent->v.x = 0;
	newent->v.y = 0;
	newent->enemy = 1;
	newent->Boundingbox.x = 8;
	newent->Boundingbox.y = 12;
	newent->Boundingbox.w = 36;
	newent->Boundingbox.h = 26;

}

void SpawnHeavy(Entity *owner, int x, int y, int weapon, int group)
{
	Entity *newent = NULL;
	newent = NewEntity();


	{
		fprintf(stderr,"Unable to generate enemy entity; %s",SDL_GetError());
		return;
	}
	strcpy(newent->EntName,"MinorShip\0");
	newent->sprite = LoadSprite("images/enemies/heas.ysheet.png",48,48);
	SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->sprite->image->format, 0,0,0));
	newent->owner = owner;
	//	newent->update = UpdateEnemy;
	newent->think = HeavyThink;
	//newent->UpdateRate = 45;
	newent->ThinkRate = 45;
	newent->Unit_Type = group;
	newent->frame = 0;
	newent->fcount = 0;
	newent->frate = 1;
	newent->state = ST_IDLE;
	newent->healthmax = 20;
	newent->health = 20;
	newent->takedamage = 1;
	newent->s.x = x;
	newent->s.y = y;
	newent->currentweapon = 0;
	newent->v.x = 0;
	newent->v.y = 0;
	newent->enemy = 1;
	newent->Boundingbox.x = 8;
	newent->Boundingbox.y = 12;
	newent->Boundingbox.w = 36;
	newent->Boundingbox.h = 26;

}

void MinorThink(Entity *self)
{
	self->s.x += self->v.x;
	self->s.y += self->v.y;
	if(self->v.x > -2)self->v.x--;
	if(self->s.y < 0)
	{
		self->s.y = 0;
		self->v.y += 15;
	}
	if(self->s.y > screen->h - 64)
	{
		self->s.y = screen->h - 24;
		self->v.y -= 15;
	}
	if((self->health <= 0) &&(self->state != ST_DYING))
	{
		self->state = ST_DYING;
		self->frame = 1;

	}
	else if((ThePlayer->health > 0) && (self->state != ST_DYING) && (CollisionDetect(self,ThePlayer)))
	{
		ThePlayer->health = 0;
		self->state = ST_DYING;
		self->frame = 3;

	}
	if(self->s.x < - 32)
	{
		self->shown = 0;
		FreeEntity(self);
	}
	switch(self->state)
	{
	case ST_DYING:
		if(self->frame >= 3)
		{
			FreeEntity(self);
		}
		self->frame++;
		break;
	case ST_IDLE:
		self->frame++;
		if(self->frame >= 2)
			self->frame = 0;
		break;
	}
}

void HeavyThink(Entity *self)
{
	self->s.x += self->v.x;
	self->s.y += self->v.y;
	if(self->v.x > -2)self->v.x--;
	if(self->s.y < 0)
	{
		self->s.y = 0;
		self->v.y += 15;
	}
	if(self->s.y > screen->h - 64)
	{
		self->s.y = screen->h - 24;
		self->v.y -= 15;
	}
	if((self->health <= 0) &&(self->state != ST_DYING))
	{
		self->state = ST_DYING;
		self->frame = 1;

	}
	else if((ThePlayer->health > 0) && (self->state != ST_DYING) && (CollisionDetect(self,ThePlayer)))
	{
		ThePlayer->health = 0;
		self->state = ST_DYING;
		self->frame = 1;

	}
	if(self->s.x < - 32)
	{
		self->shown = 0;
		FreeEntity(self);
	}
	switch(self->state)
	{
	case ST_DYING:
		if(self->frame >= 1)
		{
			FreeEntity(self);
		}
		self->frame++;
		break;
	case ST_IDLE:
		self->frame++;
		if(self->frame >= 0)
			self->frame = 0;
		break;
	}
}