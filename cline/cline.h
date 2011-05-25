#ifndef _CLINEH_
#define _CLINEH_

#include<fcntl.h>
#include<netdb.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/poll.h>
#include<sys/socket.h>
#include<termios.h>
#include<time.h>
#include<unistd.h>

#define RED     1
#define GREEN   2
#define YELLOW  3
#define BLUE    4
#define MAGENTA 5
#define CYAN    6
#define WHITE   7
#define TRUE    1
#define DONE    2
#define FALSE   0
#define VAR	1
#define COLOR(x)        "\033[1;3xm"

#define CLON    1
#define CLOFF   0

#define QINT    1
#define QSTR    2
#define QDBL    3
#define QLNG    4
#define QBOL    5
#define QNL     6
#define QBUF    80
#define QMAR    15

struct ccom
{       unsigned char    *name;
        unsigned char    type;
        int     (*function)(void*);
        unsigned char    value;
        unsigned char    *description;
	void	*arg;
        struct  ccom*left;
        struct  ccom*right;
        struct  ccom*head;
};

int findcom(struct ccom*com,unsigned char*command,struct ccom**ret);
int addcom(struct ccom**com,unsigned char*name,int(*function)(void*),void*arg,unsigned char*desc);
int showcom(struct ccom*com);
int clexit(void*nothing);
int clshowcom(void*nothing);
int cline(struct ccom**_coms,unsigned char color,unsigned char*prpt);
int clineproc(struct ccom**_coms,unsigned char*cin);
void clinit(struct ccom**);
void setcolor(int col);
void printstring(unsigned char*str,int n);
void printvalue(unsigned char*name,void*value,int type,int spaces);
void message(unsigned char*message,int color,int level,int var);
void patience();

#include"cline.c"
#include"command.c"
#include"common.c"

#endif
