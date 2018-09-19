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

void collision_delete(Node* n) {
	Collision* col = (Collision*) n->value;
	free(col->name);
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

	ListManager* channel = collision_getChannel(col->type);
	const char* channelName = collision_getFlagName(col->type);
	logger->inf(LOG_COLLISION, "-- Setting Flags For: %s", channelName);

	if (channel == NULL) {
		logger->war(LOG_COLLISION, "Fail To Find Channel: #%d | %s", channelName);
		return;
	}

	while ((n = listIterate(channel, n)) != NULL) {
		CollisionType* type = (CollisionType*) n->value;

		logger->war(LOG_COLLISION, "-- Adding Channel: %s | %s", n->name, COL_TYPE_STRING[*type]);
		unsigned int value = collision_getFlagValue(n->name);

		if (*type == COL_OVERLAP) {
			logger->war(LOG_COLLISION, "-- Overlaps: %d", col->overlaps);
			col->overlaps = col->overlaps | value;
			logger->war(LOG_COLLISION, "-- Overlaps: %d", col->overlaps);
			logger->war(LOG_COLLISION, "-- Checking Channel: %s | %d", n->name, col->overlaps & value);
		}
		else {
			logger->war(LOG_COLLISION, "-- Blocks: %d", col->blocks);
			col->blocks = col->blocks | value;
			logger->war(LOG_COLLISION, "-- Blocks: %d", col->blocks);
			logger->war(LOG_COLLISION, "-- Checking Channel: %s | %d", n->name, col->blocks & value);
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
	col->flag = flag;
	col->type = type;
	//col->flags = initListMgr();
	collision_setFlags(col);

	col->enabled = enabled;
	col->continuous = false;

	col->name = Str(colName);

	col->onHit = NULL;
	col->onOverlap = NULL;

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
	bool width1 =	
		(col2->pos.x + col2->pos.w >= col->pos.x) 
		&&
		(col2->pos.x + col2->pos.w <= (col->pos.x + col->pos.w))
    ;

	bool width2 =
		(col->pos.x + col->pos.w >= col2->pos.x)
		&&
		(col->pos.x + col->pos.w <= col2->pos.x + col2->pos.w)
    ;

    bool height1 =	
    	(col2->pos.y + col2->pos.h >= col->pos.y)
    	&&
		(col2->pos.y + col2->pos.h <= col->pos.y + col->pos.h)
    ;
    
    bool height2 =	
    	(col->pos.y + col->pos.h >= col2->pos.y)
    	&&
		(col->pos.y + col->pos.h <= col2->pos.y + col2->pos.h)
    ;

    return (width1 || width2) && (height1 || height2);
}

CollisionType collision_flagsCollides(Collision* col, Collision* col2) {
	CollisionType type = COL_NULL;

	bool block = col->blocks & col2->flag;
	bool overlaps = col->overlaps & col2->flag;

	bool apply = (col2->blocks & col->flag) || (col2->overlaps & col->flag);

	apply = apply && (block || overlaps);

	if (apply && block) {
		type = COL_BLOCK;
	}
	else if(apply && overlaps) {
		type = COL_OVERLAP;
	}

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

	return Type;
}

short collision_handleCompCol(int i, Node* n, short* delete, void* param, va_list* args) {
	Object* obj = (Object*) param;
	Object* comp = va_arg(*args, Object*);

	Collision* objCol = va_arg(*args, Collision*);
	Collision* compCol = (Collision*) n->value;

	LOCK(compCol);

	logger->inf(LOG_COLLISION, "-- Comp Collision: %s", compCol->name);
	if (compCol->enabled && compCol->flag != COL_NONE) {
		logger->inf(LOG_COLLISION, "-- Comparing Collisions: %s | %s", objCol->name, compCol->name);

	}
	else{
		logger->inf(LOG_COLLISION, "-- Skipping Disabled");
	}


	UNLOCK(compCol);
	return true;
}


short collision_handleObjCol(int i, Node* n, short* delete, void* param, va_list* args) {
	Object* obj = (Object*) param;
	Object* comp = va_arg(*args, Object*);
	Collision* objCol = (Collision*) n->value;


	LOCK(objCol);
	logger->inf(LOG_COLLISION, "-- Object Collision: %s", objCol->name);

	if (objCol->enabled && objCol->flag != COL_NONE) {
		listIterateFnc(comp->collisions, collision_handleCompCol, NULL, obj, comp, objCol);
	}
	else{
		logger->inf(LOG_COLLISION, "-- Skipping Disabled");
	}

	UNLOCK(objCol);
	return true;
}

short collision_handleCheck(int i, Node* n, short* delete, void* param, va_list* args) {
	Object* obj = (Object*) param;
	Object* comp = (Object*) n->value;

	logger->inf(LOG_COLLISION, "\n\n+++++++++++++++++++++\n");
	LOCK(comp);
	logger->inf(LOG_COLLISION, "-- Compare: %s", comp->name);

	if (comp->enabled) {
		listIterateFnc(obj->collisions, collision_handleObjCol, NULL, obj, comp);
	}
	else{
		logger->inf(LOG_COLLISION, "-- Skipping Disabled");
	}

	UNLOCK(comp);
	return true;
}

short collision_handleIterate(int i, Node* n, short* delete, void* param, va_list* args) {

	return true;
}

void collision_handle() {
	logger->inf(LOG_COLLISION, "======== HANDELING COLLISIONS =======");
	ListManager* list = collision_getObjects();

	Node* n = NULL;
	while ((n = listIterate(list, n)) != NULL) {
		if (n->next == NULL) {
			break;
		}

		logger->inf(LOG_COLLISION, "\n\n---------------------------------------------------\n");
		Object* obj = (Object*) n->value;

		LOCK(obj);
		logger->inf(LOG_COLLISION, "-- Object: %s", obj->name);

		if (obj->enabled) {
			ListManager* list = collision_getObjects();
			listIterateFnc(list, collision_handleCheck, n->next, obj);
		}
		else{
			logger->inf(LOG_COLLISION, "-- Skipping Disabled");
		}


		UNLOCK(obj);
	}

//	listIterateFnc(list, collision_handleIterate, NULL, NULL);

	logger->inf(LOG_COLLISION, "======== HANDELe COLLISIONS DONE =======");
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
	jsonPrint(channels, 0);
	jsonIterate(channels, collision_addChannel, NULL);
}

void collision_initFlags(Json* conf) {
	Json* flagNames = jsonGetData(conf, "flags");
	jsonPrint(flagNames, 0);
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
