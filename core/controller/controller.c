#include "controller.h"
#include "../animation/animation.h"

bool move(Controller* ctrl, SDL_Rect pos) {
	if (!ctrl->enabled || ctrl->character == NULL) {
		return false;
	}

	Character* ch = ctrl->character;
	Object* obj = ctrl->character->obj;


	logger->inf(LOG_CONTROLLER, "MOVING TO: X: %d | Y: %d", pos.x, pos.y);
	logger->inf(LOG_CONTROLLER, "MOVING FROM: X: %d | Y: %d", obj->pos.x, obj->pos.y);

	if (obj->pos.x == pos.x && obj->pos.y == pos.y) {
		logger->inf(LOG_CONTROLLER, "-- Already At Pos");
		return true;
	}

	vector v = getVector(obj->pos, pos);

	logger->inf(LOG_CONTROLLER, "Direction X: %lf | Y: %lf", v.x, v.y);

	int speedX = ch->attr.maxMoveSpeed;

	if (abs(v.x) <= speedX) {
		speedX = v.x;
	}
	else if (v.x < 0) {
		speedX = speedX * -1;
	}

	int speedY = ch->attr.maxMoveSpeed;
	if (abs(v.y) <= speedY) {
		speedY = v.y;
	}
	else if (v.y < 0) {
		speedY = speedY * -1;
	}

	logger->inf(LOG_CONTROLLER, "Move Speed X: %d | Y: %d", speedX, speedY);

	CharObj* chObj = (CharObj*) obj;
	chObj->ch->attr.crouch = false;
	chObj->ch->attr.moving = true;

	vector vel;
	vel.x = speedX;
	vel.y = speedY;

	setVelocity(obj, vel);

	return true;
}

void moveDir(Controller* ctrl, DirectionEnum dir) {
	logger->err(LOG_CONTROLLER, "=== MOVE DIR ===");

	if (ctrl == NULL) {
        logger->err(LOG_CONTROLLER, "-- Controller Is NULL");
        return;
	}

	vector vel = {0, 0};

	if (DIR_NULL) {
        logger->err(LOG_CONTROLLER, "-- Dir Is NULL");
		setVelocity(ctrl->obj, vel);
		return;
	}

	Character* ch = ctrl->character;
	LOCK(ctrl);
	LOCK(ch);

	short fact = dir == DIR_RIGHT || dir == DIR_DOWN ? 1 : -1;
	logger->err(LOG_CONTROLLER, "-- Factor: %d", fact);
	logger->err(LOG_CONTROLLER, "-- Max: %d", ch->attr.maxMoveSpeed);

	if (dir == DIR_RIGHT || dir == DIR_LEFT) {
		vel.x = (double) ((int) ch->attr.maxMoveSpeed * ((int) fact));
	}
	else{
		vel.y = (double) ((int) ch->attr.maxMoveSpeed * ((int) fact));
	}

	logger->err(LOG_CONTROLLER, "-- Set Velocity");
	logger->err(LOG_CONTROLLER, "-- X: %d", (int) vel.x);
	logger->err(LOG_CONTROLLER, "-- Y: %d", (int) vel.y);

	ch->attr.moving = true;
	setVelocity(ctrl->obj, vel);
	
	UNLOCK(ch);
	UNLOCK(ctrl);
}

void stopMovement(Controller* ctrl) {
	vector vel = {0, 0};
	setVelocity(ctrl->obj, vel);
	ctrl->character->attr.moving = 0;
}

void ctrlSetCharacter(Controller* ctrl, Character* c) {
	logger->inf(LOG_CONTROLLER, "===== Setting Controller Character =====");
	if (ctrl->character != NULL) {
		logger->inf(LOG_CONTROLLER, "-- Removing Old Character: %s", ctrl->name);
		logger->inf(LOG_CONTROLLER, "-- Removing Old Character: %s | %p", ctrl->character);

		ctrl->removeCharacter(ctrl);
	}

	logger->inf(LOG_CONTROLLER, "-- Get Char Obj");
	CharObj* obj = (CharObj*) c->obj;
	
	logger->inf(LOG_CONTROLLER, "-- Setting Character");
	ctrl->character = c;

	logger->inf(LOG_CONTROLLER, "-- Setting Object");
	ctrl->obj = (Object*) obj;
	
	logger->inf(LOG_CONTROLLER, "-- Setting Controller-0");
    c->ctrl = ctrl;

	logger->inf(LOG_CONTROLLER, "-- Setting Controller-1");
    obj->ctrl = ctrl;
}

void ctrlRemoveCharacter(Controller* ctrl) {
	characterDelete(ctrl->character);
	ctrl->character = NULL;
}

void initController(Controller* ctrl, ControllerType type) {
	ctrl->type = type;
	ctrl->enabled = true;

	ctrl->move = move;
	ctrl->moveToObj = NULL;
	ctrl->character = NULL;
	ctrl->setCharacter = ctrlSetCharacter;
	ctrl->removeCharacter = ctrlRemoveCharacter;
}
