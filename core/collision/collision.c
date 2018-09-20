#include "collision.h"
#include "../asset/asset.h"
#include "../project/project.h"

ListManager* collision_getObjects() {
	static ListManager* list = NULL;

	if (list != NULL) {
		return list;
	}

	list = initListMgr();
	return list;
}


ListManager* collision_getChannels() {
	static ListManager* channels = NULL;
	if (channels != NULL) {
		return channels;
	}

	channels = initListMgr();
	return channels;
}

ListManager* collision_getChannel(const unsigned int id) {
	ListManager* channels = collision_getChannels();
	Node* n = getNode(channels, id);

	if (n == NULL) {
		return NULL;
	}

	return (ListManager*) n->value;
}

const char* collision_getChannelName(const unsigned int id)  {
	ListManager* channels = collision_getChannels();
	Node* n = getNode(channels, id);

	if (n == NULL) {
		return NULL;
	}

	return n->name;
}

ListManager* collision_getChannelByName(const char* name) {
	ListManager* channels = collision_getChannels();
	Node* n = getNodeByName(channels, name);

	if (n == NULL) {
		return NULL;
	}

	return (ListManager*) n->value;
}

ListManager* collision_getFlags() {
	static ListManager* flags = NULL;
	if (flags != NULL) {
		return flags;
	}

	flags = initListMgr();
	return flags;
}

unsigned int collision_getFlag(const char* name) {
	ListManager* flags = collision_getFlags();
	Node* n = getNodeByName(flags, name);

	if (n == NULL) {
		return COL_NONE;
	}

	return n->id;
}

const char* collision_getFlagName(unsigned int id) {
	ListManager* flags = collision_getFlags();
	Node* n = getNode(flags, id);

	if (n == NULL) {
		return "COL_NONE";
	}

	return n->name;
}


void collision_delete(Node* n) {
	Collision* col = (Collision*) n->value;

	free(col->name);
	deleteList(col->collisions);

	Project* pro = getProject();
	if (pro->flags & LOG_COLLISION) {
		removeChild(col->obj, col->name);
	}
}

void collision_removeAll(Node* n) {
	Object* obj = (Object*) n->value;

	if (obj->collisions == NULL) {
		return;
	}

	deleteList(obj->collisions);
	obj->collisions = NULL;
}

Collision* collision_getByName(Object* obj, const char* name) {
	if (obj == NULL) {
		logger->war(LOG_COLLISION, "Trying To Get Collision On Null Object !!!");
		return NULL;
	}

	if (obj->collisions == NULL) {
        return NULL;
	}

	char colName[150];
	memset(colName, 0, 150);
	snprintf(colName, 150, "%s__%s", obj->name, name);

	Node* n = getNodeByName(obj->collisions, name);

	return (Collision*) n->value;
}

void collision_addDebug(Object* obj, Collision* col) {
	logger->dbg(LOG_COLLISION, "-- Adding Debug Collision");
	AssetMgr* ast = getAssets();
	SDL_Surface* img = ast->getImg("debug/collision");

	if (img == NULL) {
		logger->war(LOG_COLLISION, "Fail To Find Collision Img: debug/collision");
	}

	float scaleH = (float) col->pos.h / (float) img->h;
	float scaleW = ((float) col->pos.w / (float) img->w) * 2.0f;

	logger->dbg(LOG_COLLISION, "-- Scaling Image: W: %f | H: %f", scaleW, scaleH);
	img = scaleImg(img, scaleW, scaleH);

	logger->dbg(LOG_COLLISION, "-- Generating DebugObj");
	Object* child = genSimpleObject(col->name, img, &col->pos, obj->z);

	child->clip = calloc(1, sizeof(SDL_Rect));

	child->clip->w = col->pos.w;
	child->clip->h = col->pos.h;

	logger->dbg(LOG_COLLISION, "-- Adding Child DebugObj");
	addChild(obj, child);

	logger->dbg(LOG_COLLISION, "-- Debug Collision Done");
}

