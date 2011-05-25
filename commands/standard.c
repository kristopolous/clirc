#ifndef _STANDARD_
#define _STANDARD_
#include"../defines.h"

int subshell(void *nothing)
{
	char*query=(char*)g_void;	//Using the g_void because the parameter (subshell) is already there
	struct ccom*tosub=(struct ccom*)nothing;
	pushprompt(tosub->subvars, tosub->sub, tosub->name, RED);

	if(strlen(query))
	{
		clineproc(&tosub->sub, query);
		popprompt();
	}
	return(TRUE);
}

int ctcp(void *nothing)
{
	char*in=(char*)nothing;
	static char*tosend=0;

	if(!tosend)
	{
		tosend=malloc(SMALL);
	}
	qclear(&g_toget);
	qassign(&g_toget, "To whom", QSTR, NULL, 0);
	qassign(&g_toget, "Command", QSTR, NULL, 1);
	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	sprintf(tosend, "PRIVMSG %s :\001%s\001\r\n", 
		(char*)qnget(g_toget, 0), (char*)qnget(g_toget, 1));
	ircsend(tosend);
	return(TRUE);
}

int exthelp(void *nothing)
{	
	char*in=(char*)nothing;
	static char*ret;

	qclear(&g_toget);
	qassign(&g_toget, "Command", QSTR|QOPTIONAL, NULL, 0);
	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	godark();
	ret=gethelp((char*)qnget(g_toget, 0));
	if(ret)
	{
		message(ret, CYAN, 0, 0);
	}
	golight();	
	return(TRUE);
}

int clear(void *nothing)
{ 	
	char*in=(char*)nothing;
	qclear(&g_toget);
	qassign(&g_toget, "Buffer", QSTR, NULL, 0);
	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	clearbuffer((char*)qnget(g_toget, 0));
	return(TRUE);
}

int print(void *nothing)
{	
	char*in=(char*)nothing;
	qclear(&g_toget);
	qassign(&g_toget, "Buffer", QSTR, NULL, 0);

	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	printbuffer((char*)qnget(g_toget, 0));
	return(TRUE);
}

// Do I disconnect from all networks?
int ircdisconnect(void *null)
{ 	
	if(!gp_curcont)	
	{
		return(TRUE);
	}
	if((int)qnget(gp_curcont, 3))
	{ 
		shutdown(g_sd, SHUT_RDWR);
		close(g_sd);
		qset(&gp_curcont, "connected", 0, QVAL);
	}
	if(curc)
	{
		g_ufds[curc->ufd].revents=0;
		g_ufds[curc->ufd].fd=-1;
	}
	return(TRUE);
}

void ircreg(void)
{ 
	ircsend(rprintf("user %s %s %s %s \r\n", 
			qnget(gp_curcont, 4), 
			qnget(gp_curcont, 5), 
			qnget(gp_curcont, 6), 
			qnget(gp_curcont, 7)));

 	ircsend(rprintf("nick %s \r\n", 
			qnget(gp_curcont, 2)));

	ircsend("\r\n");
}

int ircconnect(void *null)
{ 	
	struct sockaddr_in	name;
	struct hostent	
		*hp,  
		*gethostbyname();

	int 	c=htons((int)qget(gp_curcont, "port")), 
		count=(int)qnget(g_ircvar, 9);

	char	nick[9], *temp;
	
//	static struct con*	pcon;
//	pcon=newcon();
	
	if(curc == 0)
	{
		curc=malloc(sizeof(struct con));
		curc->ufd=g_fd;
		g_fd++;
	}

	g_sd		=socket(AF_INET, SOCK_STREAM, 0);
	g_ufds[curc->ufd].fd	=g_sd;
	g_ufds[curc->ufd].events=POLLIN|POLLHUP|POLLERR|POLLNVAL;
	name.sin_family	=AF_INET;
	hp		=gethostbyname(qnget(gp_curcont, 0));
	name.sin_port	=c;

	if(!hp)
	{
		return(FALSE);
	}
	memcpy(&name.sin_addr.s_addr, hp->h_addr, hp->h_length);
	while(connect(g_sd, (const struct sockaddr *)&name, sizeof(name))<0)
	{
		mainloop();
		if(count-- == 0)
		{
			wprintf("[1D");
			message("Failed to connect!", RED, 1, LEVEL);
			loop1:
				if(incrunregbuffer((struct buffers_*)gp_curcont->value[0]))
				{
					if(ircconnect(NULL))
					{
						break;
					}
					goto loop1;
				}
			qset(&gp_curcont, "connected", FALSE, QVAL);
			return(FALSE);	
		}
		patience();
	}

	qset(&gp_curcont, "connected", (void *)WAITING, QVAL);
	message("Connected", MAGENTA, 1, LEVEL);

	for(count=2;count<8;count++)
	{	
		if(count == 3)
		{
			continue;
		}
		if(!(temp=qnget(gp_curcont, count)))
		{
			ZERO(nick, strlen(nick));
			for(c=0;c<8;c++)
			{
				nick[c]=rand()%26+65;
			}
			nick[c]=0;
			qnset(&gp_curcont, count, &nick, QSTR);
		}
	}
	ircreg();
	return(TRUE);
}

