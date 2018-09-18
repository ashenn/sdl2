#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <SDL2/SDL.h>
#include "../../base/math.h"

#define DIRECTION_TYPES(TYPE) \
	TYPE(DIR_NULL) \
	TYPE(DIR_TOP) \
	TYPE(DIR_RIGHT) \
	TYPE(DIR_DOWN) \
	TYPE(DIR_LEFT)

#define GEN_DIRECTION_TYPE_ENUM(ENUM) ENUM,
#define GEN_DIRECTION_TYPE_STRING(STRING) #STRING,


typedef enum DirectionEnum {
	DIRECTION_TYPES(GEN_DIRECTION_TYPE_ENUM)
} DirectionEnum;


static const char* GEN_DIRECTION_TYPE_STRING[] = {
    DIRECTION_TYPES(GEN_DIRECTION_TYPE_STRING)
};

typedef struct Direction
{
	DirectionEnum x;
	DirectionEnum y;
} Direction;

typedef struct Movement
{
	Direction dir;
	vector velocity;
} Movement;

#include "../object/object.h"
void setVelocityY(Object* obj, double y);
void setVelocityX(Object* obj, double y);
void setVelocity(Object* obj, vector vel);

void addVelocity(Object* obj, vector vel);
vector getLookVector(Object* obj, Object* target);

#endif
