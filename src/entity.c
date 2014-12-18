#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "entity.h"
#include "sprite.h"
#include "Level.h"

/*
Entity function definitions
*/
#define   Depth       16
#define   RPixel_W    64
#define   RPixel_H    64

Entity *__entityList = NULL;
arUint MAXENTITIES = 1024;
arUint MAXSTATES =	10;
arUint MAXPATH =	20;
arUint QMAX =		10;
arUint PERSISTENCE =  8;

extern SDL_Surface *screen;
extern SDL_Surface *clipmask;
extern SDL_Surface *background;
extern SDL_Rect Camera;
extern Uint32 NOW;
extern Level level;
extern int CurrLevel;

int numents = 0;
int MOUSEMOVE = 1;

void InitEntityList()
{
	FILE *cfgfile = NULL;
	int x;
	char buff[255];
	cfgfile = fopen("system/entity.cfg", "r");	

	if(!cfgfile)
	{
		fprintf(stderr,"unable to open file: %s\n",cfgfile);
	}
	while(fscanf(cfgfile,"%s",buff) != EOF)
	{
		if(strcmp(buff,"maxEnts:") == 0)
		{
			fscanf(cfgfile,"%i", &x);
			MAXENTITIES = x;
		}
		if(strcmp(buff,"QMax:") == 0)
		{
			fscanf(cfgfile,"%i", &x);
			QMAX = x;
		}
		/*if(strcmp(buff,"SoundsPerEnt:") == 0)
		{
			fscanf(cfgfile,"%i", &x);
			SOUNDSPERENT = x;
		}*/
		if(strcmp(buff,"States:") == 0)
		{
			fscanf(cfgfile,"%i", &x);
			MAXSTATES = x;
		}
		/*if(strcmp(buff,"MaxTrail:") == 0)
		{
		fscanf(cfgfile,"%i", &x);
		MAXTRAIL = x;
		}*/
		if(strcmp(buff,"Persistence:") == 0)
		{
			fscanf(cfgfile,"%i", &x);
			PERSISTENCE = x;
		}
		if(strcmp(buff,"MaxPath:") == 0)
		{
			fscanf(cfgfile,"%i", &x);
			MAXPATH = x;
		}
	}

	__entityList = (Entity *)malloc(sizeof(Entity) *MAXENTITIES);
	if(__entityList == NULL)
	{
		fprintf(stderr, "initentityList: FATAL: cannol allocate entity list\n");
		exit(-1);
		return;
	}
	memset(__entityList,0,sizeof(Entity) *MAXENTITIES);
	/*lets make sure our deadspace entity is blank and safe.*/

	fclose(cfgfile);
}

void ClearEntitiesExcept(Entity *skip)
{
	int i;
	for(i = 0;i < MAXENTITIES;i++)
	{
		if((__entityList[i].inuse)&&(skip != &__entityList[i]))
		{
			FreeEntity(&__entityList[i]);
		}
	}
}

void DrawEntities()
{
	int i;
	for(i = 0;i < MAXENTITIES;i++)
	{
		if(__entityList[i].inuse == 1)
		{
			if(((__entityList[i].s.x + __entityList[i].size.x) >= Camera.x - 200)&&(__entityList[i].s.x <= (Camera.x + Camera.w + 200))&&((__entityList[i].s.y + __entityList[i].size.y) >= Camera.y - 200)&&(__entityList[i].s.y <= (Camera.y + Camera.h) + 200))
			{
				DrawEntity(&__entityList[i]);
			}
		}
	}
}

void ThinkEntities()
{
	int i;
	int checked = 0;
	for(i = 0;i < MAXENTITIES;i++)
	{
		if(__entityList[i].inuse)
		{
			checked++;
			if(__entityList[i].NextThink < NOW)
			{
				if(__entityList[i].think != NULL)
				{
					__entityList[i].think(&__entityList[i]);
					__entityList[i].NextThink = NOW + __entityList[i].ThinkRate;
				}
			}
		}
	}
}

void UpdateEntities()
{
	int i;
	int checked = 0;
	for(i = 0;i < MAXENTITIES;i++)
	{
		if(__entityList[i].inuse)
		{
			checked++;
			if(__entityList[i].NextUpdate < NOW)
			{
				if(__entityList[i].update != NULL)
				{
					__entityList[i].update(&__entityList[i]);
					__entityList[i].NextUpdate = NOW + __entityList[i].UpdateRate;
				}
			}
		}
	}
}

