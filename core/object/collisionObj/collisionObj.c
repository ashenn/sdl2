#include "collisionObj.h"

ColObject* newCollisionObject(char* name, void* comp, SDL_Rect* pos, short z) {
	ColObject* obj = new(ColObject);
	initSimpleObject((Object*) obj, name, comp, pos, z);

	obj->collisions = initListMgr();

	return obj;
}
