#ifndef _SERVER_
#define _SERVER_
#include"../defines.h"

//Need to test if connected

int setpass(void*_in)
{	
	char*in=(char*)_in;
	qclear(&g_toget);
	qassign(&g_toget, "New Password:", QSTR, NULL, 0);
	qassign(&g_toget, "Confirm:", QSTR, NULL, 1);

	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	if(strcmp(qnget(g_toget, 0), qnget(g_toget, 1)))
	{
		message("Error,  passwords don't match.", RED, 0, LEVEL);
		return(FALSE);
	}
	qsetfunc(&g_remote, 3, 0);	//gets around a recursion bug
	qnset(&g_remote, 3, quenc(qnget(g_toget, 0)), QSTR);
	qsetfunc(&g_remote, 3, setpass);
	message("Password changed successfully!", BLUE, 0, LEVEL);
	return(TRUE);
}

struct server*create_server(	int port, 
				int(*inifunction)(char*, struct server*, int), 
				int(*function)(char*, struct server*), 
				int(*buffunction)(struct server*))
{	static struct server*todo;
	todo=malloc(sizeof(struct server));
	bigserver.servers[(int)bigserver.count]=todo;
	bigserver.count++;
	todo->port=port;
	todo->prifd=g_fd;
	todo->timeout=TIMEOUT;
	todo->function=function;
	todo->inifunction=inifunction;
	todo->buffunction=buffunction;
	todo->isactive=FALSE;
	todo->concount=0;
	todo->qvals=malloc(sizeof(struct qval));
	memcpy(&todo->svp, g_curprompt, sizeof(todo->svp));
	ZERO(todo->cur, MAX_CONS*4);
	ZERO(todo->curfd, MAX_CONS*4);
	ZERO(todo->last, MAX_CONS*4);
	ZERO(todo->isconnected, MAX_CONS);
	ZERO(todo->inistatus, MAX_CONS);
	g_fd++;
	return(todo);
}

struct server*create_stream(	char*file, 
				int(*inifunction)(char*, struct server*, int), 
				int(*function)(char*, struct server*), 
				int(*buffunction)(struct server*))
{	
	static struct server*todo;
	todo = malloc(sizeof(struct server));
	bigserver.servers[(int)bigserver.count] = todo;
	bigserver.count++;
//	todo->pri = open(file, O_RDWR);
	todo->prifd = g_fd;
	todo->timeout = TIMEOUT;
	todo->function = function;
	todo->inifunction = inifunction;
	todo->buffunction = buffunction;
	todo->isactive = FALSE;
	todo->concount = 0;
	g_fd++;
	g_ufds[todo->prifd].fd     = todo->pri;
        g_ufds[todo->prifd].events = POLLIN|POLLHUP|POLLERR|POLLNVAL;
	return(todo);
}

void set_timeout(struct server**_toset, int secs)
{
	struct server*toset=(*_toset);
	toset->timeout=secs;
	return;
}

void activate_server(struct server**todo_)
{ 
	static 	int		port=0;
	struct 	sockaddr_in 	name;
	struct	hostent 	*gethostbyaddr();
	struct 	server*todo	=(*todo_);
	todo->addr.sa_family=AF_INET;
	strcpy(todo->addr.sa_data, "somename");

	port=ntohs(todo->port);
	name.sin_family=AF_INET;
	name.sin_port=port;
//I think this is the accept address
	name.sin_addr.s_addr=INADDR_ANY;
	todo->pri=socket(PF_INET, SOCK_STREAM, 0);
	while(bind(todo->pri, (struct sockaddr*)&name, sizeof(name))<0)
	{	
		message("Could not bind to port: %d\n", BLUE, 1, LEVEL);
		name.sin_port+=htons(1);	
		message("Trying port %d...", GREEN, 1, LEVEL);
	}
	todo->port=ntohs(name.sin_port);
	todo->addrlen=sizeof(todo->addr);
	if((getsockname(todo->pri, &todo->addr, &todo->addrlen))<0)
	{
		message("Could not get socket name!", RED, 0, 0);
	}
	if(listen(todo->pri, 10)<0)
	{
		message("Could not listen on socket!", RED, 0, 0);
	}
	todo->isactive=TRUE;
	g_ufds[todo->prifd].fd    =todo->pri;
        g_ufds[todo->prifd].events=POLLIN|POLLHUP|POLLERR|POLLNVAL;
	return;
}

int server_stop(struct server*tostop)
{
	close(tostop->pri);	
	tostop->isactive=FALSE;
	return(TRUE);
}

