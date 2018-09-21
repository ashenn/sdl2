#include "object.h"
#include "../view/view.h"
#include "../view/layer/layer.h"
#include "../animation/animation.h"


ListManager* getObjectList() {
	static ListManager* objects = NULL;

	if (objects != NULL){
		return objects;
	}

	logger->inf(LOG_OBJ, "==== Init Object List ====");
	objects = initListMgr();

	return objects;
}

void setObjSurface(Object* obj, SDL_Surface* surf) {
	logger->inf(LOG_OBJ, "-- Setting Obj Surface");
	obj->component = surf;

	if (obj->texture != NULL) {
		logger->inf(LOG_OBJ, "-- Destroyibg Old Texture");
		SDL_DestroyTexture(obj->texture);
		obj->texture = NULL;
	}

	if (surf != NULL) {
		SDL_SetColorKey( surf, SDL_TRUE, SDL_MapRGB( surf->format, 0, 255, 255 ) );
		obj->texture = SDL_CreateTextureFromSurface(getRenderer(), surf);

		if (obj->texture == NULL) {
		    logger->err(LOG_OBJ, "Fail To GEN TEXTURE");
		    logger->err(LOG_OBJ, "%s", SDL_GetError());
		    assert(0);
		}
	}
}

Object* genSimpleObject(const char* name, void* comp, SDL_Rect* pos, short z) {
	Object* obj = new(Object);

	initSimpleObject(obj, name, comp, pos, z);

	logger->dbg(
        LOG_OBJ,
		"--name: %s\n--z-index: %d\n--pos: x: %d | y:%d | w:%d | h:%d",
		obj->name,
		obj->z,
		//(obj->click != NULL),
		//(obj->hover != NULL),
		obj->pos.x,
		obj->pos.y,
		obj->pos.w,
		obj->pos.h
	);

	return obj;
}

Object* genObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover) {
	Object* obj = genSimpleObject(name, comp, pos, z);

	//obj->click = click;
	//obj->hover = hover;

	return obj;
}

void addObjectToView(Object* obj) {
	ListManager* objects = getObjectList();

	logger->dbg(LOG_OBJ, "-- Adding Node");
	Node* n = addNodeV(objects, obj->name, obj, 1);

	if (n == NULL) {
		logger->err(LOG_OBJ, "==== Fail to insert object in list ====");
		return;
	}
	logger->dbg(LOG_OBJ, "-- Adding TEST");

	logger->dbg(LOG_OBJ, "-- nodeID: %d", n->id);
	obj->id = n->id;

	logger->dbg(LOG_OBJ, "-- Setting Layer");
	setObjectLayer(obj, obj->z);
	logger->dbg(LOG_OBJ, "-- Added To Layer");
}

Object* addObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container) {
	logger->inf(LOG_OBJ, "=== Adding Object %s ===", name);
	Object* obj = genObject(name, comp, pos, z, click, hover);

	addObjectToView(obj);

	logger->dbg(LOG_OBJ, "==== Object %s Added ====", obj->name);

	return obj;
}

Object* addSimpleObject(char* name, void* comp, SDL_Rect* pos, short z) {
	Object* obj = genSimpleObject(name, comp, pos, z);
	addObjectToView(obj);

	return obj;

}

void clearObjects() {
	Node* n = NULL;
	logger->inf(LOG_OBJ, "==== CLEARING Objects ====");
	logger->dbg(LOG_OBJ, "-- Getting Objects");

	ListManager* objects = getObjectList();

	if (objects != NULL){
		logger->dbg(LOG_OBJ, "-- Len: %d", objects->nodeCount);

		while((n = listIterate(objects, n)) != NULL) {
			logger->dbg(LOG_OBJ, "-- Deleting");
			logger->dbg(LOG_OBJ, "-- Deleting: %s", n->name);

		    deleteObject(n->value);
		    n = NULL;
		}
	}
}

void deleteObject(Object* obj) {
	logger->inf(LOG_OBJ, "===== Deleting Object ====");
	logger->dbg(LOG_OBJ, "-- Name: %s", obj->name);
	Node* layer = getNode(getLayers(), obj->z);

	if (obj->onDelete != NULL) {
		logger->dbg(LOG_OBJ, "-- Calling On Delete Func");
		obj->onDelete(obj);
	}

	if (obj->childs != NULL) {
		logger->dbg(LOG_OBJ, "-- Delete Object Childs");


		/*
		Node* childNode = NULL;
		while((childNode = listIterate(obj->childs, childNode)) != NULL) {
			Object* child = (Object*) childNode->value;

			logger->dbg(LOG_OBJ, "-- Child: %s", child->name);
			deleteObject(child);

			logger->dbg(LOG_OBJ, "-- Delete Node");
			deleteNode(obj->childs, childNode->id);
			childNode = NULL;
		}
		*/

		deleteList(obj->childs);
		logger->dbg(LOG_OBJ, "-- Childs Cleared");
	}

	if (layer != NULL) {
		logger->dbg(LOG_OBJ, "-- Deleting From : %s", layer->name);
		deleteNode(layer->value, obj->id);

		logger->dbg(LOG_OBJ, "-- Remove From Anim");
		animRemoveObject(obj);
	}

	/*if (obj->container != NULL) {
		deleteContainer(obj->container, obj->containerType);
		obj->container = NULL;
	}*/

	/*if (obj->collision != NULL) {
		ListManager* hitObjects = getHitObjectList();
		logger->dbg(LOG_OBJ, "-- Removing collision: %s", obj->name);
		deleteNode(hitObjects, obj->collision->id);

		free(obj->collision);
	}*/

	logger->dbg(LOG_OBJ, "-- Delete Object");
	free(obj->name);
	obj->name = NULL;

	ListManager* objects = getObjectList();
	deleteNode(objects, obj->id);

	logger->dbg(LOG_OBJ, "===== DELETE OBJECT DONE ====");
}

