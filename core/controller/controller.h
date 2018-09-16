#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include "../object/object.h"

typedef enum ControllerType
{
	CTRL_NULL,
	CTRL_AI,
	CTRL_PLAYER,
	CTRL_GOST
} ControllerType;


typedef struct Controller Controller;
#include "../character/character.h"

#define CONTROLLER_BODY \
	CLASS_BODY \
	bool enabled; \
	\
	Object* obj; \
	Character* character; \
	ControllerType type; \
	\
	bool (*move)(Controller* ctrl, SDL_Rect pos); \
	bool (*moveToObj)(Controller* ctrl, Object* obj); \
	\
	void (*setCharacter)(Controller* ctrl, Character* c); \
	void (*removeCharacter)(Controller* ctrl);


struct Controller {
	CONTROLLER_BODY
};

void initController(Controller* ctrl, ControllerType type);

#endif
