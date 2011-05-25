#ifndef _COMMON_
#define _COMMON_
#include"cline.h"

int curcolor, precolor, prespace;

void setcolor(int col)
{
	precolor = curcolor;
	curcolor = col;
}

void revertcolor() 
{
	curcolor = precolor;
}

void printstring(char*str, int n)
{
	if(prespace != NULL)
	{
		printf("[%dC", prespace);	
	}
	if(n != NULL) 	
	{
		printf("[0;31m[%d]:", n);
	}
	printf("[1;3%dm%s\n", curcolor, str);
	return;
}

void dospace(int n) 
{
	prespace = n;
}

void printvalue(char*name, void*value, int type, int spaces)
{	
	int t = 0;	

	printf("[%dC[1;3%dm", prespace, curcolor);
	if(name == NULL)
	{	
		name = malloc(20);
		strcpy(name, "<<UNKNOWN>>");
	}	
	if(*name>'Z')
	{
		printf("%c%s", *name - 32, (name + 1));
	}
	else
	{
		printf("%s", name);
	}
	printf("[%dC[1;33m", spaces - strlen(name));
	switch(type)
	{ 	
		case QBOL:
			t = (int)value;
			if(t)
			{
				printf("True");
			}
			else
		 	{
				printf("False");
			}
			break;

		case QINT: 
			printf("%d", (int)value);
			break;

		case QSTR:
			printf("%s", (char*)value);
			break;

		case QNL:
			break;

		default:
			fflush(stdout);
			return;
			break;
	}
	printf("\n");
	fflush(stdout);
	return;
}

void message(char*message, int color, int level, int var)
{ 
	if(var >= level)
	{
		if(color != -1)	
		{
			setcolor(color);
		}
		printstring(message, level);
		if(color != -1)
		{
			revertcolor();
		}
	}
	return;
}

void patience()
{	
	static char loop[] = "-\\|/";
	static char pos = 0;

	pos++;
	pos %= 4;
	printf("[1D%c", loop[(int)pos]);
	fflush(stdout);
	return;
}
#endif	