void collision_setFlags(Collision* col) {
	Node* n = NULL;

	const char* channelName = collision_getFlagName(col->flag);
	ListManager* channel = collision_getChannelByName(channelName);
	logger->inf(LOG_COLLISION, "-- Setting Flags For: %s", channelName);

	if (channel == NULL) {
		logger->war(LOG_COLLISION, "Fail To Find Channel: #%d | %s", col->flag, channelName);
		ListManager* channels = collision_getChannels();
		printNodes(channels);

		return;
	}

	while ((n = listIterate(channel, n)) != NULL) {
		CollisionType* type = (CollisionType*) n->value;

		logger->dbg(LOG_COLLISION, "-- Adding Channel: %s | %s", n->name, COL_TYPE_STRING[*type]);
		unsigned int value = collision_getFlagValue(n->name);

		if (*type == COL_OVERLAP) {
			logger->dbg(LOG_COLLISION, "-- Overlaps: %d", col->overlaps);
			col->overlaps = col->overlaps | value;
			logger->dbg(LOG_COLLISION, "-- Overlaps: %d", col->overlaps);
			logger->dbg(LOG_COLLISION, "-- Checking Channel: %s | %d", n->name, col->overlaps & value);
		}
		else {
			logger->dbg(LOG_COLLISION, "-- Blocks: %d", col->blocks);
			col->blocks = col->blocks | value;
			logger->dbg(LOG_COLLISION, "-- Blocks: %d", col->blocks);
			logger->dbg(LOG_COLLISION, "-- Checking Channel: %s | %d", n->name, col->blocks & value);
		}

	}
}

Collision* collision_add(Object* obj, const char* name, unsigned int type, unsigned int flag, SDL_Rect pos, bool enabled) {
	if (obj == NULL) {
		logger->war(LOG_COLLISION, "Trying To Add Collision On Null Object !!!");
		return NULL;
	}

	char colName[150];
	memset(colName, 0, 150);
	snprintf(colName, 150, "%s__%s", obj->name, name);

	logger->inf(LOG_COLLISION, "===== ADDING COLLSION: %s =====", colName);
	Collision* col = new(Collision);

	col->pos = pos;
	col->obj = obj;
	col->flag = flag;
	col->type = type;

	col->enabled = enabled;
	col->continuous = false;

	col->name = Str(colName);
	col->blocks = 0;
	col->overlaps = 0;

	//col->flags = initListMgr();
	collision_setFlags(col);

	col->onHit = NULL;
	col->onHitEnd = NULL;
	col->onOverlap = NULL;
	col->onOverlapEnd = NULL;
	col->collisions = initListMgr();

	if (obj->collisions == NULL) {
		logger->dbg(LOG_COLLISION, "-- Init Object->Collision");
		obj->collisions = initListMgr();
	}

	logger->dbg(LOG_COLLISION, "-- Adding Collision To Object");
	Node* n = addNodeV(obj->collisions,  colName, col, 1);
	n->del = collision_delete;


	ListManager* list = collision_getObjects();
	n = getNodeByName(list, colName);
	if (n == NULL) {
		logger->dbg(LOG_COLLISION, "-- Adding Object To Collision List");
		n = addNodeV(list, obj->name, obj, 0);
		n->del = collision_removeAll;
	}


	Project* pro = getProject();
	if (pro->flags & LOG_COLLISION) {
		logger->dbg(LOG_COLLISION, "-- Collision Debug Enabled !!");
		collision_addDebug(obj, col);
	}

	logger->dbg(LOG_COLLISION, "-- Collision Ready");
	return col;
}

bool collision_positionCollides(Collision* col, Collision* col2) {
	logger->inf(LOG_COLLISION, "-- Checking Positions");

	SDL_Rect pos = getWorldPos(col->obj, col->pos);
	SDL_Rect pos2 = getWorldPos(col2->obj, col2->pos);

	logger->inf(LOG_COLLISION, "-- Col1: X: %d | Y: %d | W: %d | H: %d", pos.x, pos.y, pos.w, pos.h);
	logger->inf(LOG_COLLISION, "-- Col2: X: %d | Y: %d | W: %d | H: %d", pos2.x, pos2.y, pos2.w, pos2.h);

	bool width1 =
		(pos2.x + pos2.w >= pos.x)
		&&
		(pos2.x + pos2.w <= (pos.x + pos.w))
    ;

	bool width2 =
		(pos.x + pos.w >= pos2.x)
		&&
		(pos.x + pos.w <= pos2.x + pos2.w)
    ;

    bool height1 =
    	(pos2.y + pos2.h >= pos.y)
    	&&
		(pos2.y + pos2.h <= pos.y + pos.h)
    ;

    bool height2 =
    	(pos.y + pos.h >= pos2.y)
    	&&
		(pos.y + pos.h <= pos2.y + pos2.h)
    ;

    bool collides = (width1 || width2) && (height1 || height2);
	logger->inf(LOG_COLLISION, "-- Positions Collides: %d", collides);

    return collides;
}