void DrawEntity(Entity *ent)
{
	if(ent->sprite != NULL)
		DrawSprite(ent->sprite,screen,ent->s.x - Camera.x,ent->s.y - Camera.y,ent->frame);
	return;    
}
Entity *NewEntity()
{
	int i;
	if(numents + 1 >= MAXENTITIES)
	{
		return NULL;
	}
	numents++;
	for(i = 0;i < MAXENTITIES;i++)
	{
		if(__entityList[i].inuse == 0)
		{
			__entityList[i].inuse = 1;
			return &__entityList[i];
		}
	}
	return NULL;

}

void FreeEntity(Entity *ent)
{
  int j;
  /*  fprintf(stdout,"Freeing %s\n",ent->EntName);*/
  ent->inuse = 0;
  numents--;  
  if(ent->sprite != NULL)FreeSprite(ent->sprite);

  for(j = 0; j < SOUNDSPERENT; j++)
  {
    if(ent->__sound[j] != NULL)FreeSound(ent->__sound[j]);
    ent->__sound[j] = NULL;
  }
 /* ent->legs = NULL;*/
  ent->sprite = NULL;
  ent->owner = NULL;
  ent->think = NULL;
  ent->target = NULL;
  ent->update = NULL;
      /*delete the infor first, then close the pointer*/
}

void ClearEntities()
{
  int i;
  for(i = 0;i < MAXENTITIES;i++)
    {
      FreeEntity(&__entityList[i]);
    }
}


int OnScreen(Entity *self)
{
  if(((self->s.x + self->size.x) >= Camera.x)&&(self->s.x <= (Camera.x + Camera.w))&&((self->s.y + self->size.y) >= Camera.y)&&(self->s.y <= (Camera.y + Camera.h)))
    return 1;
  return 0;
}
int CollisionDetect(Entity *owner, Entity *target)
{

	int left, left2;
	int right, right2;
	int top, top2;
	int bottom, bottom2;

	left = owner->Boundingbox.x;
	right = owner->Boundingbox.x + owner->Boundingbox.w;
	top = owner->Boundingbox.y;
	bottom = owner->Boundingbox.y + owner->Boundingbox.h;

	left2 = target->Boundingbox.x;
	right2 = target->Boundingbox.x + target->Boundingbox.w;
	top2 = target->Boundingbox.y;
	bottom2 = target->Boundingbox.y + target->Boundingbox.h;

	if((right >= left2) && (left <= right2) && (bottom >= top2) && (top <= bottom2))
	{
		printf("Bitch you guessed it");
		return 1;
	}
	return 0;

}

void DamageTarget(Entity *attacker,Entity *inflictor,Entity *defender,int damage)
{     /*inflictor is the entity that is physically doing the damage, attacker is the entity that initiated it.*/
      /*Exampe: the player is the attacker, but the shotgun shell is the inflictor*/
  if(defender == NULL)return;   /*you never know*/
  if(!defender->takedamage)return;/*lets not hurt anything that can't be hurt*/

  if(damage <= 0)damage = 1;/*you will at LEAST take 1 damage*/
  defender->health -= damage;
  if(defender->EntClass == EC_AI)defender->state = ST_ALERT;
  if(defender->health <= 0)
  {
    defender->state = ST_DEAD;
    defender->takedamage = 0;
    if(defender->EntClass == EC_AI)
    {
      attacker->killcount++;
    }
  }
  
  /*adding Id style obituary code here*/
}


/*returns the relative diference between two entities' positions.*/
int DistanceBetween(Entity *self, Entity *target)
{
  int difx,dify;
  difx = (int)abs((int)self->s.x - (int)target->s.x);
  dify = (int)abs((int)self->s.y - (int)target->s.y);
  return (difx + dify)>>1;
}


Entity *HitNextEnt(Entity *self,Entity *target)
{
  int i;
  SDL_Rect b1,b2;
  b1.x = self->s.x + self->Boundingbox.x;
  b1.y = self->s.y + self->Boundingbox.y;
  b1.w = self->Boundingbox.w;
  b1.h = self->Boundingbox.h;
  for(i = 0;i < MAXENTITIES; i++)
  {
    if(target == &__entityList[i])break;
  }
  for(i = i;i < MAXENTITIES; i++)
  {
    if(__entityList[i].inuse)
    {
      if((__entityList[i].health > 0)&&(__entityList[i].enemy != 0)&&(__entityList[i].enemy != self->enemy))
      {
        b2.x = __entityList[i].s.x + __entityList[i].Boundingbox.x;
        b2.y = __entityList[i].s.y + __entityList[i].Boundingbox.y;
        b2.w = __entityList[i].Boundingbox.w;
        b2.h = __entityList[i].Boundingbox.h;
        if(CollisionDetect(self,target))
         return &__entityList[i];
      }
    }
  }
  return NULL;
}