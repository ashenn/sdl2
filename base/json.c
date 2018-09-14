#include "json.h"
#include "../lib/jsmn/jsmn.h"

#include <assert.h>
#include <stdarg.h>

void jsonFill(Json* json, char* data) {
	logger->inf(LOG_JSON, "==== FILLING JSON ====");

	jsmn_parser parser;
	logger->dbg(LOG_JSON, "-- DATA: \n%s", data);

	jsmn_init(&parser);
	int tokenCnt = jsmn_parse(&parser, data, strlen(data), NULL, 1500);

	jsmn_init(&parser);
	jsmntok_t tokens[tokenCnt];

	int res = jsmn_parse(&parser, data, strlen(data), tokens, tokenCnt);

	if (res < 0) {
        switch (res) {
            case JSMN_ERROR_INVAL:
                logger->err(LOG_ASSET, "JSON CORRUPTED");
                break;

            case JSMN_ERROR_NOMEM:
                logger->err(LOG_ASSET, "Not Enought TOKENS");
                break;

            case JSMN_ERROR_PART:
                logger->err(LOG_ASSET, "JSON string is too short, expecting more JSON data");
                break;

            default:
                logger->err(LOG_ASSET,"-- UNKNOWN Error !!!!");
                break;
        }

        return;
	}

	int index = 0;
	short isKey = 1;
	short inArray = 0;

	char key[1500];
	char buffer[1500];

	json->childCount = tokens[0].size;
	json->type = JSON_OBJECT;

	for (int i = 1; i < res; ++i) {

		memset(buffer, 0, 1500);
		memcpy(buffer, &data[tokens[i].start], tokens[i].end - tokens[i].start);

		if (isKey && !inArray) {
			isKey = 0;
			strcpy(key, buffer);
		}
		else{
			float val = 0;
			if (inArray) {
				snprintf(key, 100, "%d", index);
			}

			switch (tokens[i].type) {
				case JSMN_UNDEFINED:
					jsonSetValue(json, key, NULL, JSON_NULL);
					break;

				case JSMN_ARRAY:
					inArray = 1;
					index = 0;

					//logger->dbg(LOG_ASSET, "-- Type: Array: %d", tokens[i].size);
					json = jsonSetValue(json, key,  initListMgr(), JSON_ARRAY);
					json->type = JSON_ARRAY;
					json->childCount = tokens[i].size;

					//strcpy(key, buffer);
					continue;
					break;

				case JSMN_OBJECT:
					isKey = 1;
					index = 0;

					//logger->dbg(LOG_ASSET, "-- Type: Object: %d", tokens[i].size);
					json = jsonSetValue(json, key,  initListMgr(), JSON_OBJECT);
					json->type = JSON_OBJECT;
					json->childCount = tokens[i].size;

					//strcpy(key, buffer);
					continue;
					break;


				case JSMN_STRING:
					//logger->err(LOG_ASSET, "-- Type: String");
					jsonSetValue(json, key, buffer, JSON_STRING);
					break;

				case JSMN_PRIMITIVE:
					//logger->err(LOG_ASSET, "-- Type: Primitive");

					if (buffer[0] == 'n') {
						jsonSetValue(json, key, NULL, JSON_NULL);
					}
					else if(buffer[0] == 't') {
						val = 1;
						jsonSetValue(json, key, &val, JSON_NUM);
					}
					else if(buffer[0] == 'f') {
						val = 0;
						jsonSetValue(json, key, &val, JSON_NUM);
					}
					else {
						val = (float) str2int(buffer);
						jsonSetValue(json, key, &val, JSON_NUM);
					}
					//logger->err(LOG_JSON, "TEST: %f", val);
					break;
			}

			if (!inArray) {
				isKey = 1;
			}

			while (json->id >= 0 && (inArray || json->type == JSON_OBJECT)) {
				// logger->err(LOG_ASSET, "-- Array CHECK");

				// logger->err(LOG_ASSET, "-- Count: %d / %d", index+1, json->childCount);

				if (index+1 >= json->childCount) {
					// logger->inf(LOG_JSON, "-- Back To Parent");

					if (json->parent == NULL) {
						// logger->inf(LOG_JSON, "-- Parent IS NULL");
					}

					// logger->inf(LOG_JSON, "-- Become Parent: %s", json->parent->key);
					json = json->parent;

					// logger->inf(LOG_JSON, "-- Check Count");
					index = json->childs->nodeCount-1;

					// logger->inf(LOG_JSON, "-- Set Is Key");
					isKey = 1;

					// logger->inf(LOG_JSON, "-- Parent Type");
					inArray = json->type == JSON_ARRAY;

					// logger->inf(LOG_JSON, "-- End Of Container Reached");
				}
				else{
					break;
				}
			}

			/*
				if (isKey) {
					logger->inf(LOG_JSON, "-- Next Key");
				}
				else{
					logger->inf(LOG_JSON, "-- Next Value");
				}
			*/

			index++;
		}
	}

	//jsonPrint(json, 0);
}

