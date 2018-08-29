#ifndef LAYER_H
#define LAYER_H

#include "../../object/object.h"
#include "../../../base/libList.h"

ListManager* getLayers();
bool setObjectLayer(Object* obj, short z);

#endif
