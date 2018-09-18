#include "libList.h"
#include <stdarg.h>

/**
 * Initialize A list
 * @return ListManager
 */
ListManager* initListMgr(){
	static int id = 0;
	ListManager* lstMgr = malloc(sizeof(ListManager));
	if (lstMgr == NULL){
		return NULL;
	}

	lstMgr->ID = id++;
	lstMgr->lastId = 0;
	lstMgr->nodeCount = 0;
	lstMgr->first = NULL;
	lstMgr->last = NULL;

	lstMgr->pid = -1;
	lstMgr->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	lstMgr->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	return lstMgr;
}

void lockList(ListManager* lst) {
	pid_t curTH = getpid();
	// fprintf(stdout, "LOCKING LIST: %d / %d \n", (int) curTH, (int) lst->pid);

	if (curTH == lst->pid) {
		// fprintf(stdout, "LOCKING LIST IN SAME THREAD: %d \n", (int) lst->pid);
		//assert(0);
	}
	else {
		pthread_mutex_lock(&lst->mutex);
	}
	lst->pid = curTH;
}

void unlockList(ListManager* lst) {
	//fprintf(stdout, "UN-LOCKING LIST: %d \n", (int) getpid());
	pthread_mutex_unlock(&lst->mutex);
}

void lockNode(Node* n) {
	pid_t curTH = getpid();
	//fprintf(stdout, "LOCKING Node: %d \n", (int) curTH);
	//fprintf(stdout, "CURRENT Node: %d \n", (int) n->pid);

	if (curTH == n->pid) {
		// fprintf(stdout, "LOCKING NODE IN SAME THREAD: %d \n", (int) n->pid);
		//assert(0);
	}
	else {
		pthread_mutex_lock(&n->mutex);
	}

	n->pid = curTH;
}

void unlockNode(Node* n) {
	//fprintf(stdout, "UN-LOCKING NODE: %d \n", (int) getpid());
	pthread_mutex_unlock(&n->mutex);
}

/**
 * Add a Node
 * @param lstMgr List Targer
 * @param params Node Key
 * @return Node*
 */
void* addNode(ListManager* lstMgr, void* params){
	int id;
	char* name = (char*) params;
	//printf("New Node Name %s\n", name);
	Node* newNode = malloc(sizeof(Node));
	if (newNode == NULL){
		printf("\n### Fail to alloc Node###\n");
		return NULL;
	}

	lockList(lstMgr);

	newNode->name = malloc(strlen(name)+1);
	strcpy(newNode->name, name);

	id = ++lstMgr->lastId;
	newNode->id = id;

	newNode->del = NULL;

	newNode->key = NULL;

	newNode->value = NULL;

	newNode->lstMgr = lstMgr;

	newNode->keyIsAlloc = 0;
	newNode->valIsAlloc = 0;

	newNode->pid = -1;
	newNode->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	newNode->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	if (!lstMgr->nodeCount) // Is the first node
	{
		lstMgr->first = newNode;
		lstMgr->last = newNode;
		newNode->prev = NULL;
		newNode->next = NULL;
	}
	else{
		newNode->next = NULL;

		newNode->prev = lstMgr->last;

		if (lstMgr->last != NULL) {
			lstMgr->last->next = newNode;
		}

		lstMgr->last = newNode;

	}

	lstMgr->nodeCount++;


	unlockList(lstMgr);

	return newNode;
}

/**
 * Add a Node whith value
 * @param lstMgr List Targer
 * @param params Node Key
 * @param value      Node Value
 * @param valueAlloc Value is malloc
 * @return	Node*
 */
void* addNodeV(ListManager* lstMgr, void* params, void* value, short valueAlloc){
	Node* n = addNode(lstMgr, params);
	if (n == NULL) {
		return NULL;
	}

	n->value = value;
	n->valIsAlloc = valueAlloc;
	return n;
}

/**
 * Print Node details
 * @param node [description]
 */
void printNode(Node* node){
	if (node == NULL)
	{
		printf("node for printNode is NULL\n");
		return;
	}

	printf("id: %d \n", node->id);
	printf("name: %s \n", node->name);

	lockNode(node);

	if (node->value == NULL)
	{
		printf("no Value Set\n");
	}
	else{
		printf("Value is Set\n");

		if (node->valIsAlloc){
			printf("Value is Alloc\n");
		}
		else {
			printf("Value is NOT Alloc\n");
		}
	}

	unlockNode(node);
}

/**
 * Print List Content
 * @param lstMgr [description]
 */
