#include "spriteAnim.h"


ListManager* getAnimLinkFncs() {
	static ListManager* linkFncs = NULL;

	if (linkFncs != NULL) {
		return linkFncs;
	}

	linkFncs = initListMgr();

	return linkFncs;
}

void addAnimLinkFncs(char* name, bool (*fnc)(SpriteAnimParam*)) {
	logger->inf(LOG_SPRITE, "=== Add Anim Link Func %s ===", name);

	ListManager* fncs = getAnimLinkFncs();
	AnimLinkFnc* linkFnc = malloc(sizeof(AnimLinkFnc));

	linkFnc->fnc = fnc;
	addNodeV(fncs, name, linkFnc, 1);
}

AnimLinkFnc* getAnimLinkFnc(char* name) {
	logger->inf(LOG_SPRITE, "=== Get Anim Link Func %s ===", name);
	if (name == NULL) {
		return NULL;
	}

	ListManager* fncs = getAnimLinkFncs();
	logger->inf(LOG_SPRITE, "-- List Ready: %p", fncs);


	logger->inf(LOG_SPRITE, "-- Searching: %s", name);
	Node* n = getNodeByName(fncs, name);

	if (n == NULL) {
		logger->war(LOG_SPRITE, "Fail To Find Anim Link Func %s !!!", name);
		return NULL;
	}

	logger->inf(LOG_SPRITE, "-- Node Found");
	return (AnimLinkFnc*) n->value;
}

void applySpriteAnim(AnimParam* animParam) {
	//logger->inf(LOG_SPRITE, "-- OBJECT: %s", animParam->obj->name);
	SpriteAnimParam* param = (SpriteAnimParam*) animParam;

	if (param->clipIndex != param->clipMax) {
		param->done = false;
	}

	param->wait--;
	if (param->wait > 0) {
		//logger->inf(LOG_SPRITE, "TEST WAITING: %d", param->wait);
		return;
	}

	param->wait = param->anim->wait;
	//logger->inf(LOG_SPRITE, "NEW WAITING: %d", param->anim->wait);
	if (++param->clipIndex >= param->clipMax) {
		//logger->inf(LOG_SPRITE, "-- Max Clip Reached: %s => %d", param->anim->name, param->clipMax);
		param->done = true;
		param->clipIndex = 0;
	}

	// logger->inf(LOG_SPRITE, "-- New Clip: %d", param->clipIndex);
	param->obj->clip = &(param->anim->clipPos[param->clipIndex]);
	// logger->inf(LOG_SPRITE, "-- X: %d", param->obj->clip->x);
}

bool checkObjAnim(SpriteObject* obj) {
	if (obj == NULL) {
		logger->war(LOG_SPRITE, "Trying To Animate NULL Object !!!");
		return false;
	}

	logger->inf(LOG_SPRITE, "==== Sprite Animate: %s ====", obj->name);

	if (obj->animList == NULL) {
		logger->war(LOG_SPRITE, "Animation List Is Null For Object: %s !!!", obj->name);
		return false;
	}
	else if(!obj->animList->nodeCount) {
		logger->war(LOG_SPRITE, "Animation List Is Empty For Object: %s !!!", obj->name);
		return false;
	}

	return true;
}

void SpriteAnimDataDelete(Node* n) {
	SpriteAnimData* anim = (SpriteAnimData*) n->value;
	logger->inf(LOG_SPRITE, "===== DELETING SPRITE ANIM DATA: %s =====", anim->name);

	free(anim->name);
	free(anim->clipPos);
}

void AnimLinkDelete(Node* n) {
	AnimLink* link = (AnimLink*) n->value;
	logger->inf(LOG_SPRITE, "===== DELETING SPRITE ANIM LINK: %s =====", link->name);

	free(link->name);
	free(link->target);
}

SpriteAnimParam* spriteCallAnim(SpriteObject* obj, SpriteAnimData* anim, unsigned int clipIndex) {
	logger->inf(LOG_SPRITE, "-- Calling Sprite Animation: #%d => %s", anim->animID, anim->name);
	logger->inf(LOG_SPRITE, "-- Removing Old Sprite Anim");
	spriteRemoveObject((Object*) obj);
	


	unsigned int animID = anim->animID;
	if (clipIndex >= anim->clipCnt) {
		logger->war(LOG_SPRITE, "Undefinded Clip Index #%d / %d for Object %s Animation: #%d !!!", clipIndex, anim->clipCnt, obj->name, animID);
		logger->war(LOG_SPRITE, "-- Clip Index has been set to 0", clipIndex, anim->clipCnt, obj->name, animID);
		clipIndex = 0;
	}

	logger->dbg(LOG_SPRITE, "-- New Anim Param");
	SpriteAnimParam* animParam = new(SpriteAnimParam);

	initAnimParam((AnimParam*) animParam, (Object*) obj, anim->duration, 0, applySpriteAnim);

	animParam->wait = 1;
	animParam->anim = anim;
	animParam->done = false;
	animParam->animID = animID;
	animParam->loop = anim->loop;
	animParam->breakAnim = false;
	animParam->clipIndex = clipIndex;
	animParam->clipMax = anim->clipCnt;

	animParam->fnc = applySpriteAnim;
	animParam->clip = anim->clipPos[clipIndex];

	logger->dbg(LOG_SPRITE, "-- Param: %s", obj->name);

	Animator* animator = getAnimator();
	logger->dbg(LOG_SPRITE, "-- Adding Anim To Animator: %s", anim->name);
	logger->dbg(LOG_SPRITE, "-- Loop: %d", animParam->loop);

	addNodeV(animator->sprites, obj->name, animParam, 1);


	return animParam;
}

SpriteAnimParam* spriteAnimByName(SpriteObject* obj, char* name, unsigned int clipIndex) {
	if (!checkObjAnim(obj)) {
		return NULL;
	}

	logger->inf(LOG_SPRITE, "-- Searching Anim: %s", name);
	Node* n = getNodeByName(obj->animList, name);

	if (n == NULL) {
		logger->war(LOG_SPRITE, "Fail To Find Animation %s for Object %s !!!", name, obj->name);
		return NULL;
	}

	logger->dbg(LOG_SPRITE, "-- Node Found: %s", n->name);

	if (n->value == NULL) {
		logger->war(LOG_SPRITE, "Animation with ID: %s is Not Initialized for Object %s !!!", name, obj->name);
		return NULL;
	}

	SpriteAnimData* anim = (SpriteAnimData*) n->value;

	logger->dbg(LOG_SPRITE, "-- Casted To Anim: #%d => %s", anim->animID, anim->name);
	return spriteCallAnim(obj, anim, clipIndex);
}

SpriteAnimParam* spriteAnim(SpriteObject* obj, unsigned int animID, unsigned int clipIndex) {
	if (!checkObjAnim(obj)) {
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

	assert(0);
	return spriteCallAnim(obj, anim, clipIndex);
}
