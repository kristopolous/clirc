#ifndef _CLINE_
#define _CLINE_
#include"../defines.h"

int 	clineproc(struct ccom**, char *);

int clexit(void*nothing)
{
	return(DONE);
}

int clshowcom(void*nothing)
{
	struct ccom*com = (struct ccom*)nothing;	
	showcom(com);
	return(TRUE);
}

void clinit(struct ccom**coms)
{ 	
	addcom(coms, "quit", clexit, NULL, "Exit program or subshell");
	addcom(coms, "help", clshowcom, (void*)(*coms), "Show help");
}

int plaincline(char *in, struct server*toproc)
{ 	
	static int ret = 0, m = 0;	
	static char *p = 0;
	static struct curprompt_*prev;

	prev = g_curprompt;
	g_curprompt = &toproc->svp;
	m = strlen(in);

	if(!m)
	{
		return(TRUE);
	}
	p = in + strlen(in);
	while(*p < 33 && p != in)
	{
		*p = 0;
		p--;
	}
	if(p == in)
	{
		return(TRUE);
	}
	ret = clineproc(&g_irccoms, in);
	g_curprompt = prev;
	return(ret);
}

int cline(struct ccom**_coms, char color, char *prpt)
{ 	
	static char buf[SMALL];
	char *p;

	if(prpt)
	{
		printf("[%s]>> ", prpt);
	}
	else
	{
		printf(">> ");
	}
	fflush(stdout);

	g_curs = stdoutin;

	if(!fgets(buf, SMALL, stdin))
	{
		return(DONE);
	}

	p = buf + strlen(buf) - 1;
	while(p >= buf && (*p == '\n' || *p == '\r'))
	{
		*p = 0;
		p--;
	}

	if(!*buf)
	{
		return(TRUE);
	}

	return(clineproc(_coms, buf));
}

//don't make elements static in order to support nested clines
int clineproc(struct ccom**_coms, char *cin)
{	
	int 	res = 0, 
		space = 0, 
	 	len, 
		ret = 0;
	static struct ccom*com;
	struct		ccom*coms = (*_coms);

	len = strlen(cin);

	while(space < len)	//This essentially translates it into a 1 word command.
	{	
		if(cin[space] == 32)
		{	
			cin[space] = 0;
			break;
		}
		space++;
	}	
	res = findcom(coms, cin, &com);

	switch(res)
	{	
		case 0:	
			message("No such command", RED, 0, 0);
			break;

		case 1:	
			if(!len)
			{
				return(42);
			}
			if(com->function != NULL)
			{	
				g_void = (void*)(&cin[space + 1]);	//Set a global arg just in case function needs args	
				if(com->arg == NULL)
				{
					ret = (com->function)((void*)(&cin[space+1]));
				}
				else
				{
					ret = (com->function)(com->arg);
				}
			}
			break;	
		default: break;
	}
	for(;space < len;space++)
	{
		if(!cin[space])
		{
			cin[space] = 32;
		}
	}
	fflush(stdout);
	return(ret);
}
#endif
