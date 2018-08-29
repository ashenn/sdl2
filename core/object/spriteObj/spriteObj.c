#include "./spriteObj.h"


SpriteObject* newSpriteObject(char* name, void* comp, SDL_Rect* pos, short z, SDL_Rect clip) {
	SpriteObject* obj = new(SpriteObject);
	initSimpleObject((Object*)obj, name, comp, pos, z);

	obj->clip = clip;
	obj->clipList = initListMgr();
	return obj;
}
