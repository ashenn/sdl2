#include "layer.h"

ListManager* getLayers() {
	static ListManager* layerList = NULL;
	if (layerList != NULL) {
		return layerList;
	}

	layerList = initListMgr();
	return layerList;
}

void removeObjectFromLayers(Object* obj) {
	Node* LayerNode = NULL;
	ListManager* layers = getLayers();

	bool found = false;

	while (!found && (LayerNode = listIterate(layers, LayerNode)) != NULL) {
		ListManager* layer = (ListManager*) LayerNode->value;

		Node* objNode = NULL;
		while ((objNode = listIterate(layer, objNode)) != NULL) {
			if (objNode->value == obj) {
				deleteNode(layer, objNode->id);
				found = true;
				break;
			}
		}
	}
}

bool setObjectLayer(Object* obj, short z) {
	logger->inf(LOG_LAYER, "==== Setting Object %s To Layer %d ====", obj->name, z);

	obj->z = z;

	removeObjectFromLayers(obj);

	ListManager* layers = getLayers();
	Node* layer = getNode(layers, z);

	if (layer == NULL){
		char layerName[12];
		snprintf(layerName, 12, "#Layer-%d", z);

		logger->inf(LOG_LAYER, "-- Creating Layer: %s", layerName);

		layer = addNodeV(layers, layerName, initListMgr(), 0);

		if (layer == NULL){
			logger->err(LOG_LAYER, "==== FAIL TO ADD LAYER: %s ====", layerName);
			return false;
		}

		layer->id = z;
	}

	ListManager* layerObjs = (ListManager*)layer->value;

	logger->inf(LOG_LAYER, "-- Adding Object To Loayer");
	Node* objNode = addNodeV(layerObjs, obj->name, obj, 0);

	logger->inf(LOG_LAYER, "-- nodeID: %d", objNode->id);
	objNode->id = obj->id;

	logger->inf(LOG_LAYER, "==== Object %s Added To Layer: %s ====", obj->name, layer->name);

	return true;
}