Json* newJson() {
	//logger->inf(LOG_JSON, "==== Creating New Json ===");

	Json* json = malloc(sizeof(Json));
	json->id = -1;
	json->num = 0;
	json->key = NULL;
	json->string = NULL;

	json->parent = NULL;
	json->childs = NULL;
	json->type = JSON_NULL;

	//logger->dbg(LOG_JSON, "-- Json Ready");
	return json;
}

Json* loadJsonFile(char* path) {
	validatePath(path);
	logger->inf(LOG_JSON, "==== New Json From Path: %s ===", path);

	char* content = fileGetContent(path);
	if (content == NULL) {
		logger->err(LOG_JSON, "Fail to Load Json File: %s", path);
		return NULL;
	}

	Json* json = newJson();
	jsonFill(json, content);

	return json;
}

Json* jsonSetValue(Json* json, char* key, void* value, JsonDataEnum type) {
	logger->inf(LOG_JSON, "=== Setting Json %s Key: %s | type: %s", json->key, key, GEN_JSON_TYPE_STRING[type]);

	if (json == NULL) {
		logger->err(LOG_JSON, "Trying to Set Json Value But Json Is Null !!!");
		return NULL;
	}
	else if (key == NULL || !strlen(key)) {
		logger->err(LOG_JSON, "Trying to Set Json Value But Key Is Empty !!!");
		return NULL;
	}

	if (json->type == JSON_NULL) {
		json->type = JSON_OBJECT;
	}
	else if(json->type != JSON_ARRAY && json->type != JSON_OBJECT){
		logger->war(LOG_JSON, "Trying to Set Json Value But JSON IS NOT ARRAY OR OBJECT !!!");
		json->type = JSON_OBJECT;
	}

	if (value == NULL && type != JSON_NULL) {
		logger->war(LOG_JSON, "Trying to Set Json Value As NULL But Type is not JSON_NULL !!!");
		type = JSON_NULL;
	}

	Json* child = NULL;
	Node* childNode = NULL;

	if (json->childs == NULL) {
		logger->dbg(LOG_JSON, "-- Init Child List");
		json->childs = initListMgr();

		logger->dbg(LOG_JSON, "-- Adding Key To Parent");
		childNode = addNode(json->childs, key);
	}
	else{
		childNode = getNodeByName(json->childs, key);

		if (childNode != NULL) {
			logger->dbg(LOG_JSON, "-- Replacing Existing Child");
			child = (Json*) childNode->value;
			child->parent = NULL;

			deleteJson(child);
		}
		else {
			childNode = addNode(json->childs, key);
		}
	}

	child = newJson(NULL);
	child->id = childNode->id;
	childNode->value = (void*) child;


	child->type = type;
	child->parent = json;
	child->key = Str(key);

	switch (type) {
		case JSON_NULL:
			logger->dbg(LOG_JSON, "-- Setting Null Value");
			break;

		case JSON_STRING:
			logger->dbg(LOG_JSON, "-- Setting String Value");
			child->string = Str((char*) value);
			break;

		case JSON_NUM:
			logger->dbg(LOG_JSON, "-- Setting Numeric Value");
			child->num = *((float*) value);
			break;

		case JSON_ARRAY:
		case JSON_OBJECT:
			logger->dbg(LOG_JSON, "-- Prepare Child->Child List");
			child->childs = (ListManager*) value;
			break;
	}

	logger->dbg(LOG_JSON, "==== Setting Key: %s Done ====", key);
	return child;
}

short jsonClearChilds(int i, Node* n, short* delete, void* param, va_list* args) {
	logger->inf(LOG_JSON, "-- Child Delete: #%d => %s", i, n->name);

	if (n->value == NULL) {
		logger->war(LOG_JSON, "-- Deleting NULL Json Child");
		*delete = true;
		return true;
	}

	logger->dbg(LOG_JSON, "-- Remove Parent Json Reference");
	Json* json = (Json*) n->value;
	json->parent = NULL;

	logger->dbg(LOG_JSON, "-- Deleting Json");
	deleteJson(json);

	return true;
}

