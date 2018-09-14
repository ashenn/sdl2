#include "./spriteObj.h"
#include "../../../base/json.h"
#include "../../asset/asset.h"
#include "../../animation/sprite/spriteAnim.h"

SpriteAnimData* addAnim(SpriteObject* obj, char* name, int fps, int clipCnt, int clipIndex, unsigned short row) {
	SpriteAnimData* anim = new(SpriteAnimData);

	anim->fps = fps;
	anim->clipCnt = clipCnt;
	obj->clip = &obj->curClip;
	anim->wait = (FPS / anim->fps);

	anim->duration = (anim->fps * anim->clipCnt);
	anim->clipPos = malloc(sizeof(SDL_Rect) * anim->clipCnt);

	int cell_x = 50;
	int cell_y = 38;

    int i = clipIndex;

	unsigned short curRow = row;

	for (int a = 0; a < anim->clipCnt; ++a) {
		if (i >= obj->spriteColumns) {
			i = 0;
			curRow++;
		}

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

	Node* n = addNodeV(obj->animList, name, anim, 1);
	anim->animID = n->id;

	return anim;
}


short initAnims(int i, Node* n, short* delete, void* param, va_list* args) {
	logger->err(LOG_SPRITE, "============ #%d==========", i);
	if (param == NULL) {
		logger->err(LOG_SPRITE, "-- Trying To Add Anim For NULL Object !!!");
		return false;
	}

	SpriteObject* obj = param;
	if (n == NULL) {
		logger->war(LOG_SPRITE, "-- Skipping NULL Anim #%d For Object: %s !!!", i, obj->name);
		return true;
	}

	Json* json = (Json*) n->value;
	if (json->type == JSON_NULL) {
		logger->war(LOG_SPRITE, "-- Skipping NULL AnimData #%d For Object: %s !!!", i, obj->name);
		return true;
	}

	float data[6];
	data[6] = 0;
	char* name = jsonGetValue(json, "name", NULL);

	jsonGetValue(json, "fps", &(data[0]));
	jsonGetValue(json, "count", &(data[1]));
	jsonGetValue(json, "index", &(data[2]));
	jsonGetValue(json, "row", &(data[3]));

	logger->dbg(LOG_SPRITE, "Adding Anim #%d => %s", i, name);
	logger->dbg(LOG_SPRITE, "-- fps: %f", data[0]);
	logger->dbg(LOG_SPRITE, "-- count: %f", data[1]);
	logger->dbg(LOG_SPRITE, "-- index: %f", data[2]);
	logger->dbg(LOG_SPRITE, "-- row: %f", data[3]);

	SpriteAnimData* anim = addAnim(obj, name, (int) data[0], (int) data[1], (int) data[2], (unsigned short) data[3]);
	anim->name = name;

	jsonGetValue(json, "loop", &(data[5]));
	anim->loop = (bool) data[5];
	logger->dbg(LOG_SPRITE, "-- Loop: %d", anim->loop);

	Json* links = jsonGetData(json, "links");

	if (links != NULL && links->childCount) {
		logger->dbg(LOG_SPRITE, "-- Has Links: %d !!!", links->childCount);
		anim->animLinks = initListMgr();
		jsonIterate(json, NULL, NULL);
	}
	else {
		anim->animLinks = NULL;
	}


    return true;
}

SpriteObject* newSpriteObject(char* name, void* comp, SDL_Rect* pos, short z) {
	logger->inf(LOG_SPRITE, "==== ADDING SPRITE OBJECT: %s ====", name);
	SpriteObject* obj = new(SpriteObject);
	initSimpleObject((Object*)obj, name, comp, pos, z);

	obj->spriteRows = 16;
	obj->spriteColumns = 7;

	logger->dbg(LOG_SPRITE, "-- Init List");
	obj->animList = initListMgr();

	AssetMgr* ast = getAssets();
	char* spritePath = Str("animation/adventurer");
	Json* json = ast->getJson(spritePath);

	jsonPrint(json, 0);

	Json* sheet = jsonGetData(json, "sheet");
	Json* anims = jsonGetData(json, "anims");
	listIterateFnc(anims->childs, initAnims, NULL, obj);

	logger->dbg(LOG_SPRITE, "-- Init Anim Data");
	//SpriteAnimData* anim = new(SpriteAnimData);

	logger->dbg(LOG_SPRITE, "-- Adding To View");
	addObjectToView((Object*) obj);



	spriteAnim(obj, 1, 0);

	//logger->dbg(LOG_SPRITE, "-- Anim ID: #%d", anim->id);

	obj->pos.h = obj->curClip.h;
	obj->pos.w = obj->curClip.w;

	logger->dbg(LOG_SPRITE, "===== Sprite Object Ready =====");
	return obj;
}
