#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "weapons.h"
//#include "particle.h"
#include "projectile.h"
                        /*2Pi*/
#define Sixteenth       0.392699081698724155
#define Twentyfifth     0.251327412287183459
#define Fiftieth        0.125663706143591729
#define Hundredth       0.062831853071795864
#define TwoHundredth    0.031415926535897932
#define ThreeHundredth  0.020943951023931955
#define FourHundredth   0.015707963267948966

Sound *WeaponSounds[NumWeapons * 2];

void FireBasic(Entity *self, Weapon *weapon);
void SuperBasic(Entity *self,Weapon *weapon);
void FireRocket(Entity *self,Weapon *weapon);


Weapon WeaponList[] =
{
	{
		"Machine Gun",	/*weapon name*/
		0,				/*cooldown between shots*/
		0,				/*charge cooldown*/
		25,				/*damage dealt */
		18,                 /*speed*/
		0.15,                /*how much variation can be expeted between each bullet.*/
		3,                  /*volume of attack*/
		0,                  /*if the projectiles produced will be affected by gravity*/
		FireBasic,		/*function to fire weapon*/
		"sounds/machineg.wav",
		"\0"
	},
	{
		"Rocket",
		5,
		0,
		50,
		12,
		0,
		1,
		0,
		FireRocket,
		"sounds/rocketfly.wav",
		"\0"
	}

};

void FireWeapon(Entity *self)
{
  if((self->currentweapon >= 0)&&(self->currentweapon < NumWeapons))
  {
    if(WeaponList[self->currentweapon].fireweapon != NULL)
    {
      WeaponList[self->currentweapon].fireweapon(self,&WeaponList[self->currentweapon]);
      self->cooldown = WeaponList[self->currentweapon].cooldown;
      if(WeaponSounds[self->currentweapon *2] != NULL)
      {
        Mix_PlayChannel(Mix_GroupAvailable(FX_Bullets),WeaponSounds[self->currentweapon *2]->sound,0);
      }
    }
  }
} 

void FireRocket(Entity *self,Weapon *weapon)
{
  int sx,sy;
  sx = (int)self->s.x + self->origin.x;
  sy = (int)self->s.y + self->origin.y;
  SpawnRocket(self,sx,sy,weapon->speed,1,weapon->damage,self->Unit_Type);
}
void FireBasic(Entity *self,Weapon *weapon)
{
  int sx,sy;
   sx = (int)self->s.x + self->origin.x;
   sy = (int)self->s.y + self->origin.y;
  SpawnBullet(self,sx,sy,weapon->speed,weapon->damage,self->Unit_Type);
}


void PrecacheWeaponSounds()                    /*load sounds that the weapons use*/
{
  int i;
  for(i = 0;i < NumWeapons;i++)
  {
    if(WeaponList[i].firesound[0] != '\0')
    {
      WeaponSounds[i*2] = LoadSound(WeaponList[i].firesound,MIX_MAX_VOLUME);
    }  
    else WeaponSounds[i*2] = NULL;
    if(WeaponList[i].ultimatesound[0] != '\0')
    {
      WeaponSounds[i*2 + 1] = LoadSound(WeaponList[i].ultimatesound,MIX_MAX_VOLUME);
    }
    else WeaponSounds[i*2 + 1] = NULL;
  }
}
