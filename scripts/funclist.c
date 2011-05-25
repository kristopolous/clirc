#include"../cline/cline.h"
#define SIZE	1000

struct funct
{	char	*filename,
		*type,
		*funcname,
		*args;
};

struct funct allf[SIZE];
struct ccom     *g_irccoms;

int plist(void*nothing)
{
}

int main()
{	int n=0;
	g_irccoms=0;
	clinit(&g_irccoms);
	addcom(&g_irccoms,"list",NULL,plist,NULL,"List commands");
	while(cline(&g_irccoms,BLUE,NULL)!=DONE);
	return(1);
}
