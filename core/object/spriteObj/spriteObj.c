#include "./spriteObj.h"
#include "../../../base/json.h"
#include "../../asset/asset.h"
#include "../../animation/sprite/spriteAnim.h"
#include "./character/charObj.h"
#include "../../movement/movement.h"

void footLand(Object* self, Object* target) {
	CharObj* obj = (CharObj*) self;
	obj->ch->attr.inAir = false;
}

void footFall(Object* self, Object* target) {
	CharObj* obj = (CharObj*) self;
	obj->ch->attr.inAir = true;
}

bool Idl2Run(SpriteAnimParam* o) {
	CharObj* obj = (CharObj*) o->obj;

	if (obj->ch->attr.moving) {
		logger->inf(LOG_SPRITE, "===== !!!! Idl2Run !!!! ====");
		return true;
	}

	return false;
}

bool Idle2Down(SpriteAnimParam* o) {
	CharObj* obj = (CharObj*) o->obj;

	if (obj->ch->attr.crouch) {
		logger->inf(LOG_SPRITE, "===== !!!! Idl2Down !!!! ====");
		return true;
	}

	return false;
}

bool Idl2Jump(SpriteAnimParam* obj) {
	//logger->inf(LOG_SPRITE, "===== !!!! Idl2Jump !!!! ====");
	return false;
}

bool Fall2Land(SpriteAnimParam* o) {
	CharObj* obj = (CharObj*) o->obj;

	if (obj->ch->attr.inAir) {
		return false;
	}

	logger->err(LOG_SPRITE, "===== !!!! Fall2Land !!!! ====");
	obj->ch->attr.doubleJump = false;
	obj->ch->attr.hasDoubleJump = false;

	setVelocityY((Object*) obj, 0);
	return true;
}

bool Idle2Fall(SpriteAnimParam* o) {
	CharObj* obj = (CharObj*) o->obj;

	if (obj->ch->attr.inAir) {
		logger->inf(LOG_SPRITE, "===== !!!! Idle2Fall !!!! ====");

		setVelocityY((Object*) obj, 50);
		return true;
	}

	return false;
}

bool Down2Jump(SpriteAnimParam* o) {
	CharObj* obj = (CharObj*) o->obj;

	if (obj->ch->attr.inAir) {
		logger->inf(LOG_SPRITE, "===== !!!! Down2Jump !!!! ====");
		return true;
	}

	return false;
}

bool Down2Idle(SpriteAnimParam* o) {
	CharObj* obj = (CharObj*) o->obj;

	if (!obj->ch->attr.crouch) {
		logger->inf(LOG_SPRITE, "===== !!!! Down2Jump !!!! ====");
		return true;
	}

	return false;
}

bool Run2Idle(SpriteAnimParam* o) {
	CharObj* obj = (CharObj*) o->obj;

	if (!obj->ch->attr.moving) {
		logger->inf(LOG_SPRITE, "===== !!!! Run2Idle !!!! ====");

		return true;
	}

	return false;
}

bool Run2Jump(SpriteAnimParam* o) {
	CharObj* obj = (CharObj*) o->obj;

	if (obj->ch->attr.inAir) {
		logger->inf(LOG_SPRITE, "===== !!!! Run2Jump !!!! ====");
		return true;
	}

	return false;
}

bool Jump2DoubleJump(SpriteAnimParam* o) {
	CharObj* obj = (CharObj*) o->obj;

	if (!obj->ch->attr.hasDoubleJump && obj->ch->attr.doubleJump) {
		obj->ch->attr.hasDoubleJump = true;
		logger->inf(LOG_SPRITE, "===== !!!!  Jump2DoubleJump !!!! ====");
		setVelocityY((Object*) obj, -45);
		return true;
	}

	return false;
}

void initAnimsLinks() {
	static bool isInit = false;

	if (isInit) {
		return;
	}

	addAnimLinkFncs("Idl2Run", Idl2Run);
	addAnimLinkFncs("Idl2Jump", Idl2Jump);
	addAnimLinkFncs("Run2Idle", Run2Idle);
	addAnimLinkFncs("Run2Jump", Run2Jump);
	addAnimLinkFncs("Fall2Land", Fall2Land);
	addAnimLinkFncs("Idle2Fall", Idle2Fall);
	addAnimLinkFncs("Down2Jump", Down2Jump);
	addAnimLinkFncs("Idle2Down", Idle2Down);
	addAnimLinkFncs("Down2Idle", Down2Idle);
	addAnimLinkFncs("Jump2DoubleJump", Jump2DoubleJump);

	isInit = true;
}

