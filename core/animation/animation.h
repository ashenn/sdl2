#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL2/SDL.h>
#include "../../common.h"
#include "../../base/math.h"
#include "../object/object.h"

typedef struct AnimDistance {
	int rest;
	int perFrame;
} AnimDistance;


typedef struct AnimParam AnimParam;
#define ANIM_PARAM_BODY \
	CLASS_BODY	\
	\
	bool isInit;	 \
	\
	float delay;	 \
	float duration;	 \
	bool breakAnim;	 \
	int frames; \
	\
	SDL_Rect originPos;	\
	SDL_Rect targetPos;	\
	\
	Object* obj;	\
	bool deleteObject;	\
	\
	AnimDistance move[2];	\
	\
	void (*fnc)();	\
	void (*callBack)(AnimParam* param);


struct AnimParam {
	ANIM_PARAM_BODY
};


#define ANIMATOR_BODY \
	CLASS_BODY	\
	ListManager* anims;

typedef struct Animator {
	ANIMATOR_BODY
} Animator;


Animator* getAnimator();
AnimParam* moveTo(Object* obj, int x, int y, float time, float delay);

#endif