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
	obj->component = surf;
	
	if (obj->texture != NULL) {
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

Object* genSimpleObject(char* name, void* comp, SDL_Rect* pos, short z) {
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
		return ;
	}

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

		Node* childNode = NULL;
		while((childNode = listIterate(obj->childs, childNode)) != NULL) {
			Object* child = (Object*) childNode->value;

			logger->dbg(LOG_OBJ, "-- Child: %s", child->name);
			deleteObject(child);

			logger->dbg(LOG_OBJ, "-- Delete Node");
			deleteNode(obj->childs, childNode->id);
			childNode = NULL;
		}

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

void initSimpleObject(Object* obj, char* name, void* comp, SDL_Rect* pos, short z) {
	obj->z = z;
	obj->visible = 1;
	obj->enabled = 1;
	obj->lifetime = -1;

	obj->name = Str(name);

	obj->component = NULL;
	obj->texture = NULL;

	setObjSurface(obj, comp);

	obj->parent = NULL;
	obj->childs = NULL;
	obj->onDelete = NULL;
	//obj->collision = NULL;

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
}