int set(void *var)
{	
	char*in=(char*)var;
	if(!(g_curprompt->vars))
	{
		wprintf("Need to create a context\n");	
		return(FALSE);
	}

	qclear(&g_toget);
	qassign(&g_toget, "Variable", QSTR, NULL, 0);
	qassign(&g_toget, "Value", QSTR, NULL, 1);
	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	if(!qset(&g_curprompt->vars, qnget(g_toget, 0), qnget(g_toget, 1), QSTR))
	{
		wprintf("Unable to set variable %s\n", (char*)qnget(g_toget, 0));	
		return(FALSE);
	}
	return(TRUE);
}

int twovarset(char*toset, char*var)
{	
	if(!qset(&g_curprompt->vars, toset, var, QSTR))
	{	
		wprintf("Unable to set variable %s\n", (char*)qnget(g_toget, 0));	
		return(FALSE);
	}
	return(TRUE);
}

int shift(void *var)
{	
	char*in=(char*)var;
	static short wrt;
	qclear(&g_toget);
	qassign(&g_toget, "Variable", QSTR, NULL, 0);
	qassign(&g_toget, "Shift", QINT, NULL, 1);
	qassign(&g_toget, "Relative", QBOL|QOPTIONAL, NULL, 2);

	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	if(qnget(g_toget, 2) != NULL)
	{
		wrt=RELATIVE;
	}
	else
	{
		wrt=ABSOLUTE;
	}
	qmove(&g_curprompt->vars, qnget(g_toget, 0), (int)qnget(g_toget, 1), wrt);
	return(TRUE);
}

int add(void *var)
{	
	char*in=(char*)var;
	if(!(g_curprompt->vars))
	{
		wprintf("Need to create a context\n");	
		return(FALSE);
	}
	qclear(&g_toget);
	qassign(&g_toget, "Variable", QSTR, NULL, 0);
	qassign(&g_toget, "Value", QSTR, NULL, 1);
	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	if(!qadd(&g_curprompt->vars, qnget(g_toget, 0), qnget(g_toget, 1), QSTR))
	{
		wprintf("Unable to set variable %s\n", (char*)qnget(g_toget, 0));	
		return(FALSE);
	}
	return(TRUE);
}

int show(void *nothing)
{	
	char*in=(char*)nothing;
	static int n;
	qclear(&g_toget);
	qassign(&g_toget, "Set", QSTR|QOPTIONAL, NULL, 0);
	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	if((char*)qnget(g_toget, 0))
	{	
		for(n=0;n<qmain.size;n++)
		{
			dospace(0);	
			message(qmain.names[n], BLUE, 0, 0);
			dospace(1);
			qprint(&qmain.qvals[n]);
		}
		dospace(0);	
		message("Current Context", BLUE, 0, 0);
		dospace(1);
		qprint(&gp_curcont);
		dospace(0);
	}
	else
	{
		qprint(&g_curprompt->vars);
	}
	return(TRUE);
}

int raw(void *var)
{ 	
	static char *out=0;
	if(!out)
	{
		out=malloc(MEDIUM);
	}
	loop:	
		//message(getstuff(RAW), BLUE, 0, 0);	
		poll_servers();
		if(poll(g_ufds, MAX_FDS, g_polltime)&&g_ufds[g_fdstdin].revents == POLLIN)
		{
			fgets(out, SMALL, stdin);	
			if(*out == 27)
			{
				return(TRUE);
			}
			ircsend(out);
		}
		goto loop;	
}

