#ifndef COLLISION_H
#define COLLISION_H

#include "../object/object.h"


#define COL_TYPES(TYPE) \
	TYPE(COL_IGNORE) \
	TYPE(COL_OVERLAP) \
	TYPE(COL_BLOCK)

#define GEN_COL_TYPE_ENUM(ENUM) ENUM,
#define GEN_COL_TYPE_STRING(STRING) #STRING,

typedef enum CollisionType {
	COL_TYPES(GEN_COL_TYPE_ENUM)
} CollisionType;

static const char* COL_TYPE_STRING[] = {
    COL_TYPES(GEN_COL_TYPE_STRING)
};


typedef struct Collision
{
	CLASS_BODY
	SDL_Rect pos;

	bool enabled;
	bool continious;

	unsigned int flag;
	unsigned int blocks;
	unsigned int overlaps;

	Object* obj;
	CollisionType type;
	ListManager* collisions;

	void (*onHit)(Object*, Object*);
	void (*onHitEnd)(Object*, Object*);
	void (*onOverlap)(Object*, Object*);
	void (*onOverlapEnd)(Object*, Object*);
} Collision;


void collision_init();
ListManager* collision_getFlags();
ListManager* collision_getObjects();
Collision* collision_getByName(Object* obj, const char* name);

Collision* collision_add(Object* obj, const char* name, CollisionType type, unsigned int flag, SDL_Rect pos, bool enabled);

void collision_handleIterate();
void* collision_handle(void* arg);

const char* collision_getFlagName(unsigned int id);
const char* collision_getChannelName(const unsigned int id);

ListManager* collision_getChannel(const unsigned int id);
ListManager* collision_getChannelByName(const char* name);
unsigned int collision_getFlagValue(const char* flag);
CollisionType collision_getTypeByName(const char* type);
bool collision_flagsMatch(const char* flag1, const char* flag2);

ListManager* colllision_getContinious(Object* obj);

#define COL_NONE 	collision_getFlagValue("COL_NONE")
#define COL_ALL 	collision_getFlagValue("COL_ALL")
#define COL_WALL 	collision_getFlagValue("COL_WALL")
#define COL_ITEM 	collision_getFlagValue("COL_ITEM")
#define COL_PLAYER 	collision_getFlagValue("COL_PLAYER")


vector canMoveTo(Object* obj, int x, int y);

#endif