CollisionType collision_flagsCollides(Collision* col, Collision* col2) {
	CollisionType type = COL_IGNORE;

	logger->inf(LOG_COLLISION, "-- Checking Flags: %s | %s", collision_getFlagName(col->flag), collision_getFlagName(col2->flag));
	bool block = col->blocks & col2->flag;
	bool overlaps = col->overlaps & col2->flag;

	logger->inf(LOG_COLLISION, "-- Block: %d", block);
	logger->inf(LOG_COLLISION, "-- Overlap: %d", overlaps);

	bool apply = (col2->blocks & col->flag) || (col2->overlaps & col->flag);
	logger->inf(LOG_COLLISION, "-- Apply: %d", apply);
	logger->inf(LOG_COLLISION, "-- Col2 Block: %d", (col2->blocks & col->flag));
	logger->inf(LOG_COLLISION, "-- Col2 Block Test: %d", col2->blocks);
	logger->inf(LOG_COLLISION, "-- Col2 Overlap: %d", (col2->overlaps & col->flag));
	logger->inf(LOG_COLLISION, "-- Col2 Overlap Test: %d", col2->overlaps);

	apply = apply && (block || overlaps);
	logger->inf(LOG_COLLISION, "-- Final Apply: %d", apply);

	if (apply && block) {
		type = COL_BLOCK;
	}
	else if(apply && overlaps) {
		type = COL_OVERLAP;
	}

	logger->inf(LOG_COLLISION, "-- Result Type: %s", COL_TYPE_STRING[type]);
    return type;
}



CollisionType collision_collides(Collision* col, Collision* col2) {
	CollisionType type = COL_IGNORE;

	if (collision_positionCollides(col, col2)) {
		logger->inf(LOG_COLLISION, "-- Position Collides !!!");
		type = collision_flagsCollides(col, col2);
	}
	else {
		logger->inf(LOG_COLLISION, "-- Skipping Pisition Does Not Collide");
	}

	return type;
}

void collision_callFncs(Collision* col, Collision* col2, CollisionType type, bool start) {
	switch (type) {
		case COL_BLOCK:
			if (col->onHit != NULL) {
				if (start) {
					col->onHit(col->obj, col2->obj);
				}
				else {
					col->onHitEnd(col->obj, col2->obj);
				}
			}
			else {
				logger->war(LOG_COLLISION, "Collision Function Is Null: %s | %d", col->obj->name, type);
			}
			break;

		case COL_OVERLAP:
			if (col->onOverlap != NULL) {
				if (start) {
					col->onOverlap(col->obj, col2->obj);
				}
				else {
					col->onOverlapEnd(col->obj, col2->obj);
				}
			}
			else {
				logger->war(LOG_COLLISION, "Collision Function Is Null: %s | %d", col->obj->name, type);
			}
			break;
	}
}

bool collision_addCollide(Collision* col, Collision* target) {
	Node* n = getNodeByName(col->collisions, target->name);
	if (n != NULL) {
		return false;
	}

	logger->inf(LOG_COLLISION, "-- Adding Collide: %p | %s", col->collisions, target->name);
	logger->inf(LOG_COLLISION, "-- Current Size: %d", col->collisions->nodeCount);
	addNodeV(col->collisions, target->name, target, 0);
	logger->inf(LOG_COLLISION, "-- Added");

	n = getNodeByName(col->obj->childs, col->name);
	if (n != NULL) {
		Object* colObj = (Object*) n->value;
		colObj->clip->x = colObj->clip->w;
	}

	return true;
}

