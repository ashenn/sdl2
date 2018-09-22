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
	col->continious = false;

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

bool collision_positionCollides(SDL_Rect* pos, SDL_Rect* pos2) {
	bool width1 =
		(pos2->x + pos2->w >= pos->x)
		&&
		(pos2->x + pos2->w <= (pos->x + pos->w))
    ;

	bool width2 =
		(pos->x + pos->w >= pos2->x)
		&&
		(pos->x + pos->w <= pos2->x + pos2->w)
    ;

    bool height1 =
    	(pos2->y + pos2->h >= pos->y)
    	&&
		(pos2->y + pos2->h <= pos->y + pos->h)
    ;

    bool height2 =
    	(pos->y + pos->h >= pos2->y)
    	&&
		(pos->y + pos->h <= pos2->y + pos2->h)
    ;

    bool collides = (width1 || width2) && (height1 || height2);
    return collides;
}

CollisionType collision_flagsCollides(Collision* col, Collision* col2) {
	CollisionType type = COL_IGNORE;

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

void collision_callFncs(Collision* col, Collision* col2, CollisionType type, bool start) {
	switch (type) {
		case COL_BLOCK:
			if (start && col->onHit != NULL) {
				col->onHit(col->obj, col2->obj);
			}
			else if (!start && col->onHitEnd != NULL){
				col->onHitEnd(col->obj, col2->obj);
			}
			break;

		case COL_OVERLAP:
			if (start && col->onOverlap != NULL) {
				col->onOverlap(col->obj, col2->obj);
			}
			else if (!start && col->onOverlapEnd != NULL){
				col->onOverlapEnd(col->obj, col2->obj);
			}
			break;

        default:
            break;
	}
}

bool collision_addCollide(Collision* col, Collision* target) {
	Node* n = getNodeByName(col->collisions, target->name);
	if (n != NULL) {
		return false;
	}

	logger->dbg(LOG_COLLISION, "-- Adding Collide : %s", target->name);
	addNodeV(col->collisions, target->name, target, 0);



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

	logger->dbg(LOG_COLLISION, "-- Removing Collide: %s", target->name);
	deleteNodeByName(col->collisions, target->name);

	if (!col->collisions->nodeCount) {
		if (col->obj->childs != NULL) {
			n = getNodeByName(col->obj->childs, col->name);

			if (n != NULL) {
				Object* colObj = (Object*) n->value;
				colObj->clip->x = 0;
			}
		}
	}

	return true;
}

short collision_handleCompCol(int i, Node* n, short* delete, void* param, va_list* args) {
	Collision* objCol = (Collision*) param;
	Collision* compCol = (Collision*) n->value;

	//logger->err(LOG_COLLISION, "Lock Comp Col");
	bool b = LOCK(compCol, "HANDLE COMP COL-0");

	// logger->inf(LOG_COLLISION, "-- Comp Collision: %s", compCol->name);
	if (compCol->enabled && compCol->flag != COL_NONE) {
		// logger->inf(LOG_COLLISION, "-- Comparing Collisions: %s | %s", objCol->name, compCol->name);

		SDL_Rect pos = getWorldPos(objCol->obj, objCol->pos);
		SDL_Rect pos2 = getWorldPos(compCol->obj, compCol->pos);
		bool posCollide = collision_positionCollides(&pos, &pos2);
		if (posCollide) {
			logger->dbg(LOG_COLLISION, "Objects Collides: %s | %s", objCol->name, compCol->name);
		}

		CollisionType type = collision_flagsCollides(objCol, compCol);
		// logger->inf(LOG_COLLISION, "-- Collision Flag Result: %s", COL_TYPE_STRING[type]);

		if (type != COL_IGNORE && posCollide) {
			if (collision_addCollide(objCol, compCol)) {
				logger->inf(LOG_COLLISION, "-- Call Fnc");
				collision_callFncs(objCol, compCol, type, true);
				collision_callFncs(compCol, objCol, type, true);
			}
		}
		else  if (collision_removeCollide(objCol, compCol)) {
			logger->inf(LOG_COLLISION, "-- Call Stop Fnc");

			collision_callFncs(objCol, compCol, type, false);
			logger->inf(LOG_COLLISION, "-- Call END-1");

			collision_callFncs(compCol, objCol, type, false);
			logger->inf(LOG_COLLISION, "-- Call END-2");
		}


	}
	else{
		logger->inf(LOG_COLLISION, "-- Skipping Disabled");
	}


	//logger->err(LOG_COLLISION, "UnLock Comp Col");
	UNLOCK(compCol, "HANDLE COMP COL-2", b);
	return true;
}


short collision_handleObjCol(int i, Node* n, short* delete, void* param, va_list* args) {
	Object* comp = (Object*) param;
	Collision* objCol = (Collision*) n->value;

	//logger->err(LOG_COLLISION, "Lock Obj Col Pos");
	bool b = LOCK(objCol, "HANDLE OBJ COL-0");
	logger->inf(LOG_COLLISION, "-- Object Collision: %s", objCol->name);

	if (objCol->enabled && objCol->flag != COL_NONE) {
		listIterateFnc(comp->collisions, collision_handleCompCol, NULL, objCol);
	}
	else{
		logger->inf(LOG_COLLISION, "-- Skipping Disabled");
	}

	//logger->err(LOG_COLLISION, "UnLock Obj Col Pos");
	UNLOCK(objCol, "HANDLE COMP COL-1", b);
	return true;
}

short collision_handleCheck(int i, Node* n, short* delete, void* param, va_list* args) {
	Object* obj = (Object*) param;
	Object* comp = (Object*) n->value;

	logger->inf(LOG_COLLISION, "\n\n+++++++++++++++++++++\n");
	//logger->err(LOG_COLLISION, "Lock Comp Pos");
	bool b = LOCK(comp, "HANDLE CHECK-0");
	logger->inf(LOG_COLLISION, "-- Compare: %s", comp->name);

	if (comp->enabled) {
		listIterateFnc(obj->collisions, collision_handleObjCol, NULL, comp);
	}
	else{
		logger->inf(LOG_COLLISION, "-- Skipping Disabled");
	}

	//logger->err(LOG_COLLISION, "UnLock Comp Pos");
	UNLOCK(comp, "HANDLE CHECK-1", b);
	return true;
}

void collision_handleIterate() {
	logger->inf(LOG_COLLISION, "======== HANDELING COLLISIONS =======");

	Node* n = NULL;
	ListManager* list = collision_getObjects();
	while ((n = listIterate(list, n)) != NULL) {
		if (n->next == NULL) {
			break;
		}

		logger->inf(LOG_COLLISION, "\n\n---------------------------------------------------\n");
		Object* obj = (Object*) n->value;

		//logger->err(LOG_COLLISION, "Lock Obj Pos");
		bool b = LOCK(obj, "HANDLE-0");
		logger->inf(LOG_COLLISION, "-- Object: %s", obj->name);

		if (obj->enabled) {
			ListManager* list = collision_getObjects();
			listIterateFnc(list, collision_handleCheck, n->next, obj);
		}
		else{
			logger->inf(LOG_COLLISION, "-- Skipping Disabled");
		}


		//logger->err(LOG_COLLISION, "UnLock Obj Pos");
		UNLOCK(obj, "HANDLE-1", b);
	}

//	listIterateFnc(list, collision_handleIterate, NULL, NULL);

	logger->inf(LOG_COLLISION, "======== HANDELe COLLISIONS DONE =======");

	return;
}

void* collision_handle(void* arg) {
	Project* pro = getProject();
	LOCK(pro, "Collision Handle Start");

	int nextTick = SDL_GetTicks();
	while (pro->status < PRO_END) {
		UNLOCK(pro, "Collision Handle UNLock Loop", true);
		nextTick += (1500 / FPS);

		collision_handleIterate();

		int waited = tickWait(nextTick);
		logger->dbg(LOG_COLLISION, "Collision Wait: %d ms", waited);

		LOCK(pro, "Collision Handle Lock Loop");
	}

	UNLOCK(pro, "Collision Handle END", true);
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



ListManager* findColAtPos(SDL_Rect pos, unsigned int flag, unsigned int blocks, unsigned int overlaps, Object* ignore) {
	Node* objN = NULL;
	ListManager* result = initListMgr();
	ListManager* objects = collision_getObjects();

	Collision testCol;

	testCol.pos = pos;
	testCol.flag = flag;
	testCol.enabled = true;
	testCol.blocks = blocks;
	testCol.overlaps = overlaps;


	while ((objN = listIterate(objects, objN)) != NULL) {
		Object* obj = (Object*) objN->value;
		if (obj == ignore) {
			continue;
		}


		Node* colN = NULL;
		if (obj->collisions != NULL)
		{
			while ((colN = listIterate(obj->collisions, colN)) != NULL) {
				Collision* col = (Collision*) colN->value;

				SDL_Rect pos2 = getWorldPos(col->obj, col->pos);
				if (collision_flagsCollides(&testCol, col) && collision_positionCollides(&testCol.pos, &pos2)) {
					addNodeV(result, "col", col, false);
				}
			}
		}
	}

	return result;
}

short checkMoveTo(int i, Node* n, short* delete, void* param, va_list* args) {
	Collision* col = (Collision*) n->value;
	logger->err(LOG_COLLISION, "\n\n\n######### Check Can Move: %s", col->name);
	if (!col->enabled) {
		logger->err(LOG_COLLISION, "-- Skip");
		return true;
	}

	bool b = LOCK(col, "Check Move START");

	vector* target = (vector*) param;
    Object* obj = col->obj;
	SDL_Rect pos = getWorldPos(obj, col->pos);

	vector final;
	final.x = target->x + col->pos.x;
	final.y = target->y + col->pos.y;

	int xFact = 0;
	if (pos.x != (int) final.x) {
		xFact = pos.x < (int) final.x ? 1 : -1;
	}

	int yFact = 0;
	if (pos.y != (int) final.y) {
		yFact = pos.y < (int) final.y ? 1 : -1;
	}

	bool block = false;
	int a = 0;
	while (pos.x != final.x || pos.y != final.y) {
		pos.x += xFact;
		pos.y += yFact;

		ListManager* cols = findColAtPos(pos, col->flag, col->blocks, col->overlaps, col->obj);

		if (cols->nodeCount) {
			Node* n = NULL;
			
			while ((n = listIterate(cols, n)) != NULL) {
				Collision* col2 = (Collision*) n->value;
				CollisionType type = collision_flagsCollides(col, col2);
				collision_callFncs(col, col2, type, true);

				if (collision_addCollide(col, col2)) {
					if (type == COL_BLOCK && !block) {
						target->x = (double) (pos.x - xFact) - col->pos.x;
						target->y = (double) (pos.y - yFact) - col->pos.y;

						block = true;
					}
				}
				else if(collision_removeCollide(col,col2)) {
					collision_callFncs(col, col2, type, false);
				}
			}
		}

		deleteList(cols);
		if (block) {
			break;
		}
	}

	UNLOCK(col, "Check Move END", b);
	return true;
}

vector canMoveTo(Object* obj, int x, int y) {
	logger->inf(LOG_COLLISION, "=== CALL CAN MOVE ===");
	vector res;
	res.x = x;
	res.y = y;

	ListManager* cols = colllision_getContinious(obj);
	if (cols == NULL || !cols->nodeCount) {
		return res;
	}

	listIterateFnc(cols, checkMoveTo, NULL, &res);

	logger->inf(LOG_COLLISION, "=== CAN MOVE END ===");
	return res;
}

short colllision_searchContinious(int i, Node* n, short* delete, void* param, va_list* args) {
	Collision* col = (Collision*) n->value;
	ListManager* lst = (ListManager*) param;

	if (col->enabled && col->continious) {
		addNodeV(lst, "col", col, 0);
	}

	return true;
}

ListManager* colllision_getContinious(Object* obj) {
	if (obj == NULL || obj->collisions == NULL || !obj->collisions->nodeCount) {
		return NULL;
	}

	ListManager* res = initListMgr();
	listIterateFnc(obj->collisions, colllision_searchContinious, NULL, res);

	if (res->nodeCount) {
		return res;
	}

	deleteList(res);
	return NULL;
}
