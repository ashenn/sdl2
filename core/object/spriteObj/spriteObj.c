#include "./spriteObj.h"
#include "../../animation/sprite/spriteAnim.h"

SpriteAnimData* addAnim(SpriteObject* obj, char* name, int fps, int clipCnt, int clipIndex, unsigned short row) {
	SpriteAnimData* anim = new(SpriteAnimData);

	anim->fps = fps;
	anim->clipCnt = clipCnt;
	obj->clip = &obj->curClip;
	anim->wait = (FPS / anim->fps);

	anim->duration = (anim->fps * anim->clipCnt);

	logger->dbg(LOG_SPRITE, "-- Init Clip Positions");
	anim->clipPos = malloc(sizeof(SDL_Rect) * anim->clipCnt);

	int cell_x = 50;
	int cell_y = 38;

    int i = clipIndex;
	logger->dbg(LOG_SPRITE, "-- LOOOP");

	unsigned short curRow = row;

	for (int a = 0; a < anim->clipCnt; ++a) {
		if (i >= obj->spriteColumns) {
			i = 0;
			curRow++;
		}

		logger->dbg(LOG_SPRITE, "-- A: %d", a);
		logger->dbg(LOG_SPRITE, "-- I: %d", i);

		logger->dbg(LOG_SPRITE, "--Get Pos:  %d", i);
		SDL_Rect* pos = &(anim->clipPos[a]);
		pos->w = cell_x;
		pos->h = cell_y;

		pos->x = i * cell_x;
		pos->y = curRow * cell_y;

		logger->dbg(LOG_SPRITE, "-- X: %d | Y: %d", pos->x, pos->y);

		if (i == 0) {
			obj->curClip = *pos;
		}

		i++;
	}


	logger->dbg(LOG_SPRITE, "-- Add Anim To List");
	Node* n = addNodeV(obj->animList, name, anim, 1);
	anim->animID = n->id;

	return anim;
}

SpriteObject* newSpriteObject(char* name, void* comp, SDL_Rect* pos, short z) {
	logger->inf(LOG_SPRITE, "==== ADDING SPRITE OBJECT: %s ====", name);
	SpriteObject* obj = new(SpriteObject);
	initSimpleObject((Object*)obj, name, comp, pos, z);

	obj->spriteRows = 16;
	obj->spriteColumns = 7;

	logger->dbg(LOG_SPRITE, "-- Init List");
	obj->animList = initListMgr();


	logger->dbg(LOG_SPRITE, "-- Init Anim Data");
	//SpriteAnimData* anim = new(SpriteAnimData);

	logger->dbg(LOG_SPRITE, "-- Adding To View");
	addObjectToView((Object*) obj);


	addAnim(obj, "Idle", 10, 4, 0, 0);
	addAnim(obj, "Run", 15, 6, 1, 1);
	addAnim(obj, "Down", 10, 4, 4, 0);

	spriteAnim(obj, 1, 0);

	//logger->dbg(LOG_SPRITE, "-- Anim ID: #%d", anim->id);

	obj->pos.h = obj->curClip.h;
	obj->pos.w = obj->curClip.w;

	logger->dbg(LOG_SPRITE, "===== Sprite Object Ready =====");
	return obj;
}
