#ifndef _CHANNEL_
#define _CHANNEL_
#include "../defines.h"

int chat(void*channel)
{	char	*in=(char*)channel,
		*mes=CHR(SMALL),
		*tosend=CHR(SMALL);
	struct qval*toget=malloc(sizeof(struct qval));
	qassign(&toget,"Channel",QSTR,NULL,0);
	if(!query(in,&toget))
			return(FALSE);
	strcpy(in,(char*)qnget(toget,0));
	message("Entering chat mode, press escape, then enter to exit\n",-1,1,LEVEL);
	while(1)
	{	getstuff(PRINT);	
		if(poll(g_ufds,2,100)&&g_ufds[1].revents==POLLIN)
		{	mes=fgets(mes,SMALL,stdin);
			if(*mes==27)
				return(TRUE);
			sprintf(tosend,"PRIVMSG %s :%s",in,mes);
			ircsend(tosend);
		}
	}
	return(FALSE);
}

int join(void*channel)
{	char	*in=(char*)channel;
	static char*chan=0;
	if(g_canjoin==FALSE)
	{	message("The server says: ACKKK! You are in too many channels",RED,0,0);
		return(FALSE);
	}
	qclear(&g_toget);
	qassign(&g_toget,"Channel",QSTR,NULL,0);
	if(!query(in,&g_toget))
		return(FALSE);
	chan=malloc(TINY);
	ZERO(chan,TINY);
	sprintf(chan,"join %s\n",(char*)qnget(g_toget,0));
	ircsend(chan);
	message("Joining channel",-1,1,LEVEL);
	return(TRUE);
}

int part(void*channel)
{	char	*in=(char*)channel,
		*chan=CHR(TINY);
	struct qval*toget=malloc(sizeof(struct qval));
	qassign(&toget,"Channel",QSTR,NULL,0);
	if(!query(in,&toget))
		return(FALSE);
	sprintf(chan,"part %s\n",(char*)qnget(toget,0));
	ircsend(chan);
	message("Parting channel",-1,1,LEVEL);
	g_canjoin=TRUE;
	return(TRUE);
}

int affinity(void*var)
{ 	static int n;
	hbuild();
	for(n=1;n<c_hnames;n++)
	{ 	whois(hnames[n]);
		patience();
	}
	printf("[1D");
	return(TRUE);
}

int list(void*nothing)
{	static char*query="list\n\r";
	ircsend(query);	
	return(TRUE);
}

int populate(void*nothing)
{	static char    *in,
                       *chan;
	static short m,n;
	in=(char*)nothing;
	qclear(&g_toget);
        qassign(&g_toget,"# of channels",QINT,NULL,0);
        if(!query(in,&g_toget))
                return(FALSE);
	for(n=(int)qnget(g_toget,0);n;n--)
	{	chan=malloc(10);	
		chan[0]='#';
		for(m=1;m<10;m++)	
			chan[m]=rand()%26+'A';
		if(!join(chan))
			return(FALSE);
		getstuff(SAVE);
	}
	return(TRUE);
}

#endif
