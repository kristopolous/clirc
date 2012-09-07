#ifndef _GETTHINGS_
#define _GETTHINGS_
#include"defines.h"

/* getuser
 * translates :bofo!~hobo@host.name
 * to an instance of struct type user
 * Also tries to be blazingly fast via pointers.
 */
void getuser(char*t_in,struct user*ret)
{	
	char * in = t_in;

	in++; //dump colon
	ret->name=in;
	for(;;in++)
	{
		switch(in[0])
		{
			case '!':
			case ':':
			case 0:
				break;
			default:
				continue;
		}
		break;
	}
	*in=0; //dump !

	in++;
	ret->usr=in;

	for(;;in++)
	{
		switch(in[0])
		{
			case '!':
			case ':':
			case '@':
			case 0:
				break;
			default:
				continue;
		}
		break;
	}
	*in=0; //dump @
	++in;
	ret->host=in;
	g_temp=1;
	return;
}

void ungetuser(struct user*ret)
{
	if(!g_temp)	
	{
		return;
	}

	if(ret->usr != NULL)
	{
		ret->usr[-1]='!';
		ret->host[-1]='@';
	}
}

char*combmessage(char**in,int n)
{	static char*ret;
	ret=in[n];
	for(;;)
	{	n++;	
		if(!in[n])
			break;
		else if(*in[n]=='\n')
			break;
		else if(*in[n]<32)
			in[n-1][strlen(in[n-1])]=*in[n];
		else if(in[n][-1]);
		else in[n][-1]=' ';
	}
	return(ret);
}

//Starting at [start] it should return a string until the newline
//Assumes the new parser is in effect which just places pointers
//Tries to be blazingly fast
char*getmessage(char**in,int n)
{ 	loop1:
		switch(*in[n])
		{	case':':break;
			case'\n':
				return(in[n-1]);
				break;	
			default:
		 		n++;
				if(!in[n])
					return(NULL);
				goto loop1;
				break;
		}
	in[n]=in[n]+1;	
	return(combmessage(in,n));
}

#define MAXBUF	15	
struct buffercluster
{	
	char buffers[MAXBUF+1][MEDIUM];
};