void SpriteObjectDelete(Object* o) {
	SpriteObject* obj = (SpriteObject*) o;
	logger->inf(LOG_SPRITE, "===== DELETING SPRITE OBJ: %s =====", obj->name);

	deleteList(obj->animList);
	deleteObject(o);
}

SpriteAnimData* addAnim(SpriteObject* obj, char* name, int fps, int clipCnt, int clipIndex, unsigned short row) {
	SpriteAnimData* anim = new(SpriteAnimData);

	anim->fps = fps;
	anim->name = Str(name);
	anim->clipCnt = clipCnt;
	obj->clip = &obj->curClip;
	anim->wait = (FPS / anim->fps);

	anim->duration = (anim->fps * anim->clipCnt);
	anim->clipPos = malloc(sizeof(SDL_Rect) * anim->clipCnt);

	int cell_x = obj->cell_x;
	int cell_y = obj->cell_y;

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
	n->del = SpriteAnimDataDelete;

	return anim;
}

short check(int i, Node* n, short* delete, void* param, va_list* arg) {
	AnimLink* link = (AnimLink*) n->value;
	if (link->fnc == NULL) {
		logger->inf(LOG_SPRITE, "-- ANIM IS NULL !!!");
		return true;
	}

	logger->inf(LOG_SPRITE, "-- Node: %p", n);
	logger->inf(LOG_SPRITE, "-- Node Value: %p", n->value);
	logger->inf(LOG_SPRITE, "-- Link: %p", link);
	logger->inf(LOG_SPRITE, "-- Link Func: %p", link->fnc);

	return false;
}

bool initAnimLink(unsigned int i, Json* json, void* cont) {
	logger->inf(LOG_SPRITE, "-- Link: %d => %s", i, json->key);

	if (cont == NULL) {
		logger->err(LOG_SPRITE, "ANIM LINK CONTAINER IS NULL !!!");
		assert(0);
	}

	AnimLink* link = new(AnimLink);
	jsonGetValue(json, "waitEnd", &(link->waitEnd));
	logger->inf(LOG_SPRITE, "-- WaitEnd: %d", link->waitEnd);

	char* name = jsonGetValue(json, "fnc", NULL);
	link->name = Str(name);

	SpriteAnimData* anim = (SpriteAnimData*) cont;
	link->target = jsonGetValue(json, "target", NULL);

	logger->inf(LOG_SPRITE, "-- Target: %s", link->target);

	logger->inf(LOG_SPRITE, "-- Link Fnc Name: %s", name);
	AnimLinkFnc* linkFnc = getAnimLinkFnc(name);
	logger->inf(LOG_SPRITE, "-- Anim Function Struc: %p", linkFnc);

	char* animName = NULL;
	if (linkFnc == NULL) {
		logger->inf(LOG_SPRITE, "-- Anim Function IS NULL");
		link->fnc = NULL;

		animName = StrE(150);
		snprintf(animName, 150, "%s_2_%s", anim->name, json->key);
	}
	else {
		logger->inf(LOG_SPRITE, "-- Anim Function: %p", linkFnc->fnc);
		link->fnc = linkFnc->fnc;
		animName = name;
	}

	logger->inf(LOG_SPRITE, "-- Adding Link Fnc To List");
	Node* n = addNodeV(anim->animLinks, animName, link, 1);
	n->del = AnimLinkDelete;

	return true;
}

short initAnims(int i, Node* n, short* delete, void* param, va_list* args) {
	logger->inf(LOG_SPRITE, "======= Init Anim: #%d => %s =====", i, n->name);
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

	int data[6];
	data[6] = 0;
	char* name = jsonGetValue(json, "name", NULL);

	jsonGetValue(json, "fps", &(data[0]));
	jsonGetValue(json, "count", &(data[1]));
	jsonGetValue(json, "index", &(data[2]));
	jsonGetValue(json, "row", &(data[3]));

	logger->dbg(LOG_SPRITE, "Adding Anim #%d => %s", i, name);
	logger->dbg(LOG_SPRITE, "-- fps: %d", data[0]);
	logger->dbg(LOG_SPRITE, "-- count: %d", data[1]);
	logger->dbg(LOG_SPRITE, "-- index: %d", data[2]);
	logger->dbg(LOG_SPRITE, "-- row: %d", data[3]);

	SpriteAnimData* anim = addAnim(obj, name, (int) data[0], (int) data[1], (int) data[2], (unsigned short) data[3]);
	anim->name = name;

	jsonGetValue(json, "loop", &anim->loop);
	logger->dbg(LOG_SPRITE, "-- Loop: %d", anim->loop);

	Json* links = jsonGetData(json, "links");

	initAnimsLinks();
	if (links != NULL && links->childCount) {
		logger->dbg(LOG_SPRITE, "-- Has Links: %d !!!", links->childCount);
		anim->animLinks = initListMgr();
		jsonIterate(links, initAnimLink, (void*) anim);
	}
	else {
		anim->animLinks = NULL;
	}


    return true;
}