//Poll main server and establish a new connection if necessary
void poll_server(struct server*topoll)
{
	static int c, p;
	g_curs=topoll;
  	if(!(poll(g_ufds, topoll->prifd+1, 0)
		&&g_ufds[topoll->prifd].revents))
	{
		return;
	}
	for(p=0;;p++)
	{
		if(topoll->isconnected[p]==FALSE)
		{
			break;
		}
	}
	if(p>topoll->concount)
	{
		topoll->concount=p;
	}
	c=accept(topoll->pri, 0, 0);
	getpeername(c, &topoll->addr, &topoll->addrlen);
	topoll->isconnected[p]=TRUE;
	topoll->cur[p]=c;
	topoll->curfd[p]=g_fd;
	g_fd++;
	g_ufds[topoll->curfd[p]].fd    =topoll->cur[p];
        g_ufds[topoll->curfd[p]].events=POLLIN|POLLHUP|POLLERR|POLLNVAL;
	topoll->last[p]=time(NULL);

	if(topoll->inifunction)
	{
		g_curs=topoll;
		topoll->curcon=p;	
		(topoll->inifunction)(NULL, topoll, p);
	}
	return;
}

void close_connection(struct server*toclose, int m)
{	
	toclose->isconnected[m]=FALSE;
	toclose->inistatus[m]=FALSE;
	shutdown(toclose->cur[m], SHUT_RD);
	shutdown(toclose->cur[m], SHUT_WR);
	close(toclose->cur[m]);
	ZERO(toclose->misc[m], 32);
	return;
}

//Poll an existing connection 
void poll_connection(struct server*topoll, int m)
{	
	static char*buf=0;
	static int c, d, t;
	if(!buf)
	{
		buf=malloc(MEDIUM);
	}
	g_curs=topoll;
	
	if(!(poll(g_ufds, topoll->curfd[m]+1, 0)
		&&g_ufds[topoll->curfd[m]].revents))
	{
		if((time(NULL)-topoll->last[m])>topoll->timeout)
		{
			close_connection(topoll, m);	
		}
		return;
	}
	c=topoll->cur[m];
	topoll->curcon=m;
//Checks for buffers
	if(topoll->buffers)
	{
		for(d = 0;d < TINY; d++)
		{
			if(!topoll->buffers[d].name)
			{
				break;	
			}
			t = *(topoll->buffers[d].size);	

			if(!t)
			{
				break;
			}
			if(topoll->buffers[d].curpoint < t)
			{	
				if(topoll->buffunction)
				{
					topoll->buffunction(topoll);
					break;
				}
			}
		}
	}
		
//Checks for input
	if(g_ufds[topoll->curfd[m]].revents==POLLIN)
	{//I don't think this cuts it . . .	
//		ZERO(buf, strlen(buf)+3);
		ZERO(buf, MEDIUM);
		read(c, buf, MEDIUM);
		if(topoll->logfd)
		{
			write(topoll->logfd,buf,MEDIUM);
		}
		topoll->last[m]=time(NULL);
		if((topoll->inifunction&&topoll->inistatus[m]==DONE)||!topoll->inifunction)
		{
			if(topoll->logfd>0)	/*Logging enabled*/
			{
				write(topoll->logfd,numtotime(time(0)),16);
				write(topoll->logfd,": ",2);
				write(topoll->logfd,buf,strlen(buf));
				write(topoll->logfd,"\r\n",2);
			}	
			switch((topoll->function)(buf,topoll))
			{
				case DONE:	
					close_connection(topoll,m);	
					break;
			}
		}
		else
		{
			switch((topoll->inifunction)(buf,topoll,m))
			{
				case DONE:	
					topoll->inistatus[m]=DONE;	
					break;
				case FAIL:
					close_connection(topoll, m);
					break;
			}
		}
	}
	return;
}


void poll_servers()
{
	static int n, m;
	static struct server*temp;

	for(n=0;n<bigserver.count;n++)
	{	
		if(bigserver.servers[n]->isactive)
		{
			temp=bigserver.servers[n];	
			poll_server(temp);
			for(m=0;m<=temp->concount;m++)
			{
				if(temp->isconnected[m])	
				{
					poll_connection(temp,m);
					wupdate();
				}
			}
		}
	}
	return;
}

