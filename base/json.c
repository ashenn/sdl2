#include "json.h"
#include "../lib/jsmn/jsmn.h"

#include <assert.h>
#include <stdarg.h>

void jsonFill(Json* json, char* data) {
	logger->inf(LOG_JSON, "==== FILLING JSON ====");

	jsmn_parser parser;
	logger->dbg(LOG_JSON, "-- DATA: \n%s", data);

	logger->dbg(LOG_JSON, "-- init parser");
	jsmn_init(&parser);

	logger->dbg(LOG_JSON, "-- Count Tokens");
	int tokenCnt = jsmn_parse(&parser, data, strlen(data), NULL, 1500);

	if (tokenCnt < 0) {
        switch (tokenCnt) {
            case JSMN_ERROR_INVAL:
                logger->err(LOG_JSON, "JSON CORRUPTED");
                break;

            case JSMN_ERROR_NOMEM:
                logger->err(LOG_JSON, "Not Enought TOKENS");
                break;

            case JSMN_ERROR_PART:
                logger->err(LOG_JSON, "JSON string is too short, expecting more JSON data");
                break;

            default:
                logger->err(LOG_JSON,"-- UNKNOWN Error !!!!");
                break;
        }

        return;
	}

	logger->dbg(LOG_JSON, "-- Re-Init Parser");
	jsmn_init(&parser);

	logger->dbg(LOG_JSON, "-- Init Tokens");
	jsmntok_t tokens[tokenCnt];

	logger->dbg(LOG_JSON, "-- Parse JSON");
	int res = jsmn_parse(&parser, data, strlen(data), tokens, tokenCnt);
    if (res == JSMN_ERROR_NOMEM) {
        logger->err(LOG_JSON, "FAIL TO PARSE JSON, NOT ENOUGTH MEMORY !!!");
    }

	int index = 0;
	short isKey = 1;
	short inArray = 0;

	char key[1500];
	char buffer[1500];

	json->childCount = tokens[0].size;

	if (data[tokens[0].start] == '{')
	{
		logger->dbg(LOG_JSON, "-- Is Object: %c", data[tokens[0].start]);
		json->type = JSON_OBJECT;
	}
	else if (data[tokens[0].start] == '[') {
		inArray = true;
		logger->dbg(LOG_JSON, "-- Is Array: %c", data[tokens[0].start]);
		json->type = JSON_ARRAY;
	}
	else {
		logger->err(LOG_JSON, "JSON CORRUPTED");
		return;
	}

	logger->dbg(LOG_JSON, "-- JSON Type: %s", GEN_JSON_TYPE_STRING[json->type]);
	logger->dbg(LOG_JSON, "-- LOOP JSON");
	for (int i = 1; i < res; ++i) {

		memset(buffer, 0, 1500);
		memcpy(buffer, &data[tokens[i].start], tokens[i].end - tokens[i].start);

		if (isKey && !inArray) {
			isKey = 0;
			strcpy(key, buffer);
			logger->dbg(LOG_JSON, "-- Key %s", buffer);
		}
		else{
			int num = 0;
			float val = 0;
			bool boolean = 0;
			if (inArray) {
				snprintf(key, 100, "%d", index);
				logger->dbg(LOG_JSON, "-- Key %s", key);
			}

			switch (tokens[i].type) {
				case JSMN_UNDEFINED:
					logger->dbg(LOG_JSON, "-- Type: Undefined");
					jsonSetValue(json, key, NULL, JSON_NULL);
					break;

				case JSMN_ARRAY:
					inArray = 1;
					index = 0;

					logger->dbg(LOG_JSON, "-- Type: Array: %d", tokens[i].size);
					json = jsonSetValue(json, key,  initListMgr(), JSON_ARRAY);
					json->type = JSON_ARRAY;
					json->childCount = tokens[i].size;

					//strcpy(key, buffer);
					continue;
					break;

				case JSMN_OBJECT:
					isKey = 1;
					index = 0;

					logger->dbg(LOG_JSON, "-- Type: Object: %d", tokens[i].size);
					json = jsonSetValue(json, key,  initListMgr(), JSON_OBJECT);
					json->type = JSON_OBJECT;
					json->childCount = tokens[i].size;

					//strcpy(key, buffer);
					continue;
					break;


				case JSMN_STRING:
					logger->inf(LOG_JSON, "-- Type: String");
					jsonSetValue(json, key, buffer, JSON_STRING);
					break;

				case JSMN_PRIMITIVE:
					logger->inf(LOG_JSON, "-- Type: Primitive");

					if (buffer[0] == 'n') {
						jsonSetValue(json, key, NULL, JSON_NULL);
					}
					else if(buffer[0] == 't') {
						boolean = true;
						jsonSetValue(json, key, &boolean, JSON_BOOL);
					}
					else if(buffer[0] == 'f') {
						boolean = false;
						jsonSetValue(json, key, &boolean, JSON_BOOL);
					}
					else {
						if (strpos('.', buffer, 0) >= 0) {
							val = str2float(buffer);
							jsonSetValue(json, key, &val, JSON_NUM);
						}
						else {
							num = str2int(buffer);
							jsonSetValue(json, key, &num, JSON_INT);
						}
					}

					break;
			}

			if (!inArray) {
				isKey = 1;
			}

			while (json->id >= 0 && (inArray || json->type == JSON_OBJECT)) {
				if (index+1 >= json->childCount) {
					logger->inf(LOG_JSON, "-- Back To Parent");

					if (json->parent == NULL) {
						logger->inf(LOG_JSON, "-- Parent IS NULL");
					}

					logger->inf(LOG_JSON, "-- Become Parent: %s", json->parent->key);
					json = json->parent;

					logger->inf(LOG_JSON, "-- Check Count");
					index = json->childs->nodeCount-1;

					logger->inf(LOG_JSON, "-- Set Is Key");
					isKey = 1;

					logger->inf(LOG_JSON, "-- Parent Type");
					inArray = json->type == JSON_ARRAY;

					logger->inf(LOG_JSON, "-- End Of Container Reached");
				}
				else{
					break;
				}
			}

			index++;
		}
	}
	//jsonPrint(json, 0);
}

