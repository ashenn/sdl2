#ifndef SPRITE_ANIM_H
#define SPRITE_ANIM_H

#include "../animation.h"
#include "../../../base/libList.h"
#include "../../object/spriteObj/spriteObj.h"


typedef struct SpriteAnimData {
	CLASS_BODY
	bool loop;
	unsigned int fps;
	unsigned int row;
	unsigned int wait;
	unsigned int animID;
	unsigned int duration;
	unsigned int startIndex;
	

	SDL_Rect* clipPos;
	unsigned int clipCnt;

	ListManager* animLinks;
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

typedef struct AnimLink
{
	char* name;
	bool (*canLink)(SpriteObject*);
} AnimLink;

SpriteAnimParam* spriteAnimByName(SpriteObject* obj, char* name, unsigned int clipIndex);
SpriteAnimParam* spriteAnim(SpriteObject* obj, unsigned int animID, unsigned int clipIndex);

#endif