char*getdata(int flag, int num)
{	static time_t m_time;
	static struct tm *gmt;
	static char*ret=0;
	if(!ret)
		ret=malloc(SMALL);
	ZERO(ret, SMALL);
	switch(flag)
	{	case DATE:
			time(&m_time);
			gmt=gmtime(&m_time);
			sprintf(ret, "%s,  %02d %s %d %02d:%02d:%02d GMT"
				, nl_langinfo(gmt->tm_wday+ABDAY_1)
				, gmt->tm_mday 
				, nl_langinfo(gmt->tm_mon+ABMON_1)
				, gmt->tm_year+1900, gmt->tm_hour
				, gmt->tm_min, gmt->tm_sec);
			break;
		case SERV:
			strcat(ret, "Clirc Phunkshun Survar");
			break;
		case LMOD:
			m_time=time(NULL);
			gmt=gmtime(&m_time);
			sprintf(ret, "%s,  %02d %s %d %02d:%02d:%02d GMT"
				, nl_langinfo(gmt->tm_wday+ABDAY_1)
				, gmt->tm_mday 
				, nl_langinfo(gmt->tm_mon+ABMON_1)
				, gmt->tm_year+1900, gmt->tm_hour
				, gmt->tm_min, gmt->tm_sec);
			break;
		case ACCE:
			strcpy(ret, "bytes");
			break;
		case CONL:
			sprintf(ret, "%d", num);
			break;
		case CONT:
			strcpy(ret, "text/html");
			break;
		default:
			break;
	}
	return(ret);
}


//The only thing that changes is the size
int get(char*args_, struct server*s)
{ 	
	static int place=0,  msg=0, a, toclear;
	static char*data=0, *start=0, *size, **args;
	place=msg=0;
	
	parse(&args_, &g_out);
	args=g_out;
//Start of header
	if(!data)
	{
		data=malloc(MEDLAR);
		start=malloc(MEDLAR);	
		ZERO(start, MEDLAR);
		strcpy(start, "HTTP/1.1 200 OK");
		for(place=0;place<CONL;place++)
		{	
			sprintf(start, "%s\n%s %s", 
			start, head[place].text, 
			getdata(place, 0));
		}
		size=start+strlen(start)-1;
		toclear=MEDLAR-(size-start)-4;
	}
	ZERO(size, toclear);
	ZERO(data, MEDLAR);

	if(!urlprint(args[1]+1, &data))
	{
		strcpy(data, "<HTML></HTML>");
	}
	a=strlen(data);

	for(place=CONL;head[place].text;place++)
	{	
		sprintf(size, "%s\n%s %s", 
			size, head[place].text, 
			getdata(place, a));
	}
	strcat(start, "\n\n");
	strcat(start, data);

	protected_write(s, start);
	return(TRUE);
}

//NOT SAFE
//tries needs to be on a per connection basis
int password(char*in, struct server*s, int m)
{
	static struct termios pterm, rterm;
	static short*tries;
	int fd;
	char*tocheck, *hash=qnget(g_remote, 3);
	if(!s->misc[m][0])
	{
		s->misc[m][0]=malloc(2);
	}
	tries=s->misc[m][0];
	if(!hash)	//if no password is set then we have an open-door policy
	{
		return(DONE);
	}
	switch(s->inistatus[m])
	{	case FALSE:
			(*tries)++;
			if((*tries)>3)
			 	return(FAIL);	
			wprintf("Password: ");
			fd=s->cur[s->curcon];
			tcgetattr(fd, &pterm);
			tcgetattr(fd, &rterm);
			pterm.c_lflag=ICANON&ECHO&ECHOCTL;
			tcsetattr(fd, TCSANOW, &pterm);	//Doesn't like to work
			s->inistatus[m]=WAITING;
			return(WAITING);
			break;
		case WAITING:
			if((*tries)>3)
			{
				return(FAIL);
			}
			fd=s->cur[s->curcon];
			tcsetattr(fd, TCSANOW, &rterm);
			in[strlen(in)-2]=0;
			tocheck=quenc(in);
			if(!strcmp(hash, tocheck))
			{	s->inistatus[m]=DONE;	
				(*tries)=0;
				exthelp("loginserver");
				return(DONE);	
			}
			else
			{	s->inistatus[m]=FALSE;	
				wprintf("***Invalid Password***\n");
				return(FALSE);
			}
			break;
		case DONE:
			break;
	}	
	return(TRUE);
}

//Symlinks the bufferpointer of s to the real buffers of c
//Should have some sanity checks so that we are not overwriting possible previous buffer links
int serverassociate(struct server*s, struct con*c)
{
	static int n;
#ifdef _DEBUG_
	wprintf("DEBUG: entering server associate\n");
#endif
	n=0;
	loop1:
		if(c->buffers[n].name)
		{
			s->buffers[n].name=&c->buffers[n].name;
			s->buffers[n].data=&c->buffers[n].data;
			s->buffers[n].size=&c->buffers[n].size;
			s->buffers[n].curpoint=0;
		}
		else
		{
			return(TRUE);
		}
		n++;
		goto loop1;
}
#endif
