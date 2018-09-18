#ifndef _MATH_H__
#define _MATH_H__

#include <math.h>
#include <stdlib.h>
#include <SDL2\SDL.h>

typedef struct vector vector;
struct vector {
	double x;
	double y;
};

double deg2rad(double deg);
double rad2deg(double deg);

double getDistance(double x, double y);
double vectorDistance(vector* vec);

vector sumVector(vector* v1, vector* v2);
vector diffVector(vector* v1, vector* v2);
vector rotateVector(vector* v1, double angle);
vector getVector(SDL_Rect rect1, SDL_Rect rect2);


#endif