Json* newJson() {
	logger->inf(LOG_JSON, "==== Creating New Json ===");

	Json* json = malloc(sizeof(Json));
	json->id = -1;
	json->num = 0;
	json->key = NULL;
	json->string = NULL;

	json->parent = NULL;
	json->childs = NULL;
	json->type = JSON_NULL;

	logger->dbg(LOG_JSON, "-- Json Ready");
	return json;
}

Json* loadJsonFile(char* p) {
	char path[350];
	memset(path, 0, 350);
	memcpy(path, p, strlen(p));

	validatePath(path);
	logger->inf(LOG_JSON, "==== New Json From Path: %s ===", path);

	logger->inf(LOG_JSON, "-- TEST-0");
	char* content = fileGetContent(path);
	logger->inf(LOG_JSON, "-- TEST-1");
	if (content == NULL) {
		logger->err(LOG_JSON, "Fail to Load Json File: %s", path);
		return NULL;
	}

	logger->inf(LOG_JSON, "-- Content Length: %d ===", strlen(content));
	Json* json = newJson();
	logger->inf(LOG_JSON, "-- CALL FILL JSON");
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
		// logger->dbg(LOG_JSON, "-- Init Child List");
		json->childs = initListMgr();

		// logger->dbg(LOG_JSON, "-- Adding Key To Parent");
		childNode = addNode(json->childs, key);
	}
	else{
		childNode = getNodeByName(json->childs, key);

		if (childNode != NULL) {
			// logger->dbg(LOG_JSON, "-- Replacing Existing Child");
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
			// logger->dbg(LOG_JSON, "-- Setting Null Value");
			break;

		case JSON_BOOL:
			// logger->dbg(LOG_JSON, "-- Setting Boolean Value");
			child->boolean = *((bool*) value);
			break;

		case JSON_STRING:
			// logger->dbg(LOG_JSON, "-- Setting String Value");
			child->string = Str((char*) value);
			break;

		case JSON_INT:
			// logger->dbg(LOG_JSON, "-- Setting Int Value");
			child->integer = *((int*) value);
			break;

		case JSON_NUM:
			// logger->dbg(LOG_JSON, "-- Setting Numeric Value");
			child->num = *((float*) value);
			break;

		case JSON_ARRAY:
		case JSON_OBJECT:
			// logger->dbg(LOG_JSON, "-- Prepare Child->Child List");
			child->childs = (ListManager*) value;
			break;
	}

	// logger->dbg(LOG_JSON, "==== Setting Key: %s Done ====", key);
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
	logger->inf(LOG_JSON, "=== TEST-0");
	Json* json = (Json*) n->value;

	logger->inf(LOG_JSON, "=== TEST-1");

	int tab = *((int*) param);
	logger->inf(LOG_JSON, "=== TEST-1.2");

	int curTab = tab+1;
	logger->inf(LOG_JSON, "=== Cur Tab: %d", curTab);

	if (json == NULL) {
		logger->err(LOG_JSON, "=== JSON IS NULL");
		return true;
	}

	char* key;
	char tmpFormat[250];
	memset(tmpFormat, 0, 250);
	logger->inf(LOG_JSON, "=== TEST-1.1");

	char format[250];
	memset(format, 0, 250);
//	snprintf(format, 250, "");

	if (json->key != NULL && json->parent->type != JSON_ARRAY) {
		short keyLen = strlen(json->key)+1;
		logger->inf(LOG_JSON, "-- Len: %d", keyLen);

		logger->inf(LOG_JSON, "-- Init Key");
		char keyName[keyLen];
		memset(keyName, 0, keyLen);

		logger->inf(LOG_JSON, "-- Init Format");


		logger->inf(LOG_JSON, "-- Set Key Format: %s", format);

		logger->inf(LOG_JSON, "-- Tabs: %d", tab);
		for (int i = 0; i <= curTab; ++i) {
			strcpy(tmpFormat, format);
			snprintf(format, 250, "%s%s", "  ", tmpFormat);
		}
		snprintf(format, 250, "%s\"%%s\": ", tmpFormat);

		snprintf(keyName, 250, format, json->key);

		key = keyName;
	}
	else{
		logger->inf(LOG_JSON, "=== TEST-2.1");
		char keyName[curTab+1];
		memset(keyName, 0, curTab+1);
		for (int i = 0; i <= curTab; ++i) {
			strcpy(tmpFormat, format);
			snprintf(format, 250, "%s%s", "  ", tmpFormat);
		}

		key = format;
	}

	logger->inf(LOG_JSON, "-- Key-0: %s", key);

	char sep = ',';
	if (i+1 >= json->parent->childs->nodeCount) {
		sep = ' ';
	}
	logger->inf(LOG_JSON, "-- SEPARATOR: %c", sep);
	logger->inf(LOG_JSON, "-- Key-1: %s", key);

	char value[5500];
	memset(value, 0, 5500);
	switch (json->type) {
		case JSON_NULL:
			logger->inf(LOG_JSON, "-- Key-2: %s", key);
			snprintf(value, 5500, "NULL");
			break;

		case JSON_BOOL:
			logger->inf(LOG_JSON, "-- Key-3: %s", key);
			if (json->boolean) {
				snprintf(value, 5500, "true");
			}
			else{
				snprintf(value, 5500, "false");
			}
			logger->inf(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_INT:
			logger->inf(LOG_JSON, "-- Key-3: %s", key);
			snprintf(value, 5500, "%d", json->integer);
			logger->inf(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_NUM:
			logger->inf(LOG_JSON, "-- Key-3: %s", key);
			snprintf(value, 5500, "%f", json->num);
			logger->inf(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_STRING:
			logger->inf(LOG_JSON, "-- Key-4: %s", key);
			snprintf(value, 5500, "\"%s\"", json->string);
			break;

		case JSON_ARRAY:
			logger->inf(LOG_JSON, "-- Key-5: %s", key);
			snprintf(value, 5500, "[\n");
			break;

		case JSON_OBJECT:
			logger->inf(LOG_JSON, "-- Key-6: %s", key);
			snprintf(value, 5500, "{\n");
			break;
	}

	logger->inf(LOG_JSON, "-- Key-7: %s", key);
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

		case JSON_INT:
			snprintf(value, 5500, "%d\n", json->integer);
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

	fprintf(stdout, "\n\n");
}






short json2StrData(int i, Node* n, short* delete, void* param, va_list* args) {
	Json* json = (Json*) n->value;
	int tab = *((int*) param);
	int curTab = tab+1;

	bool breakLine = *(va_arg(*args, bool*));
	bool indent = *(va_arg(*args, bool*));
	char** parentStr = va_arg(*args, char**);
	int* parentStrLen = va_arg(*args, int*);

	if (json == NULL) {
		logger->err(LOG_JSON, "=== JSON IS NULL");
		return true;
	}

	char* key;
	char tmpFormat[250];
	memset(tmpFormat, 0, 250);

	char format[250];
	memset(format, 0, 250);

	if (json->key != NULL && json->parent->type != JSON_ARRAY) {
		short keyLen = strlen(json->key)+1;

		char keyName[keyLen];
		memset(keyName, 0, keyLen);

		if (breakLine && indent) {
			for (int i = 0; i <= curTab; ++i) {
				strcpy(tmpFormat, format);
				snprintf(format, 250, "%s%s", "  ", tmpFormat);
			}

		}

		snprintf(format, 250, "%s\"%%s\": ", tmpFormat);
		snprintf(keyName, 250, format, json->key);

		key = keyName;
	}
	else{
		char keyName[curTab+1];
		memset(keyName, 0, curTab+1);

		if (breakLine && indent) {
			for (int i = 0; i <= curTab; ++i) {
				strcpy(tmpFormat, format);
				snprintf(format, 250, "%s%s", "  ", tmpFormat);
			}
		}

		key = format;
	}

	logger->inf(LOG_JSON, "-- Child: Key: %s", key);
	logger->inf(LOG_JSON, "-- Child type: %s", GEN_JSON_TYPE_STRING[json->type]);

	char sep = ',';
	if (i+1 >= json->parent->childs->nodeCount) {
		sep = ' ';
	}
	////logger->inf(LOG_JSON, "-- SEPARATOR: %c", sep);
	////logger->inf(LOG_JSON, "-- Key-1: %s", key);

	int write = 0;
	char value[5500];
	memset(value, 0, 5500);
	switch (json->type) {
		case JSON_NULL:
			////logger->inf(LOG_JSON, "-- Key-2: %s", key);
			write = snprintf(value, 5500, "null");
			break;

		case JSON_BOOL:
			////logger->inf(LOG_JSON, "-- Key-3: %s", key);
			if (json->boolean) {
				write = snprintf(value, 5500, "true");
			}
			else{
				write = snprintf(value, 5500, "false");
			}
			////logger->inf(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_INT:
			////logger->inf(LOG_JSON, "-- Key-3: %s", key);
			write = snprintf(value, 5500, "%d", json->integer);
			////logger->inf(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_NUM:
			////logger->inf(LOG_JSON, "-- Key-3: %s", key);
			write = snprintf(value, 5500, "%f", json->num);
			////logger->inf(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_STRING:
			////logger->inf(LOG_JSON, "-- Key-4: %s", key);
			write = snprintf(value, 5500, "\"%s\"", json->string);
			break;

		case JSON_ARRAY:
			////logger->inf(LOG_JSON, "-- Key-5: %s", key);
			write = snprintf(value, 5500, "[");
			break;

		case JSON_OBJECT:
			////logger->inf(LOG_JSON, "-- Key-6: %s", key);
			write = snprintf(value, 5500, "{");
			break;
	}

	if (breakLine) {
		value[write] = '\n';
	}

	logger->inf(LOG_JSON, "-- Value: %s", value);
	//fprintf(stdout, "%s%s", key, value);


	char* end = "";
	char* resultStr = "";

	int childStrLen = 0;


	logger->inf(LOG_JSON, "-- Prepare End");
	if (json->type == JSON_ARRAY) {
		end = Str("]");
	}
	else if(json->type == JSON_OBJECT) {
		end = Str("}");
	}

	if (strlen(end)) {
		logger->inf(LOG_JSON, "-- Prepare Child");
		char* childStr[json->childs->nodeCount+1];
		memset(childStr, 0, json->childs->nodeCount+1);

		logger->inf(LOG_JSON, "-- Call Child");
		listIterateFnc(json->childs, json2StrData, NULL, (void*) &curTab, (void*) &breakLine, (void*) &indent, (void*) &childStr, (void*) &childStrLen);

		logger->inf(LOG_JSON, "-- Join Childs");
		resultStr = join(", ", childStr, json->childs->nodeCount, childStrLen);
		logger->inf(LOG_JSON, "-- Childs Results: %s", resultStr);
	}

	logger->inf(LOG_JSON, "-- get Length");
	logger->inf(LOG_JSON, "-- Value Len: %d", strlen(value));
	logger->inf(LOG_JSON, "-- key Len: %d", strlen(key));
	logger->inf(LOG_JSON, "-- end Len: %d", strlen(end));
	logger->inf(LOG_JSON, "-- resultStr Len: %d", strlen(resultStr));
	logger->inf(LOG_JSON, "-- end Len: %d", strlen(end));

	int len = strlen(value) + strlen(key) + strlen(resultStr) + strlen(end);
	logger->inf(LOG_JSON, "-- Total Length: %d", len);

	*parentStrLen += len;
	parentStr[i] = StrE(len);
	char* parentResultStr = parentStr[i];

	logger->inf(LOG_JSON, "-- Print 2 Result");
	logger->inf(LOG_JSON, "-- Value: %s", (value));
	logger->inf(LOG_JSON, "-- key: %s", (key));
	logger->inf(LOG_JSON, "-- end: %s", (end));
	logger->inf(LOG_JSON, "-- resultStr: %s", (resultStr));

	int total = snprintf(parentResultStr, len, "%s", key);
    total += snprintf(parentResultStr, len, "%s%s", parentResultStr, value);
	total += snprintf(parentResultStr, len, "%s%s", parentResultStr, resultStr);
	total += snprintf(parentResultStr, len, "%s%s", parentResultStr, end);

	logger->inf(LOG_JSON, "-- Print Result: %s", parentResultStr);


	if (strlen(end)) {
		free(end);
	}

	return true;
}

char* json2Str(Json* json, bool breakLine, bool indent) {
	logger->inf(LOG_JSON, "===== JSON 2 STRING ====");

	if (json == NULL) {
		logger->war(LOG_JSON, "Trying to Print NULL JSON");
		return NULL;
	}

	char value[5500];
	memset(value, 0, 5500);
	switch (json->type) {
		case JSON_NULL:
			snprintf(value, 5500, "null\n");
			break;

		case JSON_INT:
			snprintf(value, 5500, "%d\n", json->integer);
			break;

		case JSON_NUM:
			snprintf(value, 5500, "%f\n", json->num);
			break;

		case JSON_STRING:
			snprintf(value, 5500, "\"%s\"\n", json->string);
			break;

		case JSON_ARRAY:
			snprintf(value, 5500, "[");
			break;

		case JSON_OBJECT:
			snprintf(value, 5500, "{");
			break;
	}

	char* end = "";
	char* resultStr = "";

	int childStrLen = 0;
	int childCount = 0;
	if (json->childs != NULL) {
		json->childs->nodeCount+1;
	}

	char* childStr[childCount];
	memset(childStr, 0, childCount);
	int tab = 0;

	if (json->type == JSON_ARRAY) {
		end = Str("]");
	}
	else if(json->type == JSON_OBJECT) {
		end = Str("}");
	}

	if (strlen(end)) {
		listIterateFnc(json->childs, json2StrData, NULL, (void*) &tab, (void*) &breakLine, (void*) &indent, (void*) &childStr, (void*) &childStrLen);
		resultStr = join(", ", childStr, json->childs->nodeCount, childStrLen);
	}

	int totalLen = strlen(value) + strlen(resultStr) + strlen(end) + 2;
	char* finalStr = StrE(totalLen);

	snprintf(finalStr, totalLen, "%s%s%s", value, resultStr, end);
	fprintf(stdout, "\n%s\n", finalStr);

	if (strlen(end)) {
		free(end);
	}

	if (strlen(resultStr)) {
		free(resultStr);
	}

	for (int i = 0; i < childCount-1; ++i) {
		free(childStr[i]);
	}

	return finalStr;
}





Json* jsonGetData(Json* json, char* key) {
	logger->inf(LOG_JSON, "Getting JSON data: %s", key);

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
		logger->inf(LOG_JSON, "Fail To Find JSON Data: %s", key);
		return NULL;
	}

	return (Json*) n->value;
}

void* jsonGetValue(Json* json, char* key, void* val) {
	Json* res = jsonGetData(json, key);
	if (res == NULL) {
		return NULL;
	}

	switch (res->type) {
		case JSON_NULL:
			return NULL;
			break;

		case JSON_BOOL:
			*((bool*) val) = res->boolean;
			return (void*) 1;
			break;

		case JSON_STRING:
			return Str(res->string);
			break;

		case JSON_INT:
			*((int*) val) = res->integer;
			return (void*) 1;
			break;

		case JSON_NUM:
			*((float*) val) = res->num;
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
	if (it->fnc == NULL) {
		logger->err(LOG_JSON, "JSON ITERATOR FUNCTION IS NULL !!!");
		return false;
	}

	return (short) it->fnc((unsigned int) i, (Json*) n->value, (void*) va_arg(*args, ListManager*));
}

void jsonIterate(Json* json, bool (*fnc)(unsigned int, Json*, void*), void* param, ...) {
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

	listIterateFnc(json->childs, jsonIteraterator, NULL, (void*) &it, param);
}