bool collision_removeCollide(Collision* col, Collision* target) {
	Node* n = getNodeByName(col->collisions, target->name);
	if (n == NULL) {
		return false;
	}

	logger->inf(LOG_COLLISION, "-- Removing Collide: %s", target->name);
	deleteNodeByName(col->collisions, target->name);
	logger->inf(LOG_COLLISION, "-- Collides Left: %d", col->collisions->nodeCount);

	if (!col->collisions->nodeCount) {
		n = getNodeByName(col->obj->childs, col->name);

		Object* colObj = (Object*) n->value;
		colObj->clip->x = 0;
	}

	return true;
}

short collision_handleCompCol(int i, Node* n, short* delete, void* param, va_list* args) {
	Object* obj = (Object*) param;
	Object* comp = va_arg(*args, Object*);

	Collision* objCol = va_arg(*args, Collision*);
	Collision* compCol = (Collision*) n->value;

	//logger->err(LOG_ANIM, "Lock Comp Col");
	LOCK(compCol, "HANDLE COMP COL-0");

	// logger->inf(LOG_COLLISION, "-- Comp Collision: %s", compCol->name);
	if (compCol->enabled && compCol->flag != COL_NONE) {
		// logger->inf(LOG_COLLISION, "-- Comparing Collisions: %s | %s", objCol->name, compCol->name);

		CollisionType type = collision_flagsCollides(objCol, compCol);
		// logger->inf(LOG_COLLISION, "-- Collision Flag Result: %s", COL_TYPE_STRING[type]);

		if (type != COL_IGNORE && collision_positionCollides(objCol, compCol)) {
			if (collision_addCollide(objCol, compCol)) {
				//logger->inf(LOG_COLLISION, "-- Call Fnc");
				collision_callFncs(objCol, compCol, type, true);
				collision_callFncs(compCol, objCol, type, true);
			}
		}
		else  if (collision_removeCollide(objCol, compCol)) {
			collision_callFncs(objCol, compCol, type, false);
			collision_callFncs(compCol, objCol, type, false);
		}

	}
	else{
		//logger->inf(LOG_COLLISION, "-- Skipping Disabled");
	}


	//logger->err(LOG_ANIM, "UnLock Comp Col");
	UNLOCK(compCol, "HANDLE COMP COL-2");
	return true;
}


short collision_handleObjCol(int i, Node* n, short* delete, void* param, va_list* args) {
	Object* obj = (Object*) param;
	Object* comp = va_arg(*args, Object*);
	Collision* objCol = (Collision*) n->value;

	//logger->err(LOG_ANIM, "Lock Obj Col Pos");
	LOCK(objCol, "HANDLE OBJ COL-0");
	//logger->inf(LOG_COLLISION, "-- Object Collision: %s", objCol->name);

	if (objCol->enabled && objCol->flag != COL_NONE) {
		listIterateFnc(comp->collisions, collision_handleCompCol, NULL, obj, comp, objCol);
	}
	else{
		//logger->inf(LOG_COLLISION, "-- Skipping Disabled");
	}

	//logger->err(LOG_ANIM, "UnLock Obj Col Pos");
	UNLOCK(objCol, "HANDLE COMP COL-1");
	return true;
}

short collision_handleCheck(int i, Node* n, short* delete, void* param, va_list* args) {
	Object* obj = (Object*) param;
	Object* comp = (Object*) n->value;

	//logger->inf(LOG_COLLISION, "\n\n+++++++++++++++++++++\n");
	//logger->err(LOG_ANIM, "Lock Comp Pos");
	LOCK(comp, "HANDLE CHECK-0");
	//logger->inf(LOG_COLLISION, "-- Compare: %s", comp->name);

	if (comp->enabled) {
		listIterateFnc(obj->collisions, collision_handleObjCol, NULL, obj, comp);
	}
	else{
		//logger->inf(LOG_COLLISION, "-- Skipping Disabled");
	}

	//logger->err(LOG_ANIM, "UnLock Comp Pos");
	UNLOCK(comp, "HANDLE CHECK-1");
	return true;
}

short collision_handleIterate(int i, Node* n, short* delete, void* param, va_list* args) {

	return true;
}

