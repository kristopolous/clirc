#ifndef _CLINE_
#define _CLINE_
#include"../defines.h"
	
struct 	pollfd clufds[1];
struct termios clorig, cltoch;
int	clstdin;
int 	clineproc(struct ccom**, char *);

int clexit(void*nothing)
{
	printf("\033[0m");	
	return(DONE);
}

int clshowcom(void*nothing)
{
	struct ccom*com = (struct ccom*)nothing;	
	showcom(com);
	return(TRUE);
}

int reset(void*nothing)
{
	close(clstdin);
	clstdin = open("/dev/stdin", O_RDONLY, O_NONBLOCK);
	tcgetattr(clstdin, &cltoch);
	tcgetattr(clstdin, &clorig);
	cltoch.c_lflag = ICANON&ECHO&ECHOCTL;
	clufds[0].fd = 0;
	clufds[0].events = POLLIN;
	return(TRUE);
}

void clinit(struct ccom**coms)
{ 	
	clstdin = open("/dev/stdin", O_RDONLY, O_NONBLOCK);
	tcgetattr(clstdin, &cltoch);
	tcgetattr(clstdin, &clorig);
	cltoch.c_lflag = ICANON&ECHO&ECHOCTL;
	clufds[0].fd = 0;
	clufds[0].events = POLLIN;
	addcom(coms, "quit", clexit, NULL, "Exit program or subshell");
//	addcom(coms, "exit", clexit, NULL, "Exit program or subshell");
	addcom(coms, "help", clshowcom, (void*)(*coms), "Show help");
}

//one of the big ideas is less overhead . . . more code, less time
int plaincline(char *in, struct server*toproc)
{ 	
	static int ret = 0, 
		   m = 0;	
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
	static char	**cin, ctmp;
	static int 	
		cur = -1,
   	c = -102913,
		csize = 64, 
		scroll = 0, 	
		prompt = 0,
		lc = 0,
		ret = 0;	

	if(c == -102913)//We have never been here before
	{	
		prompt = 0;
		scroll = 0;
		c = 0;
		cin = (char **)malloc(64*4);
		for(lc = 0;lc < 64;lc++)
		{	
			cin[lc] = malloc(64);
			memset(cin[lc], 0, 64);
		}
		lc = 0;
	}
	if(!prompt)
	{	
		tcsetattr(clstdin, TCSANOW, &cltoch);	
		if(prpt)
		{
			fprintf(stdout, "\033[0;3%dm[\033[1;3%dm%s\033[0;3%dm]", color, color, prpt, color);
		}
		printf("\033[1;34m>> \033[1;37m");
		fflush(stdout);
		cur++;
		if(cur == csize)
		{	
			csize = csize << 1;	
			cin = realloc(cin, csize);
			for(ret = csize >> 1;ret < csize;ret++)
			{	
				cin[ret] = malloc(64);
				memset(cin[ret], 0, 64);
			}
		}
		prompt = 1;
		printf("\033[s");
	}
	if(!(poll(clufds, 1, 200) && clufds[0].revents == POLLIN)) //Nothing to read
	{
		return(TRUE);
	}
	g_curs = stdoutin;
	ctmp = getc(stdin);
	if(ctmp == 27)
	{	
		getc(stdin);
		ctmp = getc(stdin);
		switch(ctmp)
		{	
			case 'A':
				if(cur - scroll)
				{
					scroll++;
				}
				break;

			case 'B':
				if(scroll > 0)	
				{
					scroll--;
				}
				break;

			case 'C':

				if(c >= lc)
				{
					break;
				}
				c++;
				printf("\033[1C");
				break;

			case 'D':
				if(c > 0)
				{	
					c--;
					printf("\033[1D");
				}
				break;
		}	
		if(ctmp == 'A' || ctmp == 'B')
		{	
			if(!scroll)	
			{	
				printf("\033[u\033[K\033[u");
				memset(cin[cur], 0, 64);
				strcpy(cin[cur], "");
			}
			else
			{	
				printf("\033[u%s\033[K", cin[cur - scroll]);
				printf("\033[u\033[%dC", strlen(cin[cur - scroll]));
				memset(cin[cur], 0, 64);
				strcpy(cin[cur], cin[cur - scroll]);
			}
			lc = c = strlen(cin[cur]);
		}
	}
	else 
	{	
		switch(ctmp)
		{	
			case '\t':
				if(c < 2)
				{
					strcpy(cin[cur], "help");
					ctmp = '\n';
					c = 1;
				}
				break;

			case '':	
				if(cur - scroll)	
				{
					scroll++;	
				}
				printf("\033[u%s\033[K", cin[cur - scroll]);
				printf("\033[u\033[%dC", strlen(cin[cur - scroll]));
				strcpy(cin[cur], cin[cur - scroll]);
				c = strlen(cin[cur]);
				break;

			case'':
				if(scroll > 0)
				{
					scroll--;
				}
				if(!scroll)
				{	
					printf("\033[u\033[K\033[u");
					strcpy(cin[cur], "");
				}
				else
				{	printf("\033[u%s\033[K", cin[cur - scroll]);
					printf("\033[u\033[%dC", strlen(cin[cur - scroll]));
					strcpy(cin[cur], cin[cur - scroll]);
				}
				c = strlen(cin[cur]);
				break;

			case 26:
				tcsetattr(clstdin, TCSANOW, &clorig);	
				raise(SIGSTOP);
				tcsetattr(clstdin, TCSANOW, &cltoch);	
				return(TRUE);
				break; 	

			case 3:
				printf("\n");
				return(DONE);
				break;

			case 8:
			case 127:
				if(c > 0)	
				{	
					strcpy(cin[cur] + c - 1, cin[cur] + c);
					c--;
					lc--;
					printf("\033[u%s\033[K\033[u\033[%dC", cin[cur], c);
					if(!c)
					{
						printf("\033[u");
					}
				}
				break;

			case'\n':
				break;	

			default:
				if(ctmp < 32)
				{
					break;
				}
				if(c < lc)//we are amidst the string
				{
					memmove(cin[cur] + c + 1, cin[cur] + c, lc - c);
					lc++;
				}
				cin[cur][c] = ctmp;
				c++;
				if(c > lc)
				{
					lc = c;
				}
				printf("\033[u%s\033[K\033[u\033[%dC", cin[cur], c);
				break;
		}
	}
	fflush(stdout);
	if(ctmp != '\n')	//If !EOC
	{
		return(TRUE);
	}

	printf("\n");
	tcsetattr(clstdin, TCSANOW, &clorig);	

	if(!c)
	{
		ret = clineproc(_coms, cin[cur]);
	}
	else 
	{
		cur--;
	}
	scroll = c = prompt = lc = 0;
	return(ret);
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
