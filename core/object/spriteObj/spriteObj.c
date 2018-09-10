#include "./spriteObj.h"
#include "../../animation/sprite/spriteAnim.h"


SpriteObject* newSpriteObject(char* name, void* comp, SDL_Rect* pos, short z) {
	logger->inf(LOG_SPRITE, "==== ADDING SPRITE OBJECT: %s ====", name);
	SpriteObject* obj = new(SpriteObject);
	initSimpleObject((Object*)obj, name, comp, pos, z);

	logger->dbg(LOG_SPRITE, "-- Init List");
	obj->animList = initListMgr();


	logger->dbg(LOG_SPRITE, "-- Init Anim Data");
	SpriteAnimData* anim = new(SpriteAnimData);

	logger->dbg(LOG_SPRITE, "-- Adding To View");
	addObjectToView((Object*) obj);
	return obj;
	

	logger->dbg(LOG_SPRITE, "-- Add Anim To List");
	Node* n = addNodeV(obj->animList, "Idle", anim, 1);

	//obj->clip = &obj->curClip;
	anim->fps = 24;
	anim->animID = n->id;

	anim->clipCnt = 7;

	logger->dbg(LOG_SPRITE, "-- Init Clip Positions");
	anim->clipPos = malloc(sizeof(SDL_Rect) * anim->clipCnt);

	int cell_x = 385;
	int cell_y = 370;

	for (int i = 0; i < anim->clipCnt; ++i) {
		logger->dbg(LOG_SPRITE, "-- Clip: #%d", i);
		SDL_Rect* pos = &(anim->clipPos[i]);
		pos->w = cell_x;
		pos->h = cell_y;

		pos->x = i * cell_x;
		pos->y = i * cell_y;

		logger->dbg(LOG_SPRITE, "-- X: %d | Y: %d", pos->x, pos->y);
		logger->dbg(LOG_SPRITE, "-- W: %d | H: %d", pos->w, pos->h);

		if (i == 0) {
			logger->dbg(LOG_SPRITE, "-- Setting As Default");
			obj->curClip = *pos;
		}
	}
	
	logger->dbg(LOG_SPRITE, "===== Sprite Object Ready =====");
	return obj;
}