void initSimpleObject(Object* obj, const char* name, void* comp, SDL_Rect* pos, short z) {
	logger->err(LOG_OBJ, "-- INIT SIMPLE OBJECT %s", name);
	obj->z = z;
	obj->visible = 1;
	obj->enabled = 1;
	obj->lifetime = -1;

	logger->inf(LOG_OBJ, "-- Set Name");

	int len = strlen(name) + 8;
	logger->err(LOG_OBJ, "-- TestLen: %d", strlen(name));
	logger->err(LOG_OBJ, "-- Len: %d", len);
	obj->name = StrE(len);
	snprintf(obj->name, len, "%s_Object", name);


	logger->inf(LOG_OBJ, "-- Set Velocity");
	vector vel = {0, 0};
	setVelocity(obj, vel);

	obj->component = NULL;
	obj->texture = NULL;

	logger->inf(LOG_OBJ, "-- Set Surface");
	setObjSurface(obj, comp);
	obj->rotation = 0;
	obj->flip = FLIP_N;

	obj->clip = NULL;
	obj->parent = NULL;
	obj->childs = NULL;
	obj->onDelete = NULL;
	obj->delete = NULL;

	obj->collisions = NULL;

	if (pos != NULL) {
		obj->pos.x = pos->x;
		obj->pos.y = pos->y;
		obj->pos.w = pos->w;
		obj->pos.h = pos->h;
	}
	else {
		obj->pos.x = 0;
		obj->pos.y = 0;
		obj->pos.w = SCREEN_W;
		obj->pos.h = SCREEN_H;
	}

	logger->inf(LOG_OBJ, "-- Simple Object Ready");
}

void deleteChild(Node* n) {
	Object* child = (Object*) n->value;
	logger->war(LOG_OBJ, "-- Delete Child");
	logger->war(LOG_OBJ, "-- Child Name: %s", child->name);

	deleteObject(child);
}

void removeChild(Object* obj, const char* name) {
	logger->war(LOG_OBJ, "-- Removing Child: %s", name);
	Node* n = getNodeByName(obj->childs, name);

	if (n == NULL) {
		logger->war(LOG_OBJ, "-- Child Not Found: %s", name);
	}

	deleteNodeByName(obj->childs, name);
}

bool addChild(Object* obj, Object* child) {
	//logger->err(LOG_ANIM, "Lock Add Child");
	LOCK(obj, "ADD CHILD-0");
	//logger->err(LOG_ANIM, "Lock Child");
	LOCK(child, "ADD CHILD-1");

	logger->inf(LOG_OBJ, "==== Adding Child %s ====", child->name);

	logger->dbg(LOG_OBJ, "-- parent: %s\n-- child: %s", obj->name, child->name);

	if (obj->childs == NULL) {
		logger->dbg(LOG_OBJ, "-- Init child list");
		obj->childs = initListMgr();
	}


	logger->dbg(LOG_OBJ, "-- Child Pos: x: %d + %d, y: %d + %d", child->pos.x, obj->pos.x, child->pos.y, obj->pos.y);
	child->pos.x += obj->pos.x;
	child->pos.y += obj->pos.y;
	logger->dbg(LOG_OBJ, "-- Child Pos: x: %d, y: %d", child->pos.x, child->pos.y);

	Node* n = addNodeV(obj->childs, child->name, child, 1);
	if (n == NULL) {
		logger->err(LOG_OBJ, "==== FAIL TO ADD CHILD NODE =====");
		//logger->err(LOG_ANIM, "UnLock Add Child");
		UNLOCK(obj, "ADD CHILD-2");
		//logger->err(LOG_ANIM, "UnLock Child");
		UNLOCK(child, "ADD CHILD-3");

		return false;
	}

	if (obj->parent == NULL) {
		child->parent = obj;
	}
	else{
		child->parent = obj->parent;
	}

	logger->dbg(LOG_OBJ, "-- nodeID: %d", n->id);
	logger->dbg(LOG_OBJ, "==== Child %s Added to %s ====", child->name, obj->name);

	n->del = deleteChild;

	//logger->err(LOG_ANIM, "UnLock Add Child");
	UNLOCK(obj, "ADD CHILD-4");
	//logger->err(LOG_ANIM, "UnLock Child");
	UNLOCK(child, "ADD CHILD-5");

	return true;
}

SDL_Rect getWorldPos(Object* obj, SDL_Rect pos) {
	SDL_Rect res;
	res.x = obj->pos.x + pos.x;
	res.y = obj->pos.y + pos.y;

	res.w = pos.w;
	res.h = pos.h;

	return res;
}