SpriteObject* initSpriteObj(SpriteObject* obj, char* name, char* path, SDL_Rect* pos, short z) {
	logger->inf(LOG_SPRITE, "-- Init Sprite Object: %s", name);
	initSimpleObject((Object*)obj, name, NULL, pos, z);
	bool b = LOCK(obj, "INIT SPRITE-0");

	logger->dbg(LOG_SPRITE, "-- Init List");
	obj->animList = initListMgr();

	int len = strlen(path)+7;

	char jsonPath[len];
	memset(jsonPath, 0, len);
	snprintf(jsonPath, len, "sheet/%s", path);

	logger->dbg(LOG_SPRITE, "-- Fetching Anim Json: %p", jsonPath);
	AssetMgr* ast = getAssets();
	Json* json = ast->getJson(jsonPath);
	//jsonPrint(json, 0);


	logger->dbg(LOG_SPRITE, "-- Get Sheet Data");
	Json* sheet = jsonGetData(json, "sheet");
	char* sheetPath = jsonGetValue(sheet, "path", NULL);
	logger->dbg(LOG_SPRITE, "-- Path: %s", sheetPath);

	logger->dbg(LOG_SPRITE, "-- Fetching Sheet");
	setObjSurface((Object*) obj, ast->getImg(sheetPath));

	if (obj->component == NULL) {
		logger->war(LOG_SPRITE, "Fail To Find Sprite Sheet: %s", sheetPath);
	}

	int values[5];
	jsonGetValue(sheet, "rows", &values[0]);
	obj->spriteRows = values[0];
	logger->dbg(LOG_SPRITE, "-- Rows: %d", obj->spriteRows);

	jsonGetValue(sheet, "columns", &values[1]);
	obj->spriteColumns = values[1];
	logger->dbg(LOG_SPRITE, "-- Columns: %d", obj->spriteColumns);

	jsonGetValue(sheet, "cell_x", &values[2]);
	obj->cell_x = values[2];
	logger->dbg(LOG_SPRITE, "-- Cell_x: %d", obj->cell_x);

	jsonGetValue(sheet, "cell_y", &values[3]);
	obj->cell_y = values[3];
	logger->dbg(LOG_SPRITE, "-- Cell_y: %d", obj->cell_y);



	logger->inf(LOG_SPRITE, "-- Getting Anims:");
	Json* anims = jsonGetData(json, "anims");
	logger->inf(LOG_SPRITE, "-- Childs: %p", anims);

	if (anims == NULL) {
		jsonPrint(json, 0);
	}

	listIterateFnc(anims->childs, initAnims, NULL, obj);

	logger->dbg(LOG_SPRITE, "-- Init Anim Data");
	//SpriteAnimData* anim = new(SpriteAnimData);

	logger->dbg(LOG_SPRITE, "-- Adding To View");
	addObjectToView((Object*) obj);

	spriteAnimByName(obj, "Idle", 0);

	//logger->dbg(LOG_SPRITE, "-- Anim ID: #%d", anim->id);

	obj->pos.h = obj->curClip.h;
	obj->pos.w = obj->curClip.w;
	UNLOCK(obj, "INIT SPRITE-1", b);

	return obj;
}

SpriteObject* newSpriteObject(char* name, char* path, SDL_Rect* pos, short z) {
	logger->inf(LOG_SPRITE, "==== ADDING SPRITE OBJECT: %s ====", name);

	SpriteObject* obj = new(SpriteObject);
	initSpriteObj(obj, name, path, pos, z);
	obj->delete = NULL;

	logger->dbg(LOG_SPRITE, "===== Sprite Object Ready =====");
	return obj;
}
