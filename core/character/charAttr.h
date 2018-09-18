#ifndef CHAR_ATTR_H
#define CHAR_ATTR_H

typedef struct CharAttr
{
	bool inAir;
	bool doubleJump;
	bool hasDoubleJump;
	bool canDoubleJump;

	bool crouch;
	bool moving;

	unsigned int moveSpeed;
	unsigned int maxMoveSpeed;
} CharAttr;

#endif