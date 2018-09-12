#ifndef SPRITE_ANIM_H
#define SPRITE_ANIM_H

#include "../animation.h"
#include "../../object/spriteObj/spriteObj.h"


typedef struct SpriteAnimData {
	CLASS_BODY
	unsigned int fps;
	unsigned int row;
	unsigned int wait;
	unsigned int animID;
	unsigned int duration;
	unsigned int startIndex;
	

	SDL_Rect* clipPos;
	unsigned int clipCnt;
} SpriteAnimData;

#define ANIM_SPRITE_PARAM_BODY \
	ANIM_PARAM_BODY	\
	\
	SDL_Rect clip;	\
	unsigned int wait;	\
	unsigned int animID; \
	unsigned short clipMax;	\
	unsigned short clipIndex;	\
	\
	SpriteAnimData* anim;


typedef struct SpriteAnimParam {
	ANIM_SPRITE_PARAM_BODY
} SpriteAnimParam;

SpriteAnimParam* spriteAnim(SpriteObject* obj, unsigned int animID, unsigned int clipIndex);

#endif