int showmesg(void *nothing)
{	
	static int 
		n, 
		m, 
		margin=QMAR+5;

	if((int)qnget(g_ircvar, 8) == 0)
	{	
		message("There are No Messages because Message Log is Turned Off!", RED, 0, 0);
		return(0);
	}
	for(n=0;n<501;n++)
	{
		if(g_msg[n].message == 0)
		{
			continue;	
		}
		switch(g_msg[n].n)
		{
			case 0:	
				break;
			case 1:	
				printvalue(itoa(n), g_msg[n].message[0], QSTR, margin);
				break;
			default:
				message(itoa(n), -1, 0, 0);
				dospace(5);

				for(m=0;m<g_msg[n].n;m++)
				{
					wprintf("     [0m%d", g_msg[n].times[m]);
					message(g_msg[n].message[m], YELLOW, 0, 0);
				}
				dospace(0);
				break;
		}
	}
	return(FALSE);
}

void whois(void *var)
{	static char*tosend=0;
	tosend=malloc(TINY);
	strcpy(tosend, "whois ");
	strcat(tosend, (char*)var);
	strcat(tosend, "\r\n");
	ircsend(tosend);
	return;	
}

int ircsleep(void *var)
{	
	char	*in=(char*)var; 	
	struct 	qval*toget=malloc(sizeof(struct qval));
	qassign(&toget, "microseconds", QINT, NULL, 0);
	if(!query(in, &toget))
	{
		return(FALSE);
	}
	SLEEP	
	return(TRUE);
}

//Must be able to identify scripts and variable lists
//Test the names of each of the variable lists and if they aren't there,  then assume context.  Or,  check for context first!
int load(void *var)
{	
	char	*in=(char*)var, 
		*data=CHR(LARGE*2), 
		*comb;
	static char**parsed;
	int 	n=0, 
		config, 
		size;
	struct qval*toget=malloc(sizeof(struct qval));

	if(!parsed)
	{
		parsed=malloc(4*LARGE);
	}
	qassign(&toget, "File", QSTR, NULL, 0);
	if(!query(in, &toget))
	{
		return(FALSE);
	}
	config=open(qget(toget, "File"), 0);
	if(!config)
	{	
		message("Invalid file", -1, 0, 0);
		return(FALSE);
	}

	size=read(config, data, LARGE*2);
	data[size-1]=0;
	parse(&data, &parsed);
	n=0;
	while(parsed[n])
	{
		if(*parsed[n] == '#')
		{
			if(!(n=nextline(parsed, n)))break;
		}
		else if(*parsed[n] == '!')
		{	
			if(!strcmp(parsed[n]+1, "vars"))
			{
				if(!(n=nextline(parsed, n)))
				{
					break;
				}
				while(*parsed[n] != '!')
				{
					if(*parsed[n] != '#'&&*parsed[n] != '\n')	
					{
						qset(&g_ircvar, parsed[n], (void *)parsed[n+1], QSTR);
					}
					if(!(n=nextline(parsed, n)))
					{
						break;	
					}
				}
			}
			else if(!strcmp(parsed[n]+1, "coms"))
			{	
				if(!(n=nextline(parsed, n)))
				{
					break;
				}
				while(*parsed[n] != '!')
				{	
					if(!combine(parsed, n, &comb))
					{
						break;
					}
					if(*comb != '#')
					{
						if((clineproc(&g_irccoms, comb)) == FALSE)
						{
							message(comb, CYAN, 0, 0);	
							message("Last command failed! Exiting Script . . .", RED, 0, 0);
							n=0;
							break;
						}
					}
					if(!(n=nextline(parsed, n)))
					{
						break;
					}
				}
			}
			else if(!(n=nextline(parsed, n)))
			{
				break;
			}
		}
		else if(!(n=nextline(parsed, n)))
		{
			break;
		}
		if(!n)
		{
			break;
		}
	}
	return(FALSE);
}

int setnick(void *nothing)
{
	char*tosend = 0;	

	tosend = CHR(SMALL);
	sprintf(tosend, "nick %s\r\n", (char*)qnget(gp_curcont, 2));

	if((int)qget(gp_curcont, "connected") == TRUE)
	{
		ircsend(tosend);
	}
	return(TRUE);
}

int openlog(void *nothing)
{
	g_log=fopen(qnget(g_ircvar, 6), "a");
	return(TRUE);
}

int script_print(void *nothing) 
{
	message((char*)nothing, WHITE, 0, 0);
	return(TRUE);
}

#endif
