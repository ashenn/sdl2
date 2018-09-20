#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H

#include "../object.h"
#include "../collisionObj/collisionObj.h"

#define SPRITE_OBJ_BODY	\
	COLLISION_OBJ_BODY	\
	SDL_Rect curClip; \
	short clipIndex; \
	\
	ListManager* animList;	\
	\
	unsigned short cell_y; \
	unsigned short cell_x;	\
	\
	unsigned short spriteRows;	\
	unsigned short spriteColumns;\


typedef struct SpriteObject {
	SPRITE_OBJ_BODY
} SpriteObject;

SpriteObject* newSpriteObject(char* name, char* jsonPath, SDL_Rect* pos, short z);
SpriteObject* initSpriteObj(SpriteObject* obj, char* name, char* path, SDL_Rect* pos, short z);

void footLand(Object* self, Object* target);
void footFall(Object* self, Object* target);

#endif
