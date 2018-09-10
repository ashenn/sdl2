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
	Object* obj;	\
	\
	bool loop;	 \
	float delay;	 \
	bool breakAnim;	 \
	bool deleteObject;	\
	\
	int frames; \
	int initialFrames; \
	\
	void (*fnc)();	\
	void (*stepFnc)(AnimParam* param);	\
	void (*callback)(AnimParam* param);


#define ANIM_MOVE_PARAM_BODY \
	ANIM_PARAM_BODY	\
	float duration;	 \
	\
	SDL_Rect originPos;	\
	SDL_Rect targetPos;	\
	\
	AnimDistance move[2];


struct AnimParam {
	ANIM_PARAM_BODY
};

typedef struct AnimMoveParam {
	ANIM_MOVE_PARAM_BODY
} AnimMoveParam;


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