void getstuff(int todo)
{	
	char 		buftoput[32],
		*toprint=0, 	*comd,	*info,
 		*chan, 		*ctmp,	*temp=0,
	       	**t_out,	*buf;
	struct user ret;
	static struct errdata*erd;
	static int 	nxl,	
		prl, 	n_com,
		n=0, 	prl1,
		tmp, 	tmp1;
	static unsigned short 	saveit,
				counter=MAXBUF;
	static struct buffercluster*bufmain;
	//Not even connected
	if(!qnget(gp_curcont,3))
	{
		return;
	}
	if(poll(g_ufds,curc->ufd+1,g_polltime))
	{
		if(g_ufds[curc->ufd].revents&badstuff)
		{
			ircdisconnect(NULL);

			if(qnget(g_ircvar,11))
			{
				usleep(g_backoff);
				ircconnect(NULL);
				g_backoff*=2;
			}
		}	
	}else
	{
		return;
	}

	temp=malloc(SMALL);
	
	if(counter>=MAXBUF)
	{	
		bufmain=malloc(sizeof(struct buffercluster));
		counter=0;
	}else
	{
		counter++;
	}

	buf=bufmain->buffers[counter];
	ZERO(buf,MEDIUM);
	ZERO(&ret,(sizeof(struct user)));
	toprint=malloc(MEDIUM);	
	saveit=TRUE;
	strcpy(buftoput,"in");
	recv(g_sd,buf,MEDIUM,0);
//	realloc(buf,strlen(buf)+2);
	if(todo==RAW)
	{
		wprintf("%s",buf);
		addtobuffer(buftoput,buf);
	}
	parse(&buf,&g_out);
	nxl=0;
	ctmp=0;
	prl=0;
	info=NULL;
	if(!g_out[0])
	{
		free(temp);
		free(toprint);
		return;
	}
	while(1)
	{	nxl=nextline(g_out,prl);
		prl1=prl+1;
		t_out=g_out+prl1;
		if(!*t_out)
		{
			break;
		}
		comd=*t_out;
		if(!comd)
		{
			break;
		}
		chan=t_out[1];
		n_com=atoi(comd);
		if(!strcmp(g_out[prl],"PING"))
		{ 
			sprintf(toprint,"PONG %s\r\n",g_out[prl+1]);
			ircsend(toprint);
			return;
		}
		else if(n_com)//since 0 is not a valid command
		{
			switch(n_com)
			{
				case RNAMREPLY:	//people in the channel
					ctmp=t_out[3]; //channel name
					for(tmp=prl1+4;g_out[tmp]&&*g_out[tmp]>31;tmp++)
					{
						for(tmp1=0;(g_out[tmp][tmp1]=='@')||
							   (g_out[tmp][tmp1]==':')||
							   (g_out[tmp][tmp1]=='+');tmp1++);
						hashadd(ctmp,g_out[tmp]+tmp1,NULL);
						if(!hashfind(g_out[tmp]+tmp1))
						{
							whois(g_out[tmp]+tmp1);
						}
					}
					break;
				case RENDOFNAMES:
					break;
				case RTOPIC:
					addinfo(t_out[2],getmessage(t_out,2));
					break;
				case RLIST:	//done in a list command
					singlehashadd(t_out[2],combmessage(t_out,3));
					break;
				case ETOOMANYCHANNELS:
					g_canjoin=FALSE;
					break;
				case ENOTREGISTERED:
					ircreg();
					break;
				case RWHOISUSER:
					info=combmessage(t_out,3);
					addinfo(t_out[2],info);
					break;
				case RWHOISCHANNELS: //The whois response
					ctmp=t_out[2]; //username
					for(tmp=prl1+3;g_out[tmp]&&*g_out[tmp]>31;tmp++)
					{	for(tmp1=0;(g_out[tmp][tmp1]=='@')||
							   (g_out[tmp][tmp1]==':')||
							   (g_out[tmp][tmp1]=='+');tmp1++);
						hashadd(ctmp,g_out[tmp]+tmp1,NULL);
					}
					break;
				case ENICKNAMEINUSE: //Gotta choose a new nick
					if(incrunregbuffer((struct buffers_*)g_ircvar->value[2]))
						qrunfunc(&g_ircvar,2);	
					else
						message("Error, your nickname is taken . . . choose another",RED,0,0);
					break;
				case RWELCOME: //Really connected!
					qset(&gp_curcont,"connected",(void*)TRUE,QVAL);
					break;
			}
			if(!(ctmp=combmessage(t_out,2)))
			{
				break;
			}
			sprintf(toprint,"%s %s\n",toprint,ctmp);
			if((int)qnget(g_ircvar,8))
			{	erd=&g_msg[n_com];	
				if(!erd)	
				{	erd=malloc(sizeof(struct errdata));
					ZERO(erd,sizeof(struct errdata));
					erd->n=0;
				}
				if(erd->n>=SMALL)
					erd->n=0;	
				tmp	=erd->n;	
				tmp1	=strlen(ctmp);
				if(!erd->message[tmp])
					erd->message[tmp]=malloc(SMALL);
				strncpy(erd->message[tmp],ctmp,SMALL);
				erd->times[tmp]=time(NULL);
				erd->n++;
			}
		
		}
		else
		{	//We assume that privmsg is it's own packet	
			//There are other things in privmsg.  For instance a dcc send and ACTION is in privmsg
			//these are encapsulated in ctrl+A's.
			if(!strcmp(comd,"PRIVMSG"))
			{ 	
				//see if it is a ctrl+a message
				getuser(t_out[0], &ret); 	

				if(t_out[3][0] == 1)
				{	
					comd = t_out[4];	
					qtolower(&comd);

					if(!strncmp("action",comd,6))
					{
						sprintf(toprint, "%s[1;35m*** %s[1;32m %s\n",
							toprint, ret.name, combmessage(t_out,5));
					}
					else if(!strncmp("version",comd,6))
					{ 
						sprintf(temp,"NOTICE %s :VERSION Clirc %s [http://qaa.ath.cx/]\r\n",
							ret.name,VERSION);

						sprintf(toprint,"*** CTCP VERSION received from %s", ret.name);
						ircsend(temp);
					}
					else if(!strncmp("time", comd, 6))
					{ 	
						sprintf(temp, "NOTICE %s :TIME %s\r\n",
							ret.name, getlongtime());

						sprintf(toprint, "*** CTCP TIME received from %s", ret.name);
						ircsend(temp);
					}
					else if(!strncmp("ping", comd, 6))
					{ 
						sprintf(temp, "NOTICE %s :PING\r\n", ret.name);
						sprintf(toprint, "*** CTCP PING received from %s", ret.name);
						ircsend(temp);
					}
				}else
				sprintf(toprint,"%s[1;35m<%s>[1;32m %s\n",
					toprint,ret.name,getmessage(t_out,0));
				//twasn't a channel!
				if(*chan!='#')
				{
					addtobuffer(ret.name,toprint);
				}
				else
				{
					addtobuffer(chan,toprint);
				}
				toprint[strlen(toprint)-1]=0;
				saveit=FALSE;
			}else
			if(!strcmp(comd,"QUIT"))
			{ 	
				getuser(t_out[0],&ret); 	
				ctmp=getmessage(t_out,0);

				sprintf(toprint,"%s*** %s has quit [%s]",
					toprint,ret.name,ctmp);	

				strcpy(buftoput,"in");
			}else
			if(!strcmp(comd,"JOIN"))
			{ 	
				getuser(t_out[0],&ret); 	
				ctmp=getmessage(t_out,0);
				sprintf(toprint,"%s*** %s has joined %s",
					toprint,ret.name,ctmp);	

         			hashadd(ctmp,ret.name,NULL);

				strcpy(buftoput,ctmp);

				if(!strcmp(ret.name, qnget(gp_curcont,2)))
				{
					createbuffer(ctmp,0);
				}
				else if(!hashfind(ret.name))
				{
					whois(ret.name);
				}
                                        
			}else
			if(!strcmp(comd,"KICK"))
			{	getuser(t_out[0],&ret);
				sprintf(toprint, "%s*** %s has kicked you from %s [%s]",
					toprint, ret.name, chan, getmessage(t_out,0));
				if((int)qnget(g_ircvar,12))
				{
					join(chan);
				}
				strcpy(buftoput,chan);
			}else
			if(!strcmp(comd,"PART"))
			{
				getuser(t_out[0],&ret); 	
				sprintf(toprint,"%s*** %s has left %s",
					toprint,ret.name,g_out[2]);	

				hashdelete(ret.name,g_out[2]);
				strcpy(buftoput,g_out[2]);
			}else
			{
				comd=g_out[prl];
				//Assume this means that life sucks
				if(!strcmp(comd,"ERROR"))
				{
					ircdisconnect(NULL);
				}	
				sprintf(toprint,"%s%s",toprint,getmessage(t_out,0));
			}
			n++;
			sprintf(toprint,"%s%c",toprint,'\n');
			ungetuser(&ret);
		}
		if(!nxl)//End of the road
			break;
		prl=nxl;	
	}
	unparse(&g_out);
	toprint = realloc(toprint,strlen(toprint)+1);
	switch(todo)
	{	case SAVE:
			if(!saveit)
				break;
			if(qnget(g_ircvar,5))
				addtobuffer(buftoput,buf);
			else
				addtobuffer(buftoput,toprint);
			break;
		case PRINT:
			wprintf("****TOPRINT****\n");
			wprintf("[1;32m%s[0m",toprint);
			break;
		case DUMP:
			break;
	}
	if(g_log)
	{
		fprintf(g_log,"[%s] %s",getshorttime(),buf);
	}

	free(temp);
	free(toprint);
	return;
}

int getnames(char*channel)
{ 	static char query[SMALL];
	sprintf(query,"names %s\n",channel);
	ircsend(query);
	return(0);
}
#endif
