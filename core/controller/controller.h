#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include "../object/object.h"

typedef enum ControllerType
{
	AI_CTRL,
	PLAYER_CTRL,
	VISUAL_CTRL
} ControllerType;


typedef struct Controller Controller;
#include "../character/character.h"

#define CONTROLLER_BODY \
	CLASS_BODY \
	bool enabled; \
	\
	Character* character; \
	ControllerType* controller; \
	\
	bool (*move)(Controller* ctrl, SDL_Rect pos); \
	bool (*moveToObj)(Controller* ctrl, Object* obj); \
	\
	void (*setCharacter)(Controller* ctrl, Character* c); \
	void (*removeCharacter)(Controller* ctrl, Character* c);


struct Controller {
	CONTROLLER_BODY
};


#endif
