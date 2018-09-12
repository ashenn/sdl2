#ifndef OBJECT_H
#define OBJECT_H

#include "../../base/basic.h"
#include "../../common.h"

#include<SDL2\SDL.h>

typedef struct Object Object;
typedef struct Controller Controller;

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
	Controller* controller;		\
	\
	Object* parent;             \
	ListManager* childs;		\
	\
	double rotation;			\
	SDL_Texture* texture;		\
	SDL_RendererFlip flip;		\
	SDL_Surface* component;		\
	\
	void (*onDelete)(Object* obj);
    // Collision* Collision;


struct Object
{
	OBJECT_BODY
};

// CREATING
ListManager* getObjectList();	// List Of all visual Objects
Object* genSimpleObject(char* name, void* comp, SDL_Rect* pos, short z);
Object* genObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover);

void initSimpleObject(Object* obj, char* name, void* comp, SDL_Rect* pos, short z);

Object* addObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container);
Object* addSimpleObject(char* name, void* comp, SDL_Rect* pos, short z); // Add Object No Click / Hover / Container

void addObjectToView(Object* obj);


// DELETING
void clearObjects();
void deleteObject(Object* obj);

#endif
