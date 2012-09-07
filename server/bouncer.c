#ifndef _BOUNCER_
#define _BOUNCER_
#include"../defines.h"

int bouncewelcome(char*args, struct server*s, int m)
{ 
	static struct con*c;
	char*servname;

	if(args==NULL)
	{
		exthelp("bouncer");
		return(WAITING);
	}

	parse(&args, &g_out);

	if(strcmp(g_out[0], "server"))
	{
		wprintf("Error,  you must specify a server first!\n");
		return(WAITING);
	}
	servname=g_out[1];	
	if(!servname)
	{
		wprintf("Error,  no server specified!\n");
		return(WAITING);
	}
	c=newcon();
	if(c==NULL||g_out[1]==NULL)
	{	
		wprintf("Unable to connect to host %s\n", g_out[1]);	
		return(WAITING);
	}
	setcon(0, 0, -1, c);
	twovarset("host", servname);

	if(!ircconnect(0))
	{	
		wprintf("Oops,  couldn't connect to server!");
		return(WAITING);
	}
	serverassociate(s, c);
	return(DONE);
}

int bouncebuf(struct server*s)
{	
	static short d;	
	d=0;

	loop:
		if(*(s->buffers[d].size)>s->buffers[d].curpoint)
		{	s->buffers[d].curpoint++;	
		//	protected_write(s, (*(s->buffers[d].data))[s->buffers[d].curpoint]);
    } else	//buffers are updated
		{
			return(TRUE);
		}

		goto loop;
	//	d++;
	//	goto loop;
}

int bounce(char*args, struct server*s)
{
	static char	*toproc, 
			*statargs;

	statargs=malloc(strlen(args)+1);
	strcpy(statargs, args);

	if(!ircsend(statargs))
	{	
		s->inistatus[s->curcon]=WAITING;
	}
	if(toproc)
	{
		protected_write(s, toproc);
	}
	return(TRUE);
}

#endif
