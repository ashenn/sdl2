#include "libList.h"

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
	lstMgr->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	lstMgr->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	return lstMgr;
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

	pthread_mutex_lock(&lstMgr->mutex);

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

		lstMgr->last->next = newNode;
		lstMgr->last = newNode;
	}

	lstMgr->nodeCount++;

	pthread_mutex_unlock(&lstMgr->mutex);

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

	pthread_mutex_lock(&node->mutex);

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

	pthread_mutex_unlock(&node->mutex);
}

/**
 * Print List Content
 * @param lstMgr [description]
 */
void printNodes(ListManager* lstMgr){
	Node* currentNode;
	currentNode = NULL;

	pthread_mutex_lock(&lstMgr->mutex);

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

	pthread_mutex_unlock(&lstMgr->mutex);
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

	pthread_mutex_lock(&lstMgr->mutex);

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
	pthread_mutex_unlock(&lstMgr->mutex);

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
	fprintf(stdout, "GETTING NODE: %s\n", name);

	if (strlen(name) == 0) {
		printf("## Error: name param for getNode is invalid\n");
		return NULL;
	}

	fprintf(stdout, "ASK LOCK !!!\n");
	pthread_mutex_lock(&lstMgr->mutex);
	fprintf(stdout, "LOCKED !!!\n");

	int i;
	for (i = 0; i < lstMgr->nodeCount; ++i)
	{
		fprintf(stdout, "SEARCHING: %d !!!\n", i);
		if (currentNode == NULL)
		{
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		if (!strcmp(currentNode->name, name)) {
			fprintf(stdout, "FOUND !!!\n");
			found = 1;
			break;
		}
	}

	fprintf(stdout, "UNLOCKED !!!\n");
	pthread_mutex_unlock(&lstMgr->mutex);

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

	deleteNode(lstMgr, n->id);
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

	pthread_mutex_lock(&lstMgr->mutex);
	//fprintf(stdout, "Lock Node\n");
	pthread_mutex_lock(&node->mutex);

	//fprintf(stdout, "Working\n");
	removeNode(lstMgr, node);

	//fprintf(stdout, "NO FREE DONE\n");
	pthread_mutex_unlock(&lstMgr->mutex);
	pthread_mutex_unlock(&node->mutex);

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
	Node* node = deleteNodeNoFree(lstMgr, id);
	pthread_mutex_lock(&node->mutex);

	if (node == NULL) {
		//fprintf(stdout, "NOt FOUND\n");
		return NULL;
	}
	//fprintf(stdout, "-- FOUND !!!\n");

	if (node->del != NULL) {
		node->del(node->value);
	}

	freeNode(node);

	//fprintf(stdout, "DELETE DONE\n");
	pthread_mutex_unlock(&node->mutex);
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
	pthread_mutex_lock(&node->mutex);
	if (node->valIsAlloc && node->value != NULL){
		free(node->value);
	}

	node->value = value;
	node->valIsAlloc = asAlloc;

	pthread_mutex_unlock(&node->mutex);
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
	pthread_mutex_lock(&lstMgr->mutex);

	do{
		if (currentNode == NULL){
			currentNode = lstMgr->first;
			pthread_mutex_lock(&currentNode->mutex);
		}
		else{
			currentNode = currentNode->next;
			pthread_mutex_lock(&currentNode->mutex);
		}

		ids[i++] = currentNode->id;
		pthread_mutex_unlock(&currentNode->mutex);
	}while(lstMgr->last != currentNode);

	pthread_mutex_unlock(&lstMgr->mutex);
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
	pthread_mutex_lock(&list->mutex);

	if (n != NULL) {
        pthread_mutex_lock(&n->mutex);
	}

	if (list->first == NULL){
        if (n != NULL) {
            pthread_mutex_unlock(&n->mutex);
        }

        pthread_mutex_unlock(&list->mutex);
		return  NULL;
	}
	else if (n == NULL) {
		n = list->first;
		pthread_mutex_unlock(&list->mutex);
		return n;
	}
	else if (n->next == NULL) {
        pthread_mutex_unlock(&n->mutex);
        pthread_mutex_unlock(&list->mutex);
		return NULL;
	}

    if (n != NULL) {
        pthread_mutex_unlock(&n->mutex);
    }

    pthread_mutex_unlock(&list->mutex);
	return n->next;
}


void listIterateFnc(ListManager* list, short (*fnc)(int , Node*, short*), Node* n) {
	pthread_mutex_lock(&list->mutex);

	int i = 0;
	short delete = 0;
	short process = 1;

	if (n == NULL) {
        n = list->first;
	}

	while (process && n != NULL) {
		pthread_mutex_lock(&n->mutex);

		Node* tmp = n;
		process = fnc(i++, n, &delete);
		n = n->next;

		pthread_mutex_unlock(&tmp->mutex);

		if (delete) {
			removeAndFreeNode(list, tmp);
		}

		delete = 0;
	}

	pthread_mutex_unlock(&list->mutex);
}


void listRevIterateFnc(ListManager* list, short (*fnc)(int , Node*, short*), Node* n) {
	pthread_mutex_lock(&list->mutex);

	int i = 0;
	short delete = 0;
	short process = 1;

	if (n == NULL) {
        n = list->last;
	}

	while (process && n != NULL) {
		pthread_mutex_lock(&n->mutex);

		Node* tmp = n;
		process = fnc(i++, n, &delete);
		n = n->prev;

		pthread_mutex_unlock(&tmp->mutex);

		if (delete) {
			removeAndFreeNode(list, tmp);
		}

		delete = 0;
	}

	pthread_mutex_unlock(&list->mutex);
}

Node* listRevIterate(ListManager* list, Node* n) {
	pthread_mutex_lock(&list->mutex);
	if (list->last == NULL){
        if (n != NULL) {
            pthread_mutex_unlock(&n->mutex);
        }
        pthread_mutex_unlock(&list->mutex);
		return NULL;
	}
	else if (n == NULL) {
		n = list->last;
		pthread_mutex_unlock(&list->mutex);
		return  n;
	}
	else if (n->prev == NULL) {
        pthread_mutex_unlock(&n->mutex);
        pthread_mutex_unlock(&list->mutex);
		return NULL;
	}

	pthread_mutex_unlock(&list->mutex);
    if (n != NULL) {
        pthread_mutex_unlock(&n->mutex);
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

	pthread_mutex_lock(&lst->mutex);

	pthread_mutex_lock(&n->mutex);
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


	pthread_mutex_unlock(&n->mutex);

	pthread_mutex_unlock(&lst->mutex);
	return 1;
}

void sortList(ListManager * lst, short (*fnc)(void*, void*)) {

	int i;
	short sort = 0;
	Node* tmp = NULL;
	Node* comp = NULL;

	pthread_mutex_lock(&lst->mutex);

	Node* key = lst->first->next;
	for (i = 1; i < lst->nodeCount; i++) {
    	comp = lst->first;
    	pthread_mutex_lock(&key->mutex);
    	pthread_mutex_lock(&comp->mutex);

		//fprintf(stdout, "##### Node %d: %s #####\n", key->id, key->name);

        do {
			//fprintf(stdout, "-- Compare %d: %s\n", comp->id, comp->name);
        	if (key == comp) {
				//fprintf(stdout, "-- Skipping\n");
        		comp = key->next;
        		pthread_mutex_unlock(&key->mutex);
        		pthread_mutex_unlock(&comp->mutex);
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

    				pthread_mutex_lock(&tmp->mutex);
					//fprintf(stdout, "-- Compare %d: %s\n", tmp->id, tmp->name);
					sort = fnc(comp->value, tmp->value);

					//fprintf(stdout, "-- Res: %d\n", sort);

    			    if (sort < 0) {
						listInsertAfter(lst, comp, tmp->id);
						listInsertAfter(lst, tmp, comp->id);

						comp = key;
						break;
    			    }

    			    pthread_mutex_unlock(&tmp->mutex);
					//fprintf(stdout, "+++++++++++++++++++++++++\n");
    			}

    			//printNodes(lst);
				//assert(0);
    		}


        	pthread_mutex_unlock(&comp->mutex);
    		comp = comp->next;
			//fprintf(stdout, "================================================\n");
        } while (comp != NULL);

		//fprintf(stdout, "--------------------------------------------------------------------\n");
		//
		pthread_mutex_unlock(&key->mutex);

		pthread_mutex_lock(&lst->mutex);

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
