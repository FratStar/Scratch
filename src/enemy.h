#ifndef __ENEMIES__
#define __ENEMIES__
#include "entity.h"

#define ENEMYMAX 128

typedef struct
{
	char	*name;
	char	*sp;
	Sprite	*sprite;
	int		sx,sy;
	float	radius;
	int		powerupchange;
	Entity	*powerup;
	int		health;
	float	droprate;
	float	firespeed;
	int		damage;
	float	movespeed;


}enemy;

void SpawnGenericEnemy(int x, int y); /*will add some new shit later */
void SpawnHeavy(Entity *owner, int x, int y, int weapon, int group);
void SpawnMinor(Entity *owner, int x, int y, int weapon, int group);
void LoadEnemyList();
enemy GetEnemyByName(Entity *self, Entity *other); /* not used*/
void UpdateEnemy(Entity *self);;
void EnemyAlert(Entity *self, Entity*other);
void EnemyPath(Entity *self, Entity *target);



#endif
