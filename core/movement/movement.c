#include "movement.h"
#include "../../base/logger.h"

#include "../animation/animation.h"

void applyVelocity(Object* obj);

vector getLookVector(Object* obj, Object* target) {
	return getVector(obj->pos, target->pos);
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

	logger->inf(LOG_MOVE, "==== Applying Velocity DONE ====");
}

void velocityUpdated(Object* obj) {
	animRemoveObject(obj);
	applyVelocity(obj);
}

void _setVelocityX(Object* obj, double x) {
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
}

void _setVelocityY(Object* obj, double y) {
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
}

void setVelocityX(Object* obj, double x) {
	bool b = LOCK(obj, "Set Velocity X-0");
	_setVelocityX(obj, x);
	velocityUpdated(obj);
	UNLOCK(obj, "Set Velocity X-1", b);
}

void setVelocityY(Object* obj, double y) {
	bool b = LOCK(obj, "Set Velocity Y-0");
	_setVelocityY(obj, y);
	velocityUpdated(obj);
	UNLOCK(obj, "Set Velocity Y-1", b);
}

void setVelocity(Object* obj, vector vel) {
	//logger->err(LOG_ANIM, "Lock Set Velocity");
	logger->inf(LOG_MOVE, "==== Setting Velocity ====");
	logger->dbg(LOG_MOVE, "-- Obj: %s", obj->name);
	logger->dbg(LOG_MOVE, "-- vel: X: %d | Y: %d", (int) vel.x, (int) vel.y);

	bool b = LOCK(obj, "Set Velocity-0");

	_setVelocityX(obj, vel.x);
	_setVelocityY(obj, vel.y);

	//logger->err(LOG_ANIM, "Lock Set Velocity");

	logger->inf(LOG_MOVE, "-- Getting Obj Movement: %p", obj->movement);

	Movement* move = &obj->movement;

	logger->inf(LOG_MOVE, "-- Direction: X: %lf | %lf", move->dir.x, move->dir.y);

	logger->inf(LOG_MOVE, "-- UpdatingVelocity");
	velocityUpdated(obj);

	UNLOCK(obj, "Set Velocity-1", b);
	logger->inf(LOG_MOVE, "-- Set Velocity Done");
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
