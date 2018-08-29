#ifndef __BASIC_C__
#define __BASIC_C__

int isNum(char c);
int isAlpha(char c);
int strIsNum(char* str);
int char2int(char ch);
int str2int(char* str);
int strIsAlpha(char* str);
int alnum(char* str);
void substr(char* str, int offset, int length, char** target);
char* concat(char* str1, char* str2, char* res);
int charCnt(char c, char* str, int limit);
int strpos(char c, char* str, int offset);
void explode(char c, char* str, int offset, int limit, char* res[]);

int numBase(int num);
char int2char(int a);
char* int2str(int num);
float percent(const float x, const float y);

#endif