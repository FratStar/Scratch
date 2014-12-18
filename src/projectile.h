#ifndef __PROJECTILE__
#define __PROJECTILE__
#include "entity.h"

enum Bullet_Size {B_Tiny,B_Small,B_Medium,B_Large,B_Huge,B_Atomic};
void PrecacheProjSounds();

Entity *SpawnBullet(Entity *owner,int sx,int sy,float speed,int damage,int UType);
Entity *SpawnRocket(Entity *owner,int sx,int sy,float speed,float accel,int damage,int UType);
Entity *GetTouchingEnt(Entity *self);

#endif