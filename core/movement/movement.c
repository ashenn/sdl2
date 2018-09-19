#include "movement.h"
#include "../../base/logger.h"

#include "../animation/animation.h"

void applyVelocity(Object* obj);

vector getLookVector(Object* obj, Object* target) {
	return getVector(obj->pos, target->pos);
}

void _setVelocityX(Object* obj, double x) {
	LOCK(obj);
	logger->inf(LOG_MOVE, "==== Setting Velocity X: %d ====", (int) x);
	Movement* move = &obj->movement;
	vector* oVel = &move->velocity;

	oVel->x = x;
	if (oVel->x < 0) {
		move->dir.x = DIR_LEFT;
	}
	else if (oVel->x > 0) {
		move->dir.x = DIR_RIGHT;
	}

	logger->inf(LOG_MOVE, "-- Done X: %d ====", (int) x);
	UNLOCK(obj);
}

void _setVelocityY(Object* obj, double y) {
	LOCK(obj);
	logger->inf(LOG_MOVE, "==== Setting Velocity Y: %d ====", (int) y);
	Movement* move = &obj->movement;
	vector* oVel = &move->velocity;

	oVel->y = y;
	if (oVel->y < 0) {
		move->dir.y = DIR_TOP;
	}
	else if (oVel->y > 0) {
		move->dir.y = DIR_DOWN;
	}

	logger->inf(LOG_MOVE, "-- Done Y: %d ====", (int) y);
	UNLOCK(obj);
}

void refreshVelocity(AnimParam* anim) {
	logger->inf(LOG_MOVE, "==== Refresh Velocity ====");
	Object* obj = (Object*) anim->obj;
	applyVelocity(obj);
}

void applyVelocity(Object* obj) {
	logger->inf(LOG_MOVE, "==== Applying Velocity ====");
	vector* vel = &obj->movement.velocity;

	logger->inf(LOG_MOVE, "Move To: X: %d + %d | Y: %d  + %d", (int) obj->pos.x, (int) vel->x, (int) obj->pos.y, (int) vel->y);
	logger->inf(LOG_MOVE, "Move To: X: %d | Y: %d", (int) (obj->pos.x + vel->x), (int) (obj->pos.y + vel->y));

	AnimMoveParam* anim = (AnimMoveParam*) moveTo(obj, (obj->pos.x + vel->x), (obj->pos.y + vel->y), 0.5f, 0);
	anim->breakOnReach = true;

	if (vel->x || vel->y) {
		anim->callback = refreshVelocity;
	}
}

void velocityUpdated(Object* obj) {
	animRemoveObject(obj);
	applyVelocity(obj);
}

void setVelocityX(Object* obj, double x) {
	_setVelocityX(obj, x);
	velocityUpdated(obj);
}

void setVelocityY(Object* obj, double y) {
	_setVelocityY(obj, y);
	velocityUpdated(obj);
}

void setVelocity(Object* obj, vector vel) {
	LOCK(obj);
	logger->inf(LOG_MOVE, "==== Setting Velocity ====");
	logger->dbg(LOG_MOVE, "-- Obj: %s", obj->name);
	logger->dbg(LOG_MOVE, "-- vel: X: %d | Y: %d", (int) vel.x, (int) vel.y);

	_setVelocityX(obj, vel.x);
	_setVelocityY(obj, vel.y);

	logger->inf(LOG_MOVE, "-- Getting Obj Movement: %p", obj->movement);

	Movement* move = &obj->movement;

	logger->inf(LOG_MOVE, "-- Direction: X: %lf | %lf", move->dir.x, move->dir.y);
	//logger->dbg(LOG_MOVE, "-- Direction: X: %s | Y: %s", GEN_DIRECTION_TYPE_STRING[move->dir.x], GEN_DIRECTION_TYPE_STRING[move->dir.y]);

	velocityUpdated(obj);
	logger->inf(LOG_MOVE, "-- UpdatingVelocity");
	UNLOCK(obj);
}

void addVelocity(Object* obj, vector vel) {
	logger->inf(LOG_MOVE, "==== Adding Velocity ====");
	logger->dbg(LOG_MOVE, "-- Obj: %s", obj->name);
	logger->dbg(LOG_MOVE, "-- vel: X: %d | Y: %d", (int) vel.x, (int) vel.y);

	vector resV;
	Movement* move = &obj->movement;
	vector* oVel = &move->velocity;

	resV.x = oVel->x + vel.x;
	resV.y = oVel->y + vel.y;

	setVelocity(obj, resV);
}
