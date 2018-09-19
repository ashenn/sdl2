#ifndef OBJECT_H
#define OBJECT_H

#include "../../common.h"
#include "../../base/basic.h"

#include<SDL2\SDL.h>

typedef struct Object Object;
typedef struct Controller Controller;

#include "../movement/movement.h"

#define OBJECT_BODY \
	CLASS_BODY 		\
	\
	short z;     	    \
	short lifetime; 	\
	\
	bool visible;		\
	bool enabled;		\
	unsigned int color;		\
	\
	SDL_Rect pos;		\
	SDL_Rect* clip; \
	\
	double rotation;			\
	Movement movement;			\
	Controller* controller;		\
	\
	Object* parent;             \
	ListManager* childs;		\
	\
	SDL_Texture* texture;		\
	SDL_RendererFlip flip;		\
	SDL_Surface* component;		\
	\
	void (*delete)(Object* obj);  \
	void (*onDelete)(Object* obj); \
	\
	ListManager* collisions;


struct Object
{
	OBJECT_BODY
};


// CREATING
ListManager* getObjectList();	// List Of all visual Objects
Object* genSimpleObject(const char* name, void* comp, SDL_Rect* pos, short z);
Object* genObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover);

void initSimpleObject(Object* obj, const char* name, void* comp, SDL_Rect* pos, short z);

Object* addObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container);
Object* addSimpleObject(char* name, void* comp, SDL_Rect* pos, short z); // Add Object No Click / Hover / Container


void addObjectToView(Object* obj);
void setObjSurface(Object* obj, SDL_Surface* surf);

// CHILDS
bool addChild(Object* obj, Object* child);


// DELETING
void clearObjects();
void deleteObject(Object* obj);

#endif
