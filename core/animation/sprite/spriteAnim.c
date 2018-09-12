#include "spriteAnim.h"

void applySpriteAnim(AnimParam* animParam) {
	//logger->inf(LOG_SPRITE, "-- OBJECT: %s", animParam->obj->name);
	SpriteAnimParam* param = (SpriteAnimParam*) animParam;

	param->wait--;
	if (param->wait > 0) {
		//logger->inf(LOG_SPRITE, "TEST WAITING: %d", param->wait);
		return;
	}

	param->wait = param->anim->wait;
	//logger->inf(LOG_SPRITE, "NEW WAITING: %d", param->anim->wait);
	if (++param->clipIndex >= param->clipMax) {
		//logger->inf(LOG_SPRITE, "-- Max Clip Reached: %d", param->clipMax);
		param->clipIndex = 0;
	}

	logger->inf(LOG_SPRITE, "-- New Clip: %d", param->clipIndex);
	param->obj->clip = &(param->anim->clipPos[param->clipIndex]);
	logger->inf(LOG_SPRITE, "-- X: %d", param->obj->clip->x);
}

SpriteAnimParam* spriteAnim(SpriteObject* obj, unsigned int animID, unsigned int clipIndex) {
	if (obj == NULL) {
		logger->war(LOG_SPRITE, "Trying To Animate NULL Object !!!");
		return NULL;
	}

	logger->inf(LOG_SPRITE, "==== Sprite Animate: %s ====", obj->name);

	logger->inf(LOG_SPRITE, "-- Removing Old Sprite Anim");
	spriteRemoveObject((Object*) obj);

	if (obj->animList == NULL) {
		logger->war(LOG_SPRITE, "Animation List Is Null For Object: %s !!!", obj->name);
		return NULL;
	}
	else if(!obj->animList->nodeCount) {
		logger->war(LOG_SPRITE, "Animation List Is Empty For Object: %s !!!", obj->name);
		return NULL;
	}

	logger->dbg(LOG_SPRITE, "-- Searching Anim: #%d", animID);

	Node* n = getNode(obj->animList, animID);
	if (n == NULL) {
		logger->war(LOG_SPRITE, "Fail To Find Animation ID: #%d for Object %s !!!", animID, obj->name);
		return NULL;
	}

	SpriteAnimData* anim = (SpriteAnimData*) n->value;
	if (anim == NULL) {
		logger->war(LOG_SPRITE, "Animation with ID: #%d is Not Initialized for Object %s !!!", animID, obj->name);
		return NULL;
	}

	if (clipIndex >= anim->clipCnt) {
		logger->war(LOG_SPRITE, "Undefinded Clip Index #%d / %d for Object %s Animation: #%d !!!", clipIndex, anim->clipCnt, obj->name, animID);
		logger->war(LOG_SPRITE, "-- Clip Index has been set to 0", clipIndex, anim->clipCnt, obj->name, animID);
		clipIndex = 0;
	}

	logger->dbg(LOG_SPRITE, "-- New Anim Param");
	SpriteAnimParam* animParam = new(SpriteAnimParam);

	initAnimParam((AnimParam*) animParam, (Object*) obj, anim->duration, 0, applySpriteAnim);

	animParam->loop = true;
	animParam->anim = anim;
	animParam->animID = animID;
	animParam->wait = 1;
	animParam->breakAnim = false;
	animParam->clipIndex = clipIndex;
	animParam->clipMax = anim->clipCnt;

	animParam->fnc = applySpriteAnim;
	animParam->clip = anim->clipPos[clipIndex];

	logger->dbg(LOG_SPRITE, "-- Param: %s", obj->name);

	Animator* animator = getAnimator();
	n = addNodeV(animator->sprites, obj->name, animParam, 1);

	return animParam;
}
