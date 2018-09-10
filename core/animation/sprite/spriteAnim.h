#ifndef SPRITE_ANIM_H
#define SPRITE_ANIM_H

#include "../animation.h"
#include "../../object/spriteObj/spriteObj.h"


typedef struct SpriteAnimData {
	unsigned int fps;
	unsigned int animID;
	unsigned int clipCnt;
	SDL_Rect* clipPos;
} SpriteAnimData;

#define ANIM_SPRITE_PARAM_BODY \
	ANIM_PARAM_BODY	\
	\
	SDL_Rect clip;	\
	unsigned int animID; \
	unsigned short clipMax;	\
	unsigned short clipIndex;	\
	\
	SpriteAnimData* spriteData;


typedef struct SpriteAnimParam {
	ANIM_SPRITE_PARAM_BODY
} SpriteAnimParam;

SpriteAnimParam* spriteAnim(SpriteObject* obj, unsigned int animID, unsigned int clipIndex);

#endif