void collision_handle() {
	//logger->inf(LOG_COLLISION, "======== HANDELING COLLISIONS =======");
	ListManager* list = collision_getObjects();

	Node* n = NULL;
	while ((n = listIterate(list, n)) != NULL) {
		if (n->next == NULL) {
			break;
		}

		//logger->inf(LOG_COLLISION, "\n\n---------------------------------------------------\n");
		Object* obj = (Object*) n->value;

		//logger->err(LOG_ANIM, "Lock Obj Pos");
		LOCK(obj, "HANDLE-0");
		//logger->inf(LOG_COLLISION, "-- Object: %s", obj->name);

		if (obj->enabled) {
			ListManager* list = collision_getObjects();
			listIterateFnc(list, collision_handleCheck, n->next, obj);
		}
		else{
			//logger->inf(LOG_COLLISION, "-- Skipping Disabled");
		}


		//logger->err(LOG_ANIM, "UnLock Obj Pos");
		UNLOCK(obj, "HANDLE-1");
	}

//	listIterateFnc(list, collision_handleIterate, NULL, NULL);

	//logger->inf(LOG_COLLISION, "======== HANDELe COLLISIONS DONE =======");
}


bool collision_addFlag(unsigned int i, Json* json, void* param) {
	logger->inf(LOG_COLLISION, "======== Adding Collision Flag: %s =======", json->string);

	ListManager* flags = collision_getFlags();

	unsigned int id = flags->nodeCount;
	if (id > 1) {
		id = 1 << flags->nodeCount;
	}
	logger->inf(LOG_COLLISION, "-- Value: %u", id);

	Node* n = addNode(flags, json->string);
	n->id = id;

	return true;
}

void collision_deleteChannel(Node* n) {
	logger->inf(LOG_COLLISION, "======== Deleting Collision Channel: %s =======", n->name);
	ListManager* channel = (ListManager*) n->value;

	deleteList(channel);
}

CollisionType collision_getTypeByName(const char* type) {
	for (int i = 0; COL_TYPE_STRING[i]; ++i) {
		if (!strcmp(COL_TYPE_STRING[i], type)) {
			return i;
		}
	}

	return COL_IGNORE;
}

bool collision_loadChannel(unsigned int i, Json* json, void* param) {
	logger->inf(LOG_COLLISION, "-- Key: %s =======", json->key);
	ListManager* channel = (ListManager*) param;

	CollisionType* type = malloc(sizeof(CollisionType));
	*type = collision_getTypeByName(json->string);

	logger->inf(LOG_COLLISION, "-- value: %s | %d =======", json->string, *type);
	addNodeV(channel, json->key, type, 1);

	return true;
}

bool collision_addChannel(unsigned int i, Json* channel, void* param) {
	logger->inf(LOG_COLLISION, "======== Adding Collision Channel: %s =======", channel->key);

	ListManager* channels = collision_getChannels();
	Node* n = getNodeByName(channels, channel->key);
	if (n != NULL) {
		logger->war(LOG_COLLISION, "Trying To Add Duplicate Collision Channel: %s", channel->key);
		return true;
	}


	n = addNodeV(channels, channel->key, initListMgr(), 0);

	logger->inf(LOG_COLLISION, "======== Loading Collision Channel: %s =======", channel->key);
	jsonIterate(channel, collision_loadChannel, n->value);

	return true;
}

void collision_initChannels(Json* conf) {
	Json* channels = jsonGetData(conf, "channels");
	// jsonPrint(channels, 0);
	jsonIterate(channels, collision_addChannel, NULL);
}

void collision_initFlags(Json* conf) {
	Json* flagNames = jsonGetData(conf, "flags");
	// jsonPrint(flagNames, 0);
	jsonIterate(flagNames, collision_addFlag, NULL);
}

void collision_init() {
	static bool isInit = false;
	if (isInit) {
		return;
	}

	AssetMgr* ast = getAssets();
	Json* conf = ast->getConf("collision/collision");


	collision_initFlags(conf);
	collision_initChannels(conf);

	isInit = true;
}

unsigned int collision_getFlagValue(const char* flag) {
	ListManager* flags = collision_getFlags();
	Node* n = getNodeByName(flags, flag);

	if (n == NULL) {
		return 0;
	}

	return n->id;
}

bool collision_flagsMatch(const char* flag1, const char* flag2) {
	unsigned int f1 = collision_getFlagValue(flag1);
	unsigned int f2 = collision_getFlagValue(flag2);

	return f2 & f1;
}
