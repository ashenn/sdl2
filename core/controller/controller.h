#ifndef OBJECT_H
#define OBJECT_H

#include <SDL2/SDL.h>
#include "../object/object.h"
#include "../character/character.h"

typedef enum ControllerType
{
	AI_CTRL,
	PLAYER_CTRL,
	VISUAL_CTRL
} ControllerType;


typedef struct Controller Controller;

#define CONTROLLER_BODY \
	CLASS_BODY \
	bool enabled; \
	\
	ControllerType* controller; \
	Character* character \
	\
	void (*move)(Controller* ctrl, SDL_Rect pos); \
	void (*moveToObj)(Controller* ctrl, Object* obj); \
	\
	void (*setCharacter)(Controller* ctrl, Character* c); \
	void (*removeCharacter)(Controller* ctrl, Character* c);


struct Controller {
	CONTROLLER_BODY
};


#endif
