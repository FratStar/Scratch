#ifndef __ENTITY__
#define __ENTITY__

#include "graphics.h"
#include "audio.h"
#include "sprite.h"
#include "ltypes.h"
#include <SDL.h>

#define SOUNDSPERENT 4

enum ENTTYPES   {ET_Temp,ET_WorldBullet,ET_WorldEnt,ET_PlayerBullet,ET_Player};
enum STATES {ST_IDLE,ST_FIRE,ST_HYPER,ST_DYING,ST_DEAD, ST_ALERT};
enum ENTCLASS{EC_NULL,EC_AI,EC_PLAYER,EC_BULLET};
enum SOUNDTYPE  {SF_ALERT,SF_IMPACT,SF_PAIN,SF_DYING}; 

typedef struct ENTITY_T
{
	Sprite *sprite;
	Sound *__sound[SOUNDSPERENT];
	int Player;
	int EntClass;
	int Unit_Type;
	char *EntName;
	struct ENTITY_T *owner;
	struct ENTITY_T *target;
	void (*think) (struct ENTITY_T *self);
	void (*update) (struct ENTITY_T *self);
	arUI32 NextThink;
	arUI32 NextUpdate;
	arUI16 ThinkRate;
	arUI16 UpdateRate;
	int grounded;
	int shown;
	int frame;
	int fcount;
	int frate;
	int inuse;
	arUI32 framedelay;
	int enemy;
	int face;
	int state;
	arRect Boundingbox;
	arRect Clipbox;
	int movespeed;
	int maxspeed;
	Coord s;
	Coord a;
	Coord v;
	Point m;
	Point size;
	Point origin;
	int killcount;
	int numlives;
	int health,healthmax;
	int lifespan;
	int takedamage;
	int currentweapon;
	int cooldown;
	int accel;
	int damage;
}Entity;

/*startup and clean up*/
void InitEntityList();
void ClearEntities();
void ClearEntitiesExcept(Entity *skip);

/*creation and destruction*/
Entity *NewEntity();
void FreeEntity(Entity *ent);

/*update functions*/
void DrawEntity(Entity *ent);
void DrawEntities();
void UpdateEntities();
void ThinkEntities();


void DrawBBoxEntities();/*draw only the bounding box of the entity*/

/*generic entity support functions*/
int OnScreen(Entity *self);
void DamageTarget(Entity *attacker,Entity *inflictor,Entity *defender,int damage);
int MoveUnit(Entity *self);
int Collide(arRect box1,arRect box2);/*returns 1 if the two rectangles are clipping*/
int CollisionDetect(Entity *owner, Entity *target);

int WhatFace(Entity *self);/*return the enumerated value of the direction the entity is facing*/
void GetFace(Entity *self);
void Get16Face(Entity *self);

int UpdateEntityPosition(Entity *self,int bounce);
int DistanceBetween(Entity *self, Entity *target);/*returns the diference between two entities.*/
int AimAtTarget(Entity *self,Entity *target);

#endif
