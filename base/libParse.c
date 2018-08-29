#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>

#include "basic.h"
#include "libParse.h"


int verifyParam(char* p, char* type){
	//fprintf(stderr, "verifyParam: %s  |  %s\n", p, type);
	if(!strcmp("alnum", type)){
		return alnum(p);
	}
	else if(!strcmp("alpha", type)){
		return strIsAlpha(p);
	}
	else if(!strcmp("num", type)){
		return strIsNum(p);
	}
	else{
		return 1;
	}
}

int callArg(ListManager* lstMgr, char* name, void* val){
	//fprintf(stderr, "==== CALL ARG: %s ====\n", name);
	Arg* arg = NULL;
	// printB("Calling Arg");
	// printB(name);
	Node* node = getNodeByName(lstMgr, name);
	if (node == NULL){
		//fprintf(stderr, "==== NOT FOUND ARG: %s\n", name);
		return 0;
	}

	arg = (Arg*) node->value;
	//fprintf(stderr, "==== CALL ARG FNC: %s\n", name);
	if(arg->hasParam){

		if (arg->asInt)
		{
			if (arg->defParam != NULL){
				if (val == NULL){
					arg->function(str2int(arg->defParam));
				}
				else{
					arg->function(str2int(arg->defParam), (*(int*)val));
				}
			}
			else{
				arg->function((*(int*)val));
			}
		}
		else{
			if (arg->defParam != NULL){
				if (val == NULL){
					arg->function((char*) arg->defParam);
				}
				else{
					arg->function(arg->defParam, (char*)val);
				}
			}
			else{
				arg->function((char*)val);
			}
		}
	}
	else{
		arg->function();
	}

	// printB("Calling Arg DONE");
	return 1;
}

int parseArgs(ListManager* lstMgr, int argc, char* argv[]){
	//fprintf(stderr, "==== PARSING ARGs ====\n");
	int i;
	int len;
	char* arg;
	Node* node = NULL;
	Arg* val = NULL;
	int v;

	for (i = 1; i < argc; ++i){
		if (!isArgument(argv[i])){
			continue;
		}

		len = strlen(argv[i]);
		arg = malloc(len+1);
		substr(argv[i], 1, len, &arg);

		//fprintf(stderr, "######  CMD: %s #####\n", arg);
		//fprintf(stderr, "######  PARAM: %s\n", argv[i+1]);
		node = getNodeByName(lstMgr, arg);

		if (node == NULL)
		{
			//fprintf(stderr, "==== IS NULL\n");
			free(arg);
			continue;
		}

		//fprintf(stderr, "==== NODE ARGs: %s ====\n", node->name);
		val = (Arg*) node->value;
		if(val->hasParam){
			//fprintf(stderr, "==== HAS PARAM\n");
			if (isArgument(argv[i+1]))
			{
				//fprintf(stderr, "==== FAIL ARG\n");
				free(arg);
				return -1;
			}

			if (val->asInt)
			{
				//fprintf(stderr, "==== AS INT\n");
				if (!strIsNum(argv[i+1]))
				{
					free(arg);
					return -1;
				}

				v = str2int(argv[++i]);

				if (val->defParam != NULL){
					val->function(val->defParam, &v);
				}
				else{
					val->function(v);
				}
			}
			else{
				//fprintf(stderr, "==== AS CHAR\n");
				if (!verifyParam(argv[i+1], val->type))
				{
					//fprintf(stderr, "==== FAIL VERIFY PARAM\n");
					free(arg);
					return -1;
				}

				if (val->defParam != NULL){
					//fprintf(stderr, "==== NO DEFAULT\n");
					val->function(val->defParam, argv[i+1]);
				}
				else{
					//fprintf(stderr, "==== DEFAULT\n");
					val->function(argv[i+1]);
				}

				i++;
			}
		}
		else if (val->defParam != NULL){
			//fprintf(stderr, "==== DEFAULT PARAM\n");
			val->function(val->defParam);
		}
		else{
			//fprintf(stderr, "==== NO PARAM\n");
			val->function();
		}

		free(arg);
	}

	return 1;
}

int isArgument(char* str){
	return str[0] == '-';
}

ListManager* defineArgs(Arg** params){
	int i;
	Node* arg = NULL;
	ListManager* lstMgr = initLibParse();

	for (i=0; params[i] != NULL; ++i){
		arg = addNode(lstMgr, params[i]->name);
		setValue(arg, params[i], 0);
	}

	return lstMgr;
}

ListManager* initLibParse(){
    return initListMgr();
}
