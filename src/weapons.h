#ifndef __WEAPONS__
#define __WEAPONS__

#include "entity.h"

/*
    Weapons in this game will be a separate data structure that entities will access through an index.
*/

#define NumWeapons  2


typedef struct WEAPON_T
{
  char *Name;        /*weapon's name*/
  int cooldown;         /*how long it will take to cool down between shots*/
  int Ucooldown;        /*Ultimate cooldown*/
  int damage;           /*how much damage each projectile will deal*/
  float kick;             /*how much kick the weapon carries*/
  int speed;
  float spread;         /*how much variation can be expeted between each bullet.*/
  int volume;           /*how much is produced at once*/
  int gravity;          /*if the projectiles produced will be affected by gravity*/
  int bounce;
  int size;             /*some weapons have different sized projectiles*/
  void (*fireweapon)   (Entity *self,struct WEAPON_T *weapon); /*call this when you fire the weapon*/
  char firesound[40];   /*paths to the sound files that the weapons use*/
  char ultimatesound[40];
}Weapon;

void PrecacheWeaponSounds();                    /*load sounds that the weapons use*/
void FireWeapon(Entity *self);    /*fires the entity's current equipped weapon in the desired direction*/
int GetWeaponByName(char name[40]);             /*return the index of the weapon in the list by the weapon's name*/
char *PrintWeaponName(int index);

#endif
