#include "spriteAnim.h"

SpriteAnimParam* spriteAnim(SpriteObject* obj, unsigned int animID, unsigned int clipIndex) {
	if (obj == NULL) {
		logger->war(LOG_SPRITE, "Trying To Animate NULL Object !!!");
		return NULL;
	}

	logger->inf(LOG_SPRITE, "==== Sprite Animate: %s ====", obj->name);
	if (obj->animList == NULL || !obj->animList->nodeCount) {
		logger->war(LOG_SPRITE, "No Animation Associated With Object: %s !!!", obj->name);
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

	SpriteAnimParam* animParam = new(SpriteAnimParam);

	animParam->animID = animID;
	animParam->clipMax = anim->clipCnt;
	animParam->spriteData = anim;
	animParam->clipIndex = clipIndex;

	animParam->clip = anim->clipPos[clipIndex];

	char animName[150];
	memset(animName, 0, 150);
	snprintf(animName, 150, "%s-%d", obj->name, animID);

	Animator* animator = getAnimator();
	n = addNodeV(animator->sprites, animName, animParam, 1);

	return animParam;
}
