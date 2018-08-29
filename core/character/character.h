#ifndef CHARACTER_H
#define CHARACTER_H

#include "../../common.h"
#include "../../base/math.h"

typedef struct Character Character;

#include "../object/object.h"
#include "../controller/controller.h"


#define CHARACTER_BODY \
	CLASS_BODY \
	\
	Object* obj; \
	Controller* ctrl; \
	vector* direction; \
	\
	void (*onPossess)(Character* c, Controller* ctrl); \
	void (*onUnPossess)(Character* c, Controller* ctrl); \

struct Character {
	CHARACTER_BODY
};

#endif
