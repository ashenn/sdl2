#ifndef __LIBLIST_H__
#define __LIBLIST_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <pthread.h>

typedef struct ListManager ListManager;
typedef struct Node Node;

struct Node{
	int id;
	char* name;
	ListManager* lstMgr;

	void* value;
	void* key;
	void* (*fnc)();
	
	short valIsAlloc;
	short keyIsAlloc;

	Node* next;
	Node* prev;

	void* (*del)(void*);

	pthread_cond_t cond;
	pthread_mutex_t mutex;
};

struct ListManager
{
	int ID;
	int lastId;
	int nodeCount;

	Node* first;
	Node* last;
	
	pthread_cond_t cond;
	pthread_mutex_t mutex;
};

ListManager* initListMgr();
void* addNode(ListManager* lstMgr, void* params);
void* addNodeV(ListManager* lstMgr, void* params, void* value, short valueAlloc);
void printNode(Node* node);
void printNodes(ListManager* lstMgr);
Node* getNode(ListManager* lstMgr, int id);
Node* getNodeByName(ListManager* lstMgr, char* name);

void removeNode(ListManager* lstMgr, Node* node);
void* deleteNode(ListManager* lstMgr, int id);
Node* deleteNodeNoFree(ListManager* lstMgr, int id);
Node* deleteNodeByNameNoFree(ListManager* lstMgr, char* name);
void deleteNodeByName(ListManager* lstMgr, char* name);

void freeNode(Node* n);
void freeNodeKey(Node* n);
void freeNodeValue(Node* n);
void removeAndFreeNode(ListManager* lstMgr, Node* node);

int setValue(Node* node, void* value, short asAlloc);


int* getIds(ListManager* lstMgr, int* ids);
void clearList(ListManager* lstMgr);
void deleteList(ListManager* lstMgr);

Node* listIterate(ListManager* list, Node* n);
Node* listRevIterate(ListManager* list, Node* n);

void listIterateFnc(ListManager* list, short (*fnc)(int , Node*, short*, void*, va_list* args), Node* n, void* param, ...);
void listRevIterateFnc(ListManager* list, short (*fnc)(int , Node*, short*, void*, va_list* args), Node* n, void* param, ...);

short listInsertAfter(ListManager* lst, Node* n, short id);
void sortList(ListManager * lst, short (*fnc)(void*, void*));

#endif
