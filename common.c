#ifndef _COMMON_
#define _COMMON_
#include "defines.h"

int curcolor, 
  precolor, 
  prespace, 
  brightness, 
  prebright;

void addvarcom(struct ccom**_ccom, struct qval**_qval, char addcoms)
{	
	unsigned short index = g_varcom.count;
	static struct ccom*ctemp = 0;
	static struct qval*qtemp = 0;

	g_varcom.ccoms[index] = 0;

	if(_qval)
	{
		g_varcom.qvals[index] = malloc(sizeof(struct qval));
	}
	else
	{
		g_varcom.qvals[index] = 0;
	}
	qtemp = g_varcom.qvals[index];
	clinit(&g_varcom.ccoms[index]);
	ctemp = g_varcom.ccoms[index];
	(*_ccom) = ctemp;
	if(_qval)
	{
		(*_qval) = qtemp;
	}
	if(addcoms)
    {
		addcom(&ctemp, "set", set, NULL, "Set a variable");
    	addcom(&ctemp, "show", show, NULL, "Show the value of a variable");
	}
	g_varcom.count++;
	return;
}

void mainloop()
{
	static int i;	

	if(cline(&g_curprompt->coms, g_curprompt->color, g_curprompt->prompt) == DONE)
	{    
		if(!popprompt())
		{
		 	ircdisconnect(NULL);
			dumpvals();
			do_exit(0);
		}
	}

	wupdate();

	for(i = 0;i < g_con.size;i++)
	{	
		curc = g_con.cons[i];
//		g_curs = curc->serv;

//This tests if it is connected
		if(*(int*)gp_curcont->value[3])
		{
			getstuff(RAW);
		}
// If buffered things need to be sent, this will send them
		ircsend(NULL);
	}
	poll_servers();
	return;
}

void wupdate()
{	
	if(strlen(towrite))	
	{
		if(!g_curs->port)
		{
			printf("%s", towrite);
			fflush(stdout);
		}
		else	
		{
	 		protected_write(g_curs, towrite);
		}
		ZERO(towrite, MEDIUM);
	}
}
 
// Allocates memory
char*itoa(int in)
{	
	static char
		*ret = 0, 
		*pr, 
		*pre;

	ret = malloc(TINY);
	pre = ret + TINY - 1;
	*(pre+1) = 0;
	
	pr = pre;
	do
	{ 	
		pr --;	
		*pr = in % 10 + '0';
		in /= 10;
	}while(in);

	return(pr);
}

char*wfgets(char*buf, int size)
{	
	struct server*curs = g_curs;	
	static int n;

	wupdate();
	g_curs->isactive = FALSE;	

	if(g_curs->port != 0)
	{ 
		for(;;)
		{	
			if(poll(g_ufds, g_curs->curfd[g_curs->curcon] + 1, 10)&&g_ufds[g_curs->curfd[g_curs->curcon]].revents == POLLIN)
			{
				g_curs->isactive = TRUE;
				ZERO(buf, size);
	  			read(g_curs->cur[g_curs->curcon], buf, size);
				n = strlen(buf);
				buf[n] = 0;
				buf[n - 1] = 0;
				printf("<%s>", buf);
				fflush(stdout);
				break;
			}
			else
			{
				poll_servers();	
			}
			g_curs = curs;
		}
	}
	else
	{	
		for(;;)
		{
			if(poll(g_ufds, g_curs->prifd+1, 10)&&g_ufds[g_curs->prifd].revents == POLLIN)
			{
				g_curs->isactive = TRUE;
				fgets(buf, size, stdin);
	  		//	read(g_curs->pri, buf, size);
				break;
			}
			else
			{
				poll_servers();	
			}
			g_curs = curs;
		}
	}
	return(buf);
}

