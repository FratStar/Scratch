#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "projectile.h"
#include "sprite.h"


void UpdateRocket(Entity *self);
void UpdateBullet(Entity *self);

extern Uint32 NOW;
extern Entity *ThePlayer;
extern Entity *__entityList;
extern arUint MAXENTITIES;

Entity *SpawnProjectile(int sx,int sy,float angle,float speed,float accel,int damage)
{
	float cosine,sine;
	Entity *newent = NULL;
	newent = NewEntity();
	if(newent == NULL)return NULL;
	cosine = cos(angle);
	sine = sin(angle);
	newent->damage = damage;
	newent->shown = 1;
	/*newent->legstate = -1;    /*needed if we don't have separate legs*/
	newent->s.x = sx;
	newent->s.y = sy;
	newent->v.x = speed * cosine;
	newent->v.y = speed * sine;
	if(accel != 0)
	{
		newent->v.x = speed * cosine;
		newent->v.y = speed * sine;
		newent->accel = accel;
	} 
	newent->movespeed = (int)speed;
	/*binding the entity to the engine should happen here as well*/
	return newent;
}

Entity *SpawnRocket(Entity *owner,int sx,int sy,float angle,float speed,float accel,int damage,int UType)
{
	Entity *newent = NULL;
	newent = SpawnProjectile(sx,sy,angle,speed,accel,damage);
	if(newent == NULL)return NULL;
	strcpy(newent->EntName,"Rocket\0");
	newent->sprite = LoadSprite("images/weaponsprites/missle.png",20,17);
	newent->__sound[SF_ALERT] = LoadSound("sounds/deadexplode.wav",SDL_MIX_MAXVOLUME>>3);
	newent->update = UpdateRocket;
	newent->UpdateRate = 30;
	newent->movespeed = speed;
	newent->owner = owner;
	//newent->v.x = vx;
//	newent->v.y = vy;
	newent->Unit_Type = UType;
	newent->lifespan = 60;
	newent->origin.x = 12;
	newent->origin.y = 20;  
	newent->frame = newent->face;
	if(newent->frame < 0)newent->face = 8;
	newent->Boundingbox.x = 2;
	newent->Boundingbox.y = 2;
	newent->Boundingbox.w = 14;
	newent->Boundingbox.h = 14;
	newent->m.x = (sx + newent->origin.x) >> 6;
	newent->m.y = (sy + newent->origin.y) >> 6;
	return newent;
}


void UpdateRocket(Entity *self)
{
	int i;
	Entity *target = NULL;
	self->lifespan--;

	if(self->lifespan <= 0)
	{
		Mix_PlayChannel(Mix_GroupAvailable(2),self->__sound[SF_ALERT]->sound,0);
		FreeEntity(self);
			return;
	}

	if(self->owner==ThePlayer)
	{
		for(i = 0; i < MAXENTITIES; i++)
		{

			if(__entityList[i].enemy==1 && __entityList[i].inuse == 1)
			{
				target = &__entityList[i];
				if(CollisionDetect(self, target) == 1)
				{
					DamageTarget(self->owner,self,target,self->damage);
					Mix_PlayChannel(Mix_GroupAvailable(2),self->__sound[SF_ALERT]->sound,0);
					DamageTarget(self->owner,self,target,self->damage);
					FreeEntity(self);    
				}
			}
		}
	}
}


Entity *SpawnBullet(Entity *owner,int sx,int sy,float angle,float speed,int damage,int UType)
{

	Entity *newent = NULL;
	newent = SpawnProjectile(sx,sy,angle,speed,0,damage);
	if(newent == NULL)return NULL;
	newent->sprite = LoadSprite("images/weaponsprites/bullet.png",5,5);
	newent->origin.x = 50;
		newent->origin.y = 30;
		newent->Boundingbox.x = 1;
		newent->Boundingbox.y = 1;
		newent->Boundingbox.w = 4;
		newent->Boundingbox.h = 4;
		newent->__sound[SF_ALERT] = LoadSound("sounds/ric1.wav",SDL_MIX_MAXVOLUME>>4);
		SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGBA(newent->sprite->image->format, 0,0,0,255));
	newent->frame = 0;
	newent->owner = owner;
	newent->update = UpdateBullet;
	newent->UpdateRate = 30;
	newent->Unit_Type = UType;
	newent->lifespan = 60;
	newent->m.x = (sx + newent->origin.x) >> 6;
	newent->m.y = (sy + newent->origin.y) >> 6;
	return newent;
}
void UpdateBullet(Entity *self)
{
	int i;
	Entity *target = NULL;
	self->lifespan--;
	if(self->lifespan <= 0)
	{
		FreeEntity(self);
		return;  
	}
	if(self->owner==ThePlayer)
	{
		if(CollisionDetect(self, ThePlayer) == 1)
		{
			ThePlayer->takedamage = 0;
		}
		else
		{
			ThePlayer->takedamage = 1;
		}
		for(i = 0; i < MAXENTITIES; i++)
		{

			if(__entityList[i].enemy==1 && __entityList[i].inuse == 1)
			{
				target = &__entityList[i];
				if(CollisionDetect(self, target) == 1)
				{
					DamageTarget(self->owner,self,target,self->damage);
					FreeEntity(self);
				}
			}
		}
	}
	
		if(self->owner != ThePlayer)
		{
				if(CollisionDetect(self, ThePlayer) == 1)
				{
					target = ThePlayer;
					DamageTarget(self->owner,self,target,self->damage);
					FreeEntity(self);
				}
			}
	

	/*if(target != NULL)
	{
	DamageTarget(self->owner,self,target,self->damage,self->dtype,self->kick,self->v.x,self->v.y);
	FreeEntity(self);    
	}*/
}

/*************************************************************

Projectile Support Functions

*************************************************************/
void PrecacheProjSounds()
{
	LoadSound("sounds/deadexplode.wav",SDL_MIX_MAXVOLUME>>2);
	LoadSound("sounds/explode.wav",SDL_MIX_MAXVOLUME>>3);
	LoadSound("sounds/bluehit.wav",SDL_MIX_MAXVOLUME>>3);
	LoadSound("sounds/machgf1b.wav",SDL_MIX_MAXVOLUME>>3);
	LoadSound("sounds/machinexplode.wav",SDL_MIX_MAXVOLUME);
	LoadSound("sounds/xfire.wav",SDL_MIX_MAXVOLUME>>2);
	LoadSound("sounds/ric1.wav",SDL_MIX_MAXVOLUME>>4);
	LoadSound("sounds/ric2.wav",SDL_MIX_MAXVOLUME>>3);
	LoadSound("sounds/ric3.wav",SDL_MIX_MAXVOLUME>>2);
	LoadSound("sounds/hgrenb1a.wav",SDL_MIX_MAXVOLUME>>4);
	LoadSound("sounds/grenlb1b.wav",SDL_MIX_MAXVOLUME>>4);
	LoadSound("sounds/bang.wav",SDL_MIX_MAXVOLUME>>3);
}