void deleteJson(Json* json) {
	logger->inf(LOG_JSON, "==== Deleting Json: #%d => %s ===", json->id, json->key);

	if (json->string != NULL) {
		logger->inf(LOG_JSON, "-- Free String Data Json");
		free(json->string);
		json->string = NULL;
	}

	if (json->parent != NULL) {
		logger->inf(LOG_JSON, "-- Removing From Parent Json");
		deleteNodeByName(json->parent->childs, json->key);
	}

	if (json->childs != NULL) {
		logger->inf(LOG_JSON, "-- Removing Json Childs");
		listIterateFnc(json->childs, jsonClearChilds, NULL, NULL);
		deleteList(json->childs);
		json->childs = NULL;
	}

	free(json);
}

short jsonPrintData(int i, Node* n, short* delete, void* param, va_list* args) {
	//logger->inf(LOG_JSON, "=== TEST-0");
	Json* json = (Json*) n->value;

	//logger->inf(LOG_JSON, "=== TEST-1");

	int tab = *((int*) param);
	//logger->inf(LOG_JSON, "=== TEST-1.2");

	int curTab = tab+1;
	//logger->inf(LOG_JSON, "=== Cur Tab: %d", curTab);

	if (json == NULL) {
		logger->err(LOG_JSON, "=== JSON IS NULL");
		return true;
	}

	char* key;
	char tmpFormat[250];
	memset(tmpFormat, 0, 250);
	//logger->inf(LOG_JSON, "=== TEST-1.1");

	char format[250];
	memset(format, 0, 250);
//	snprintf(format, 250, "");

	if (json->key != NULL && json->parent->type != JSON_ARRAY) {
		short keyLen = strlen(json->key)+1;
		//logger->inf(LOG_JSON, "-- Len: %d", keyLen);

		//logger->inf(LOG_JSON, "-- Init Key");
		char keyName[keyLen];
		memset(keyName, 0, keyLen);

		//logger->inf(LOG_JSON, "-- Init Format");


		//logger->inf(LOG_JSON, "-- Set Key Format: %s", format);

		//logger->inf(LOG_JSON, "-- Tabs: %d", tab);
		for (int i = 0; i <= curTab; ++i) {
			strcpy(tmpFormat, format);
			snprintf(format, 250, "%s%s", "  ", tmpFormat);
		}
		snprintf(format, 250, "%s\"%%s\": ", tmpFormat);

		snprintf(keyName, 250, format, json->key);

		key = keyName;
	}
	else{
		//logger->inf(LOG_JSON, "=== TEST-2.1");
		char keyName[curTab+1];
		memset(keyName, 0, curTab+1);
		for (int i = 0; i <= curTab; ++i) {
			strcpy(tmpFormat, format);
			snprintf(format, 250, "%s%s", "  ", tmpFormat);
		}

		key = format;
	}

	//logger->inf(LOG_JSON, "-- Key-0: %s", key);

	char sep = ',';
	if (i+1 >= json->parent->childs->nodeCount) {
		sep = ' ';
	}
	//logger->inf(LOG_JSON, "-- SEPARATOR: %c", sep);
	//logger->inf(LOG_JSON, "-- Key-1: %s", key);

	char value[5500];
	memset(value, 0, 5500);
	switch (json->type) {
		case JSON_NULL:
			//logger->inf(LOG_JSON, "-- Key-2: %s", key);
			snprintf(value, 5500, "NULL");
			break;

		case JSON_NUM:
			//logger->inf(LOG_JSON, "-- Key-3: %s", key);
			snprintf(value, 5500, "%f", json->num);
			//logger->inf(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_STRING:
			//logger->inf(LOG_JSON, "-- Key-4: %s", key);
			snprintf(value, 5500, "\"%s\"", json->string);
			break;

		case JSON_ARRAY:
			//logger->inf(LOG_JSON, "-- Key-5: %s", key);
			snprintf(value, 5500, "[\n");
			break;

		case JSON_OBJECT:
			//logger->inf(LOG_JSON, "-- Key-6: %s", key);
			snprintf(value, 5500, "{\n");
			break;
	}

	//logger->inf(LOG_JSON, "-- Key-7: %s", key);
	fprintf(stdout, "%s%s", key, value);

	if (json->type == JSON_ARRAY) {
		listIterateFnc(json->childs, jsonPrintData, NULL, (void*) &curTab);
		fprintf(stdout, "\n%s]", tmpFormat);
	}
	else if(json->type == JSON_OBJECT) {
		listIterateFnc(json->childs, jsonPrintData, NULL, (void*) &curTab);
		fprintf(stdout, "\n%s}", tmpFormat);
	}

	fprintf(stdout, "%c\n", sep);
	return true;
}