int wprintf(char*fmt, ...)
{	
	va_list ap;
	static int 	d = 0;
	static char 	c = 0, 
			*s = 0, 
			*pt = 0;
//current buffer point
	pt = towrite+strlen(towrite);

	va_start(ap, fmt);
	while (*fmt)
	{	
		if(*fmt == '%')	
		{
			fmt++;	
			switch(*fmt) 
			{
				case 's':	/* string */
			 		s = va_arg(ap, char *);
					if(s)
					{
						while(*s)
						{
							*pt = *s;
							pt++;
							s++;
						}
					}
					else
						strcat(pt, " < NULL > ");
					break;
				case 'd':	/* int */
					d = va_arg(ap, int);
					s = itoa(d);
					while(*s)
					{
						*pt = *s;
						pt++;
						s++;
					}
					break;
				case 'c':	/* char */
					/* need a cast here since va_arg only
					takes fully promoted types */
					c = (char) va_arg(ap, int);
					*pt++= c;
					break;
				default:
					return(FALSE);
					break;
			}
			fmt++;
		}
		else
		{
			*pt = *fmt;
			pt++;
			fmt++;
		}
	}

	va_end(ap);
	if((pt - towrite) > 750)
	{
		wupdate();
	}
	return(TRUE);
}

char*rprintf(char*fmt, ...)
{	
	va_list 	ap;
	static int 	d = 0;
	static char 	
		c = 0, 
		*s = 0, 
		*pt = 0;
	char *ret = 0;
//current buffer point
	ret = malloc(MEDIUM);
	pt = ret;

	va_start(ap, fmt);
	while (*fmt)
	{	
		if(*fmt == '%')	
		{
			fmt++;	
			switch(*fmt) 
			{
				case 's':	/* string */
			 		s = va_arg(ap, char *);
					if(s)
					{	
						while(*s)
						{
							*pt = *s;
							pt++;
							s++;
						}
					}
					else
						strcat(pt, " < NULL > ");
					break;
				case 'd':	/* int */
					d = va_arg(ap, int);
					s = itoa(d);
					while(*s)
					{
						*pt = *s;
						pt++;
						s++;
					}
					break;
				case 'c':	/* char */
					/* need a cast here since va_arg only
					takes fully promoted types */
					c = (char) va_arg(ap, int);
					*pt++= c;
					break;
				default:
					return(FALSE);
					break;
			}
			fmt++;
		}
		else
		{
			*pt = *fmt;
			pt++;
			fmt++;
		}
	}
	va_end(ap);
	return(ret);
}

void do_exit(int n)
{	
	printf("[0m");
	exit(n);
}

void protected_write(struct server*towrite, char*toput)
{	
	if(	poll(g_ufds, towrite->curfd[towrite->curcon]+1, 10)
		&&g_ufds[towrite->curfd[towrite->curcon]].revents)
	{
		if(g_ufds[towrite->curfd[towrite->curcon]].revents&badstuff)
		{
	  		return;
		}
	}
	write(towrite->cur[towrite->curcon], toput, strlen(toput));

	return;
}

void pushprompt(struct qval*vars, struct ccom*coms, char*prompt, int color)
{ 	
	g_curprompt->next = malloc(sizeof(struct curprompt_));
	(g_curprompt->next)->prev = g_curprompt;
	g_curprompt = g_curprompt->next;
	g_curprompt->prompt = prompt;
	g_curprompt->vars = vars;
	g_curprompt->coms = coms;
	g_curprompt->color = color;
	return;
}

int popprompt()
{	
	g_curprompt = g_curprompt->prev;

	if(g_curprompt == NULL)
	{
		return(FALSE);
	}
	free(g_curprompt->next);
	return(TRUE);
}

void setcolor(int col)
{    
	precolor = curcolor;
    curcolor = col;
}

void revertcolor() 	
{
	curcolor = precolor;
}
void goback() 		
{
	brightness = prebright;
}

void godark()	
{
	prebright = brightness;
	brightness = 0;
}
void golight()	
{
	prebright = brightness;
	brightness = 1;
}

