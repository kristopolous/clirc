#ifndef _CONNECTION_
#define _CONNECTION_
#include"defines.h"

int cset(void*nothing)
{
	static struct qval*temp;	
	temp=g_curprompt->vars;
	g_curprompt->vars=gp_curcont;
	set(nothing);
	g_curprompt->vars=temp;
	return(TRUE);
}

int cadd(void*nothing)
{	
	static struct qval*temp;	
	temp=g_curprompt->vars;
	g_curprompt->vars=gp_curcont;
	add(nothing);
	g_curprompt->vars=temp;
	return(TRUE);
}

struct con*newcon()
{	
	static int 
		n=0, 
		port=6667;	

	struct con
		*ret=0;
	static struct qval*temp;

	g_con.curpoint=findcon(NULL, NULL, NOFOUND);
	n=g_con.curpoint;
	if(n>=g_con.size)
	{
		g_con.size=n+1;
	}
	
	g_con.cons[n]=malloc(sizeof(struct con));	
	memset(g_con.cons[n], 0, sizeof(struct con));
	ret=g_con.cons[n];
	ret->mynum=n;
	ret->vals=malloc(sizeof(struct qval));
	qassign(&ret->vals, "Host", QSTR, "localhost", 0);
	qassign(&ret->vals, "Port", QINT, &port, 1);
	qassign(&ret->vals, "Nick", QSTR, NULL, 2);
	qassign(&ret->vals, "connected", QBOL|QREAD, (void*)0, 3);
	qassign(&ret->vals, "username", QSTR, (void*)0, 4);
	qassign(&ret->vals, "hostname", QSTR, (void*)0, 5);
	qassign(&ret->vals, "nickname", QSTR, (void*)0, 6);
	qassign(&ret->vals, "realname", QSTR, (void*)0, 7);
	qsetfunc(&ret->vals, 2, setnick);
	ret->hosts=(ret->vals)->value[0];
	ret->nicks=(ret->vals)->value[2];
	ret->curhost=&(ret->hosts->curpoint);
	ret->curnick=&(ret->nicks->curpoint);
//Like OmG! This is such bad design!
	{
		if(g_curprompt)	
		{
			temp=g_curprompt->vars;
		}
		setcon(NULL, NULL, n, NULL);
		//now that it equals curc
		createbuffer("in", NOBUFFER);
		createbuffer("out", 0);

		if(g_curprompt)
		{
			g_curprompt->vars=temp;
		}
	}
	return(ret);
}

int showcon(void*nothing)
{	
	static int c = 0;

	for(c=0;c<g_con.size;c++)
	{	
		dospace(2);	
		if(curc==g_con.cons[c])
		{
			dospace(0);
			wprintf("*");
		}
	 	printvalue(itoa(c), g_con.cons[c]->hosts, QSTR, QMAR);
	}

	dospace(0);
	if(g_con.size)
	{
		message("Current Context", BLUE, 0, 0);
		qprint(&gp_curcont);
	}
	else
	{
		wprintf("No context created yet\n");
	}
	return(TRUE);
}

int findcon(char*server, char*nick, int number)
{	
	static int n = 0, 
		   m = 0;	
	
	if(server)
	{	
		m=strlen(server);	

		for(n=0;n<SMALL;n++)
		{
			if(!strncmp(server, g_con.cons[n]->hosts->data[*g_con.cons[n]->curhost], m))
			{
				return(n);
			}
		}
		return(NOFOUND);
	}
	else if(nick)
	{	
		m=strlen(nick);	
		for(n=0;n<SMALL;n++)
		{
			if(!strncmp(nick, g_con.cons[n]->nicks->data[*g_con.cons[n]->curnick], m))
			{
				return(n);
			}
		}
		return(NOFOUND);
	}
	else if(number==NOFOUND)
	{	
		for(n=0;n<SMALL;n++)
		{
			if(!g_con.cons[n])
			{
				return(n);
			}
		}
		return(NOFOUND);
	}
	else
	{
		return(number);
	}
	return(NOFOUND);
}

int conadd(void*in)
{
	static struct con*tomake;
	tomake=newcon();	
	return(TRUE);
}

int setcon(char*server, char*nick, int number, struct con*ctxt)
{	
	static int toset;	

	if(!ctxt)
	{
		toset=findcon(server, nick, number);	
		if(toset==NOFOUND)
		{
			return(FALSE);
		}
		curc=g_con.cons[toset];
	}
	else
	{
		curc=ctxt;	
	}
	if(!curc)
	{
		wprintf("Context %d doesn't exist. Making one...\n", number);
		conadd(NULL);
		setcon(server, nick, number, ctxt);
		return(FALSE);
	}
	gp_curcont=curc->vals;
	g_curprompt->vars=gp_curcont;
	return(TRUE);
}

int delcon(char*server, char*nick, int number)
{	
	static int todel;

	todel=findcon(server, nick, number);
	if(todel==NOFOUND)
	{
		return(FALSE);
	}
	free(&g_con.cons[todel]);
	g_con.cons[todel]=0;	
	g_con.curpoint=todel;
	return(TRUE);
}

//This conforms with the command prototype

int condel(void*in_)
{	
	static char*in;
	static int res;

	if((res=findcon(in, NULL, 0))&&(res!=NOFOUND))
	{
		delcon(in, NULL, 0);
	}
	else
	if((res=findcon(NULL, in, 0))&&(res!=NOFOUND))
	{
		delcon(NULL, in, 0);
	}
	else
	{
		return(FALSE);	
	}
	return(TRUE);
}

int context(void*var)
{	
	static int 
		res = 0, 
		ix = 0;
	char*in=(char*)var;
	
        qclear(&g_toget);
        qassign(&g_toget, "Context", QINT, NULL, 0);
        if(!query(in, &g_toget))
	{
                return(FALSE);
	}

	ix=(int)qnget(g_toget, 0);
	if((res=setcon(NULL, NULL, ix, NULL))&&(res!=NOFOUND))
	{	
		wprintf("Set context to %d\n", ix);	
		return(TRUE);	
	}
	return(FALSE);
}

#endif