void printNodes(ListManager* lstMgr){
	Node* currentNode;
	currentNode = NULL;

	lockList(lstMgr);

	int i;
	for (i = 0; i < lstMgr->nodeCount; ++i)
	{
		if (currentNode == NULL)
		{
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		printNode(currentNode);
		printf("\n");
	}

	unlockList(lstMgr);
}

/**
 * Get Node By Id
 * @param  lstMgr List
 * @param  id     Id
 * @return        Node*
 */
Node* getNode(ListManager* lstMgr, int id){
	//fprintf(stdout, "GETTING NODE: %d\n", id);
	short found = 0;
	Node* currentNode = NULL;

	lockList(lstMgr);

	int i;
	for (i = 0; i < lstMgr->nodeCount; ++i)
	{
		//fprintf(stdout, "--NODE #%d\n", i);
		if (currentNode == NULL) {
			//fprintf(stdout, "--GET FIRST\n");
			currentNode = lstMgr->first;
		}
		else{
			//fprintf(stdout, "--GET NEXT\n");
			currentNode = currentNode->next;
		}

		if (currentNode->id == id) {
			found = 1;
			//fprintf(stdout, "--FOUND !!\n");
			break;
		}
	}

	//fprintf(stdout, "--GET END !!\n");
	unlockList(lstMgr);

	if (found) {
		return currentNode;
	}

	return NULL;
}

/**
 * Get Node By Key
 * @param  lstMgr List
 * @param  name   Key
 * @return        Node*
 */
Node* getNodeByName(ListManager* lstMgr, char* name){
	short found = 0;
	Node* currentNode = NULL;

	// fprintf(stdout, "GETTING NODE BY NAME: %d \n", (int) getpid());


	if (strlen(name) == 0) {
		printf("## Error: name param for getNode is invalid\n");
		return NULL;
	}

	// fprintf(stdout, "CALL LOCK LIST: %d \n", (int) getpid());
	lockList(lstMgr);
	// fprintf(stdout, "Lock Done\n");

	int i;
	for (i = 0; i < lstMgr->nodeCount; ++i)
	{
		if (currentNode == NULL)
		{
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		if (!strcmp(currentNode->name, name)) {
			found = 1;
			break;
		}
	}

	// fprintf(stdout, "UN-LOCK LIST \n");
	unlockList(lstMgr);

	if (found) {
		return currentNode;
	}

	return NULL;
}

/**
 * Delete Node By Name
 * @param lstMgr List
 * @param id     Id
 */
void deleteNodeByName(ListManager* lstMgr, char* name) {
	//fprintf(stdout, "DELETING NODE: %s\n", name);
	Node* n = getNodeByName(lstMgr, name);
	if (n == NULL) {
		return;
	}

	removeAndFreeNode(lstMgr, n);
}

/**
 * Delete Node By Name
 * @param lstMgr List
 * @param id     Id
 */
Node* deleteNodeByNameNoFree(ListManager* lstMgr, char* name) {
	//fprintf(stdout, "DELETING NO FREE NODE: %s\n", name);
	Node* n = getNodeByName(lstMgr, name);
	if (n == NULL) {
		return NULL;
	}

	return deleteNodeNoFree(lstMgr, n->id);
}

void freeNodeKey(Node* n) {
	if (n->keyIsAlloc) {
		free(n->key);
	}
}

void freeNodeValue(Node* n) {
	if (n->valIsAlloc) {
		free(n->value);
	}
}

void freeNode(Node* node) {
	if (node->del != NULL) {
		node->del(node->value);
	}

	freeNodeKey(node);
	freeNodeValue(node);

	free(node->name);
	free(node);
}

void removeNode(ListManager* lstMgr, Node* node) {
	Node* prev = node->prev;
	Node* next = node->next;

	if (prev != NULL) {
		prev->next = next;
	}

	if (next != NULL){
		next->prev = prev;
	}

	if (lstMgr->first == node){
		lstMgr->first = next;
	}

	if (lstMgr->last == node){
		lstMgr->last = prev;
	}

	if (--lstMgr->nodeCount == 1){
		lstMgr->first->next = NULL;
		lstMgr->first->prev = NULL;
	}
	else if (!lstMgr->nodeCount){
		lstMgr->first = NULL;
		lstMgr->last = NULL;
	}
}

void removeAndFreeNode(ListManager* lstMgr, Node* node) {
	removeNode(lstMgr, node);
	freeNode(node);
}

/**
 * Delete Node By Id
 * @param lstMgr List
 * @param id     Id
 */
Node* deleteNodeNoFree(ListManager* lstMgr, int id){
	//fprintf(stdout, "DELETING NODE NO FREE: %u\n", id);
	Node* node = getNode(lstMgr, id);
	if (node == NULL){
		//fprintf(stdout, "NOt FOUND\n");
		return NULL;
	}

	//fprintf(stdout, "FOUND !!!\n");

	lockList(lstMgr);
	//fprintf(stdout, "Lock Node\n");
	lockNode(node);

	//fprintf(stdout, "Working\n");
	removeNode(lstMgr, node);

	//fprintf(stdout, "NO FREE DONE\n");
	unlockList(lstMgr);
	unlockNode(node);

	return node;
}

/**
 * Delete Node By Id
 * @param lstMgr List
 * @param id     Id
 */
void* deleteNode(ListManager* lstMgr, int id){
	//fprintf(stdout, "DELETING NODE: %u\n", id);
	//printf("DELETING NODE: %d\n", id);
	Node* node = getNode(lstMgr, id);
	lockNode(node);

	if (node == NULL) {
		//fprintf(stdout, "NOt FOUND\n");
		return NULL;
	}
	//fprintf(stdout, "-- FOUND !!!\n");


	//fprintf(stdout, "DELETE DONE\n");
	unlockNode(node);
	removeAndFreeNode(lstMgr, node);

	return NULL;
}

/**
 * Set Node Value
 * @param  node    Node
 * @param  value   Value
 * @param  asAlloc Value Is Alloc
 * @return         1
 */
int setValue(Node* node, void* value, short asAlloc){
	//fprintf(stdout, "SET VALUE: %d\n", node->id);
	lockNode(node);
	if (node->valIsAlloc && node->value != NULL){
		free(node->value);
	}

	node->value = value;
	node->valIsAlloc = asAlloc;

	unlockNode(node);
	return 1;
}

/**
 * Get Array Of Node Ids
 * @param  lstMgr List
 * @param  ids    int[]
 * @return        int*
 */
int* getIds(ListManager* lstMgr, int* ids){
	if (lstMgr->nodeCount <= 0){
		return NULL;
	}

	int i=0;
	Node* currentNode = NULL;
	lockList(lstMgr);

	do{
		if (currentNode == NULL){
			currentNode = lstMgr->first;
			lockNode(currentNode);
		}
		else{
			currentNode = currentNode->next;
			lockNode(currentNode);
		}

		ids[i++] = currentNode->id;
		unlockNode(currentNode);
	}while(lstMgr->last != currentNode);

	unlockList(lstMgr);
	return ids;
}


/**
 * Clear List
 * @param lstMgr List
 */
void clearList(ListManager* lstMgr){
	int i;
	int ids[lstMgr->nodeCount];

	getIds(lstMgr, ids);

	for (i = 0; lstMgr->nodeCount > 0; ++i){
		deleteNode(lstMgr, ids[i]);
	}
}


/**
 * Delete List
 * @param lstMgr List
 */
void deleteList(ListManager* lstMgr){
	clearList(lstMgr);
	free(lstMgr);
}

Node* listIterate(ListManager* list, Node* n) {
	lockList(list);

	if (n != NULL) {
        lockNode(n);
	}

	if (list->first == NULL){
        if (n != NULL) {
            unlockNode(n);
        }

        unlockList(list);
		return  NULL;
	}
	else if (n == NULL) {
		n = list->first;
		unlockList(list);
		return n;
	}
	else if (n->next == NULL) {
        unlockNode(n);
        unlockList(list);
		return NULL;
	}

    if (n != NULL) {
        unlockNode(n);
    }

    unlockList(list);
	return n->next;
}


void listIterateFnc(ListManager* list, short (*fnc)(int , Node*, short*, void*, va_list* args), Node* n, void* param, ...) {
	lockList(list);

	int i = 0;
	short delete = 0;
	short process = 1;

	va_list args;

	if (n == NULL) {
        n = list->first;
	}

	while (process && n != NULL) {
		va_start(args, param);
		lockNode(n);

		Node* tmp = n;
		process = fnc(i++, n, &delete, param, &args);
		n = n->next;

		unlockNode(tmp);

		if (delete) {
			removeAndFreeNode(list, tmp);
		}

		delete = 0;
	}

	va_end(args);

	unlockList(list);
}


void listRevIterateFnc(ListManager* list, short (*fnc)(int , Node*, short*, void*, va_list* args), Node* n, void* param, ...) {
	lockList(list);

	int i = 0;
	short delete = 0;
	short process = 1;

	va_list args;
	va_start(args, param);

	if (n == NULL) {
        n = list->last;
	}

	while (process && n != NULL) {
		lockNode(n);

		Node* tmp = n;
		process = fnc(i++, n, &delete, param, &args);
		n = n->prev;

		unlockNode(tmp);

		if (delete) {
			removeAndFreeNode(list, tmp);
		}

		delete = 0;
	}

	va_end(args);
	unlockList(list);
}

Node* listRevIterate(ListManager* list, Node* n) {
	lockList(list);
	if (list->last == NULL){
        if (n != NULL) {
            unlockNode(n);
        }
        unlockList(list);
		return NULL;
	}
	else if (n == NULL) {
		n = list->last;
		unlockList(list);
		return  n;
	}
	else if (n->prev == NULL) {
        unlockNode(n);
        unlockList(list);
		return NULL;
	}

	unlockList(list);
    if (n != NULL) {
        unlockNode(n);
    }

	return n->prev;
}

short listInsertAfter(ListManager* lst, Node* n, short id) {
	Node* prev = getNode(lst, id);
	if (prev == NULL){
		return 0;
	}

	short inList = (getNode(lst, n->id) == NULL) ? 0 : 1;
	if (!inList) {
		return 0;
	}

	////fprintf(stdout, "-- Inserting %s After %s\n", n->name, prev->name);
	if (n->prev == prev) {
		//fprintf(stdout, "-- Nothing to to\n");
		return 0;
	}


	//printNodes(lst);

	lockList(lst);

	lockNode(n);
	Node* tmpP = n->prev;
	Node* tmpN = n->next;

	if (tmpP != NULL) {
		if (tmpN != NULL) {
			//fprintf(stdout, "-- 1# %s Is Now Before %s\n", tmpP->name, tmpN->name);
			tmpP->next = tmpN;
		}
		else{
			//fprintf(stdout, "-- 1# %s Is Now LAST\n", tmpP->name);
			tmpP->next = NULL;
			lst->last = tmpP;
		}

	}

	if (tmpN != NULL) {
		if (tmpP != NULL) {
			//fprintf(stdout, "-- 2# %s Is Now After %s\n", tmpN->name, tmpP->name);
			tmpN->prev = tmpP;
		}
		else{
			//fprintf(stdout, "-- 2# %s Is Now FIRST\n", tmpN->name);
			tmpN->prev = NULL;
			lst->first = tmpN;
		}
	}

	n->prev = prev;
	//fprintf(stdout, "-- 3# %s Is Now Before %s\n", prev->name, n->name);

	n->next = prev->next;
	//fprintf(stdout, "-- 4# %s Is Now Before %s\n", n->name, prev->next->name);

	n->lstMgr = lst;

	if (prev->next != NULL) {
		prev->next->prev = n;
	}

	prev->next = n;
	//fprintf(stdout, "-- 5# %s Is Now Before %s\n", prev->name, n->name);

	if (prev == lst->last){
		lst->last = n;
		n->next = NULL;
	}

	//fprintf(stdout, "==============\n");
	//lst->nodeCount++;


	unlockNode(n);

	unlockList(lst);
	return 1;
}

void sortList(ListManager * lst, short (*fnc)(void*, void*)) {

	int i;
	short sort = 0;
	Node* tmp = NULL;
	Node* comp = NULL;

	lockList(lst);

	Node* key = lst->first->next;
	for (i = 1; i < lst->nodeCount; i++) {
    	comp = lst->first;
    	lockNode(key);
    	lockNode(comp);

		//fprintf(stdout, "##### Node %d: %s #####\n", key->id, key->name);

        do {
			//fprintf(stdout, "-- Compare %d: %s\n", comp->id, comp->name);
        	if (key == comp) {
				//fprintf(stdout, "-- Skipping\n");
        		comp = key->next;
        		unlockNode(key);
        		unlockNode(comp);
        		continue;
        	}


    		sort = (*fnc)(comp->value, key->value);

			//fprintf(stdout, "-- Res: %d\n", sort);

    		if (sort < 0) {
    			tmp = NULL;
				//fprintf(stdout, "+++++ Moving: %s\n", key->name);
    			while((tmp = listIterate(lst, tmp)) != NULL) {
    				if (tmp == comp) {
						//fprintf(stdout, "-- Skipping\n");
    					continue;
    				}

    				lockNode(tmp);
					//fprintf(stdout, "-- Compare %d: %s\n", tmp->id, tmp->name);
					sort = fnc(comp->value, tmp->value);

					//fprintf(stdout, "-- Res: %d\n", sort);

    			    if (sort < 0) {
						listInsertAfter(lst, comp, tmp->id);
						listInsertAfter(lst, tmp, comp->id);

						comp = key;
						break;
    			    }

    				unlockNode(tmp);
					//fprintf(stdout, "+++++++++++++++++++++++++\n");
    			}

    			//printNodes(lst);
				//assert(0);
    		}


    		unlockNode(comp);
    		comp = comp->next;
			//fprintf(stdout, "================================================\n");
        } while (comp != NULL);

		//fprintf(stdout, "--------------------------------------------------------------------\n");
		//
		unlockNode(key);
		unlockList(lst);

   		key = key->next;
        if (key == NULL) {
        	break;
        }
	}
}

Node* getNodeByKey(ListManager* lst, void* key) {
    Node* n = NULL;

    while ((n = listIterate(lst, n)) != NULL) {
        if (n->key != key) {
            return n;
        }
    }

    return NULL;
}