void printstring(char*str, int n)
{
	if(prespace&&g_curs->hascolor)
	{
		wprintf("[ % dC", prespace);	
	}
	if(n) 	
	{
		if(g_curs->hascolor)
		{
			wprintf("[ % d;31m[ % d]:", brightness, n);
		}	
		else
		{
			wprintf("[ % d]:", n);
		}
	}
	if(g_curs->hascolor)
	{
		wprintf("[ % d;3 % dm % s", brightness, curcolor, str);
	}
	else
	{
		wprintf(" % s", str);
	}
	if(str[strlen(str) - 1] > 31)
	{
		wprintf("\n");
	}
	return;
}

void dospace(int n) 
{
	prespace = n;
}

char*getspaces(int count)
{
	static char*space = 0;

	if(!space)
	{
		space = malloc(SMALL);
	}
	space[count] = 0;

	while(count--)
	{
		space[count] = ' ';
	}

	return(space);
}	

void printvalue(char*name, void*value, int type, int spaces)
{	
	static int n;	
	static struct buffers_*tmp;
	if(g_curs->hascolor)
	{
		wprintf("[%dC[%d;3%dm", prespace, brightness, curcolor);
	}
	if(name == NULL)
	{	
		name = CHR(20);
		strcpy(name, " < < UNKNOWN > > ");
	}	
	if(*name > 'Z')
	{
		wprintf("%c%s", *name - 32, (name + 1));
	}
	else
	{
		wprintf("%s", name);
	}
	
	if(g_curs->hascolor)
	{	
		wprintf("[%dC[%d;33m", spaces - strlen(name), brightness);
	}
	else
	{
		wprintf("%s", getspaces(spaces - strlen(name)));
	}

	switch(type)
	{
		case QBOL:
			switch((int)value)
			{	
				case TRUE: 
					wprintf("True");
					break;

				case FALSE:
					wprintf("False");
					break;

				case WAITING:
					wprintf("Waiting . . .");
					break;

				default:
					wprintf("Unknown");
					break;
			}
			wprintf("\n");
			break;

		case QINT:
			wprintf("%d\n", (int)value);
			break;

		case QSTR:	
			tmp = value; 
			if(tmp->data[0])
			{
				wprintf("%s", tmp->data[0]);
			}
			else
			{
				wprintf("<No Value>");

				if(!tmp->curpoint&&tmp->size > 1)
				{
					if(g_curs->hascolor)
					{
						wprintf("[%dC[1;3%dm<<[1;3%dm", 2, MAGENTA, YELLOW);
					}
					else
					{
						wprintf("*");
					}
				}
				wprintf("\n");

				if(g_curs->hascolor)
				{	
					for(n = 1;n < tmp->size;n++)
					{
						if(n == tmp->curpoint)
						{
							wprintf("[%dC[1;3%dm<<[1;3%dm\n", spaces - 2, MAGENTA, YELLOW);
						}
						else
						{
							wprintf("[%dC", spaces);
						}
						wprintf("%s\n", tmp->data[n]);
					} 
				}
				else
				{
					for(n = 1;n < tmp->size;n++)
					{
						wprintf("%s%s", getspaces(spaces), tmp->data[n]);	
						if(n == tmp->curpoint)
						{
							wprintf("*\n");
						}
						else
						{
							wprintf("\n");
						}
					} 
				}
			}
			break;

		case QLIN:	
			wprintf("%s\n", (char*)value);
			break;

		case QNL:
			wprintf("\n");	
			break;

		default:
			fflush(stdout);
			return;
			break;
	}
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
	pos = pos % 4;
	wprintf("[1D%c", loop[(int)pos]);
	fflush(stdout);
	return;
}

