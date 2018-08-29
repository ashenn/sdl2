#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include "../object.h"

#define COLLISION_OBJ_BODY	\
	OBJECT_BODY	\
	ListManager* collisions;

typedef struct ColObject {
	COLLISION_OBJ_BODY
} ColObject;

#endif