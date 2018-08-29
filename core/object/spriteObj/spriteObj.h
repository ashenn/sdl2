#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H

#include "../collisionObj/collisionObj.h"

#define SPRITE_OBJ_BODY	\
	COLLISION_OBJ_BODY	\
	SDL_Rect clip; \
	short clipIndex; \
	ListManager* clipList;


typedef struct SpriteObject {
	SPRITE_OBJ_BODY
} SpriteObject;

#endif
