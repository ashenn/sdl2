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
	\
	bool breakAnim;	 \
	int frames; \
	int initialFrames; \
	\
	SDL_Rect originPos;	\
	SDL_Rect targetPos;	\
	\
	Object* obj;	\
	\
	bool loop;	 \
	bool deleteObject;	\
	\
	AnimDistance move[2];	\
	\
	void (*fnc)();	\
	void (*stepFnc)(AnimParam* param);	\
	void (*callback)(AnimParam* param);


struct AnimParam {
	ANIM_PARAM_BODY
};


#define ANIMATOR_BODY \
	CLASS_BODY	\
	ListManager* moves; \
	ListManager* sprites;

typedef struct Animator {
	ANIMATOR_BODY
} Animator;


void animate();
Animator* getAnimator();
void animRemoveObject(Object* obj);
AnimParam* moveTo(Object* obj, int x, int y, float time, float delay);
#endif
