#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Level.h"
#include "entity.h"
#include "audio.h"

extern ScreenData  S_Data;
extern arRect Camera;
extern arRect Abs_Camera;
extern Uint32 rmask,gmask,bmask,amask;
extern SDL_Surface *bgimage;
extern SDL_Surface *background;
Sprite *LevelSprite;
Mix_Music *BgMusic = NULL;
extern arSurface *buffer;

arSurface *clipmask;
Level level;

int NumTiles = 0;
int NumLevels = 2;
int CurrentLevel = 0;

void CloseLevel()
{
	/* if(BgMusic != NULL)
	{
	Mix_FreeMusic(BgMusic);
	}*/
	if(bgimage != NULL)
	{
		SDL_FreeSurface(bgimage);
	}
	if(clipmask != NULL)
	{
		SDL_FreeSurface(clipmask);
	}
}

void GenerateLevel(int lvl)
{	
	int i,j,k,l;
	FILE *file;
	char buf[512];
	char *filename = NULL;
	level.tileset = 1;
	level.infocount = 0;
	level.spawncount = 0;
	strcpy(level.levelname,"Level" + lvl);
	strcpy(level.bgimage,"images/bgimages/bgimage1.png");
	
	if (lvl == 1)
	{
		filename = "level/Level1.def";
	}
	else if (lvl == 2)
	{
		filename = "level/Level2.def";
	}

	file = fopen(filename,"r");
	if(file == NULL)
	{
		fprintf(stderr,"Unable to open map object definition file");
		return;
	}
	memset(level.tilemap,0,sizeof(level.tilemap));
	NumTiles = 0;

	i = 0;
	j = 0;
	k = 0;
	l = 0;

	while(fscanf(file, "%s", buf) != EOF)
	{
		if(strcmp(buf, "#") == 0)
		{
			fgets(buf, sizeof(buf), file);
			continue;
		}
		else if(strcmp(buf,"<level>") == 0)
		{
			fgets(buf,sizeof(buf), file);
			continue;
		}

		
		else if(buf[0] == '1')
		{
			for(l = 0; l < NELEMS(buf); l++)
			{
				if(buf[l] == '1')
				{
					level.tilemap[j][i] = 1;
					NumTiles++;
					i++;
				}
				else if(buf[l] == '0')
				{
					i++;
				}
				else if(buf[l] == 'p')
				{
					strcpy(level.spawnlist[k].name, "player_start");
					level.spawnlist[k].sx = i*40;
					level.spawnlist[k].sy = j*40;
					level.spawnlist[k].UnitType = 0;
					level.spawncount++;
					k++;
					i++;
				}
				else if(buf[l] == 'H')
				{
					strcpy(level.spawnlist[k].name, "Heavy");
					level.spawnlist[k].sx = i*40;
					level.spawnlist[k].sy = j*40;
					level.spawnlist[k].UnitType = 1;
					level.spawncount++;
					k++;
					i++;
				}
				else if(buf[l] == 'M')
				{
					strcpy(level.spawnlist[k].name, "Minor");
					level.spawnlist[k].sx = i*40;
					level.spawnlist[k].sy = j*40;
					level.spawnlist[k].UnitType = 2;
					level.spawncount++;
					k++;
					i++;
				}
				else if(buf[l] == ';')
				{
					j++;
					break;
				}
				level.width = i;
			}

				i = 0;
				fgets(buf, sizeof(buf), file);
				level.height = j;
		}
		
		/*  for(j = 0;j < y;j++)
		{
		level.tilemap[j][0] = 1;
		level.tilemap[j][x - 1] = 1;    
		}
		for(i = 0;i < x;i++)
		{
		level.tilemap[0][i] = 1;
		level.tilemap[y - 1][i] = 1;    
		}*/
	}
	fclose(file);
	
}

int GetCurrentLevel(int level)
{
	return level;
}


/*load the graphics and draw the graphics to the background buffer*/
void DrawLevel()
{
	Sprite *tiles = NULL;
	SDL_Surface *temp;
	int i,j;
	/*first we load the sprite sheet for the background tiles*/
	switch(level.tileset)
	{
	case 1:
	default:
		tiles = LoadSprite("images/tiles/dirt.png",40,40);
		SDL_SetColorKey(tiles->image, SDL_SRCCOLORKEY , SDL_MapRGBA(tiles->image->format, 255,255,255,255));
	}
	if(tiles == NULL)
	{
		fprintf(stderr,"Unable to load tile set for map: %s \n",SDL_GetError());
		exit(0);
	}
	/*if(BgMusic != NULL)
	{
	Mix_HaltMusic();
	Mix_FreeMusic(BgMusic);
	}
	/*then we load the background music*/
	/*BgMusic = Mix_LoadMUS(level.bgmusic);
	if(BgMusic == NULL)
	{
	fprintf(stderr,"Unable to load music: %s \n",SDL_GetError());
	exit(0);
	}*/

	/*And we load the background image*/
	if(bgimage != NULL)SDL_FreeSurface(bgimage);
	temp = IMG_Load(level.bgimage);
	if(temp == NULL)
	{
		fprintf(stderr,"Unable to load background image: %s \n",SDL_GetError());
		exit(0);
	}
	bgimage = SDL_DisplayFormat(temp);
	SDL_SetColorKey(bgimage, SDL_RLEACCEL, 0);
	SDL_FreeSurface(temp);

	/*then we check to see if the background image is already in memory and if so, then we shall destroy it*/
	if(background != NULL)SDL_FreeSurface(background);
	temp = SDL_CreateRGBSurface(SDL_HWSURFACE, level.width * 40, level.height * 40, S_Data.depth,rmask, gmask,bmask,amask);
	if(temp == NULL)
	{
		fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
		exit(1);
	}
	/* Just to make sure that the surface we create is compatible with the screen*/
	background = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);

	if(clipmask != NULL)SDL_FreeSurface(clipmask);
	temp = SDL_CreateRGBSurface(SDL_HWSURFACE, level.width * 40, level.height * 40, S_Data.depth,rmask, gmask,bmask,amask);
	if(temp == NULL)
	{
		fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
		exit(1);
	}
	/* Just to make sure that the surface we create is compatible with the screen*/
	clipmask = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);
	// DrawFilledRect(0,0, background->w,background->h, SDL_MapRGB(background->format,0,0,0), background);
	for(j = 0;j < level.height;j++)
	{
		for(i = 0;i < level.width;i++)
		{
			DrawSprite(tiles,background,i * tiles->w,j * tiles->h,level.tilemap[j][i] - 1);
		}
	}
	SDL_BlitSurface(background,NULL,clipmask,NULL);
	SDL_SetColorKey(background,SDL_RLEACCEL, SDL_MapRGBA(background->format,0,0,0,255));
	FreeSprite(tiles);
	/*Mix_VolumeMusic(MIX_MAX_VOLUME>>3);
	Mix_PlayMusic(BgMusic, -1);*/
}