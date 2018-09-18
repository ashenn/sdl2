#ifndef CHARACTER_H
#define CHARACTER_H

#include "../../common.h"
#include "../../base/math.h"

typedef enum CharacterType
{
	CHAR_PLAYER,
	CHAR_MONSTER,
	CHAR_BOSS
} CharacterType;

typedef struct Character Character;
#include "./charAttr.h"
#include "../controller/controller.h"

#include "../object/spriteObj/character/charObj.h"

#define CHARACTER_BODY \
	CLASS_BODY \
	\
	Object* obj; \
	Controller* ctrl; \
	vector direction; \
	CharacterType type; \
	\
	CharAttr attr; \
	\
	void (*onPossess)(Character* c, Controller* ctrl); \
	void (*onUnPossess)(Character* c, Controller* ctrl);

struct Character {
	CHARACTER_BODY
};

void characterDelete(Character* ch);
Character* initCharacter(CharacterType type, char* name, SDL_Rect* pos, int z);

#endif
