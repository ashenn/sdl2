#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include "../object.h"
#include "../../collision/collision.h"

#define COLLISION_OBJ_BODY	\
	OBJECT_BODY

typedef struct ColObject {
	COLLISION_OBJ_BODY
} ColObject;

#endif
