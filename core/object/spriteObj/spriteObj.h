#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H

#include "../object.h"
#include "../collisionObj/collisionObj.h"

#define SPRITE_OBJ_BODY	\
	COLLISION_OBJ_BODY	\
	SDL_Rect curClip; \
	short clipIndex; \
	\
	char* spritePath;	\
	ListManager* animList;	\
	\
	unsigned short spriteRows;	\
	unsigned short spriteColumns;


typedef struct SpriteObject {
	SPRITE_OBJ_BODY
} SpriteObject;


SpriteObject* newSpriteObject(char* name, void* comp, SDL_Rect* pos, short z);

#endif
