#ifndef __LEVEL__
#define __LEVEL__


#include "SDL.h"
#include "ltypes.h"

#define TILERES       12
#define LEVELLENGTH   128
#define SPACE_W     128
#define SPACE_H     27
#define MAX_OBJ     128
#define MAX_ENT     128
#define NELEMS(x) (sizeof(x) / sizeof(x[0]))
#define REMAIN(x) (sizeof(x) - (sizeof(NELEMS(x))))



typedef struct INFO_TAG
{
	char   name[40];      /*the name of the tag, to keep index hazards away*/
	Uint16 sx,sy;         /*coordinates of spawn point or info tag*/
	Uint16 UnitInfo;      /*used by some spawn types.*/
	Uint16 UnitType;      /*faction affiliation*/
	char   info[40];      /*info tags will need more info*/
}Tag;



typedef struct LEVEL_STRUCT
{
	Uint8   tilemap[SPACE_H][SPACE_W];
	Uint16  width,height;
	Tag  infolist[MAX_OBJ];
	int infocount;
	Tag  spawnlist[MAX_ENT];
	int spawncount;
	Uint8 tileset;
	char levelname[40];
	char bgimage[40];
	char bgmusic[40];
	arUI32  inuse;           /*makes sure there is something here before loading*/
	char name[80];        /*name of the map file without the path, but with extension.*/
	char tilesetar[80];/*path to file with list of tiles*/
	arUI32  *tilelist;       /*tiles in the map*/
	arUI32  numtiles;        /*number of tiles in the map*/
}Level;

void DrawLevel();
void LoadLevelSprites();  //this just loads the appropriate textures used to render the map.
void RenderLevel(Level *level);  //This will use the camera position to determine what to render.
int GetCurrentLevel(int level); //right now this doesn't do much
void GenerateLevel(int lvl);
void UpdateLevel(Level *level);
int MoveCamera(int Length);  // only moves in that -> direction

#endif