char*numtotime(time_t m_time)
{	
	static struct tm *gmt;
	static char*ret = 0;
	if(!ret)
	{
		ret = CHR(SMALL);
	}
	gmt = localtime(&m_time);
	sprintf(ret, "%d%s%02d:%02d:%02d"
		, gmt->tm_mday
		, nl_langinfo(gmt->tm_mon+ABMON_1)
		, gmt->tm_hour
		, gmt->tm_min, gmt->tm_sec);
	return(ret);
}

char*getshorttime()
{	
	static time_t m_time;
	static struct tm *gmt;
	static char*ret = 0;
	if(!ret)
	{
		ret = CHR(SMALL);
	}
	time(&m_time);
	gmt = localtime(&m_time);
	sprintf(ret, " %d%s%02d:%02d:%02d"
		, gmt->tm_mday
		, nl_langinfo(gmt->tm_mon+ABMON_1)
		, gmt->tm_hour
		, gmt->tm_min, gmt->tm_sec);
	return(ret);
}

char*getlongtime()
{
	static time_t m_time;
	static struct tm *gmt;
	static char*ret = 0;
	if(!ret)
	{
		ret = CHR(SMALL);
	}
	time(&m_time);
	gmt = localtime(&m_time);
	sprintf(ret, "%s, %02d%s%d%02d:%02d:%02d GMT"
		, nl_langinfo(gmt->tm_wday+ABDAY_1)
		, gmt->tm_mday
		, nl_langinfo(gmt->tm_mon+ABMON_1)
		, gmt->tm_year+1900, gmt->tm_hour
		, gmt->tm_min, gmt->tm_sec);
	return(ret);
}

//This is backwardsssss
struct tm *chrtotm(char *toconvert)
{	
	struct tm *ret = (struct tm *)malloc(sizeof(struct tm));
	char mon[5] = {0}, day[5] = {0};
	char str[7][16] = {{0}};
	int n;
	for (n = 0;n < 7;n++) ZERO(&str[n], 16);
	memcpy(&str[0][0], toconvert, 3);
	memcpy(&str[1][0], toconvert+5, 2);
	memcpy(&str[2][0], toconvert+8, 3);
	memcpy(&str[3][0], toconvert+12, 4);
	memcpy(&str[4][0], toconvert+17, 2);
	memcpy(&str[5][0], toconvert+20, 2);
	memcpy(&str[6][0], toconvert+23, 2);
	strcpy(day, str[0]);
	ret->tm_mday = atoi(str[1]);
	strcpy(mon, str[2]);
	ret->tm_year = atoi(str[3]);
	ret->tm_hour = atoi(str[4]);
	ret->tm_min = atoi(str[5]);
	ret->tm_sec = atoi(str[6]);
	ret->tm_isdst = 0;
	day[4] = 0;
	mon[4] = 0;
	for(n = 0;n < 7;n++)
	{
		if(!strcmp(day, nl_langinfo(ABDAY_1+n)))
		{
			ret->tm_wday = n;
		}
	}
	for(n = 0;n < 12;n++)
	{
		if(!strcmp(mon, nl_langinfo(ABMON_1+n)))
		{
			ret->tm_mon = n;
		}
	}

	ret->tm_year -= 1900;

	return(ret);
}

struct helpdata
{	
	char*title;
	char*data;
};