void jsonPrint(Json* json, int tab) {
	if (json == NULL) {
		logger->war(LOG_JSON, "Trying to Print NULL JSON");
		return;
	}

	char* key;
	char tmpFormat[250];
	memset(tmpFormat, 0, 250);

	if (json->key != NULL) {
		short keyLen = strlen(json->key)+1;

		char keyName[keyLen];
		memset(keyName, 0, keyLen);

		char format[250];
		memset(format, 0, 250);

		for (int i = 0; i < tab; ++i) {
			strcpy(tmpFormat, format);
			snprintf(format, 250, "%s%s", "  ", tmpFormat);
		}

		snprintf(format, 250, "%s\"%%s\": ", tmpFormat);
		snprintf(keyName, 250, format, json->key);

		key = keyName;
	}
	else{
		char keyName[1];
		memset(keyName, 0, 1);

		key = keyName;
	}

	char value[5500];
	memset(value, 0, 5500);
	switch (json->type) {
		case JSON_NULL:
			snprintf(value, 5500, "NULL\n");
			break;

		case JSON_NUM:
			snprintf(value, 5500, "%f\n", json->num);
			break;

		case JSON_STRING:
			snprintf(value, 5500, "\"%s\"\n", json->string);
			break;

		case JSON_ARRAY:
			snprintf(value, 5500, "[\n");
			break;

		case JSON_OBJECT:
			snprintf(value, 5500, "{\n");
			break;
	}

	fprintf(stdout, "%s%s", key, value);
	if (json->type == JSON_ARRAY) {
		listIterateFnc(json->childs, jsonPrintData, NULL, (void*) &tab);
		fprintf(stdout, "\n%s]", tmpFormat);
	}
	else if(json->type == JSON_OBJECT) {
		listIterateFnc(json->childs, jsonPrintData, NULL, (void*) &tab);
		fprintf(stdout, "\n%s}", tmpFormat);
	}
}

Json* jsonGetData(Json* json, char* key) {
	if (json == NULL) {
		logger->war(LOG_JSON, "Trying To Get Value of A NULL JSON !!!");
		return NULL;
	}

	if (json->childs == NULL) {
		logger->war(LOG_JSON, "Trying To Get Value of A None Parent JSON !!!");
		return NULL;
	}

	Node* n = getNodeByName(json->childs, key);
	if (n == NULL) {
		return NULL;
	}

	return (Json*) n->value;
}

void* jsonGetValue(Json* json, char* key, float* floatP) {
	Json* res = jsonGetData(json, key);
	if (res == NULL) {
		return NULL;
	}

	switch (res->type) {
		case JSON_NULL:
			return NULL;
			break;

		case JSON_STRING:
			return Str(res->string);
			break;

		case JSON_NUM:
			*floatP = res->num;
			return (void*) 1;
			break;

		case JSON_ARRAY:
		case JSON_OBJECT:
			return res->childs;
			break;
	}

	return NULL;
}

short jsonIteraterator(int i, Node* n, short* delete, void* param, va_list* args) {
	JsonIterator* it = (JsonIterator*) param;

	Json* json = n->value;
	logger->err(LOG_JSON, "-- Child-%d: %s", i, json->key);
	logger->err(LOG_JSON, "-- type: %s", GEN_JSON_TYPE_STRING[json->type]);

	if (it->fnc == NULL) {
		logger->err(LOG_JSON, "JSON ITERATOR FUNCTION IS NULL !!!");
		return false;
	}

	return it->fnc((unsigned int) i, (Json*) n->value);
}

void jsonIterate(Json* json, short (*fnc)(unsigned int i, Json* json), void* param, ...) {
	if (json == NULL) {
		logger->war(LOG_JSON, "Trying To Iterate Through NULL JSON !!!");
		return;
	}

	if (json->childs == NULL) {
		logger->war(LOG_JSON, "Trying To Iterate Through JSON BUT CHILDS IS NULL !!!");
		return;
	}

	JsonIterator it;
	it.fnc = fnc;

	listIterateFnc(json->childs, jsonIteraterator, NULL, (void*) &it);
}