char*gethelp(char*topic)
{	
	static int 
		helpdb = 0, 
		ret = 0, 
		n = 0, 
		topics = 0, 
		size = 0;

	static char
		*buffer = 0,
		*pbuf = 0,
		flag = 0;

	static struct helpdata hdata[MEDIUM];

	if(!helpdb)
	{	
		helpdb = open("help.db", O_RDONLY);

		if(helpdb == - 1)
		{
			return("Unable to retrieve help");
		}
		buffer = malloc(MEDIUM);
		pbuf = buffer;

		for (;;)
		{
			ret = read(helpdb, pbuf, MEDIUM);

			if (MEDIUM == ret)
			{
				n += MEDIUM;
				buffer = realloc(buffer, n);
				pbuf = n + buffer - MEDIUM;
			} 
			else
			{
				break;
			}
		}
		pbuf += ret;
		size = pbuf - buffer;
		for(n = 0;n < size;n++)
		{	
			switch(buffer[n])
			{
				case '\n':
					if(flag == 2)
					{	
						buffer[n] = 0;
						hdata[topics].data = buffer+n+1;
					}
					flag = 1;
					break;

				case '.':
					if(flag)
					{
						buffer[n] = 0;
						flag = 2;
						topics++;
						hdata[topics].title = buffer+n+1;
					}
					break;

				default:
					if(flag == 1)
					{
						flag = 0;
					}
					break;
			}
		}
		topics++;
	}
	if(topic == NULL)
	{
		for(n = 0;n < topics;n++)
		{
			if(hdata[n].title)
			{
				message(hdata[n].title, CYAN, 0, 0);
			}
		}
		return(NULL);
	}
	for(n = 0;n < topics;n++)
	{
		if(hdata[n].title)
		{
			if(!strncmp(topic, hdata[n].title, strlen(topic)))
			{
				return(hdata[n].data);
			}
		}
	}

	return("Sorry, no help available on that topic.");
}	

int getpasswd(short call)
{	
	wprintf("Password:");
	return(TRUE);
}

char*quenc(char*toc)
{	
	static char 
		*p, 
		*e, 
		*s1, 
		*e1, 
		toenc[12];
	const int size = 12;
	static int sz;
	sz = strlen(toc);
	strcpy(toenc, toc);
//End of string
	e = toenc+size;
	e1 = e - 1;
//Beginning of string
	s1 = toenc - 1;
	for(;sz < size;sz++)
	{
		toenc[sz] = 32;	
	}
//Cut the string
	toenc[size - 1] = 0;
	for(p = toenc;p != e;p++)
	{
		switch(size)
		{
			case 12:*p += toenc[toenc[11] % size];	
			case 11:*p += toenc[toenc[10] % size];	
			case 10:*p += toenc[toenc[9] % size];	
			case 9:	*p += toenc[toenc[8] % size];	
			case 8:	*p += toenc[toenc[7] % size];	
			case 7:	*p += toenc[toenc[6] % size];	
			case 6:	*p += toenc[toenc[5] % size];	
			case 5:	*p += toenc[toenc[4] % size];	
			case 4:	*p += toenc[toenc[3] % size];	
			case 3:	*p += toenc[toenc[2] % size];	
			case 2:	*p += toenc[toenc[1] % size];	
			case 1: *p += toenc[toenc[0] % size];
		}
	}
	for(p = e1;p != s1;p--)
	{
		switch(size)
		{
			case 12:*p += toenc[toenc[11] % size];	
			case 11:*p += toenc[toenc[10] % size];	
			case 10:*p += toenc[toenc[9] % size];	
			case 9:	*p += toenc[toenc[8] % size];	
			case 8:	*p += toenc[toenc[7] % size];	
			case 7:	*p += toenc[toenc[6] % size];	
			case 6:	*p += toenc[toenc[5] % size];	
			case 5:	*p += toenc[toenc[4] % size];	
			case 4:	*p += toenc[toenc[3] % size];	
			case 3:	*p += toenc[toenc[2] % size];	
			case 2:	*p += toenc[toenc[1] % size];	
			case 1: *p += toenc[toenc[0] % size];
		}
		*p = *p % 32;
		*p += 'A';
	}
	return(toenc);
}

void dettach()
{
	return;
}

void reattach(char*toattach)
{
	return;
}

char*selectrandom(struct random*toselect)
{	
	static int 
		rnd = 0, 
		i = 0;
	rnd = rand() % 100000;
	i = 0;

	while((i < toselect->indices)&&(rnd > toselect->bps[i]));

	return(toselect->values[i]);
}
#endif
