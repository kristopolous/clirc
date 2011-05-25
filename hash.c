//Seems to be slow right now - - want a datastructure to avoid the if (exist) things
//can keep a list of valid indexes but I am afraid that that might get too big
#ifndef _HASH_
#define _HASH_
#include"defines.h"

#define FACTOR	65536	
#define NOD(x)	malloc(4*x)
#define HVAL(m)	(256*m[1]+m[2])

#define HGSIZE	1000	
#define NAME_SZ	32
#define INFO_SZ	64 
#define URLT_SZ	16

struct hash
{	struct hash	**nodes;
	char 	value[33],  
			info[65];
	unsigned int 	time;
	unsigned short 	size, 	
			count, 
			hval, 	//hashvalue
			sflag;	//search flag
};

struct grouphash
{	struct hash hashes[HGSIZE];
	struct hash nodes[HGSIZE][H_SMALL];
};

unsigned short 	sths=sizeof(struct hash);

static struct 	hash	*p_names, 
			*p_chans;

char 		h_global=0, 
		h_flagval=1;

void 	p_hashadd(struct hash**, struct hash*);
void 	hashinit();
unsigned short p_hashfind(struct hash*, char*, struct hash**);

void setflag(struct hash**_t)
{	struct hash*t=(*_t);	
	static unsigned int n;
	g_sflag++;
	n=t->size;
	for(;n;n--)
		if(t->nodes[n])
			t->nodes[n]->sflag=g_sflag;
	if(t->nodes[0])
		t->nodes[0]->sflag=g_sflag;
	return;
}

unsigned int getmatch(struct hash*t)
{	static unsigned int n, count;
	n=t->size;
	count=0;
	for(;n;n--)
		if(t->nodes[n])
			if(t->nodes[n]->sflag==g_sflag)
				count++;
	if(t->nodes[0])
		if(t->nodes[0]->sflag==g_sflag)
			count++;
	return(count);
}

//This is run as a web search result
void makeaffinitylist()
{
	return;
}

unsigned int getaffinity(char*h_first, char*h_second)
{ 	static struct hash*temp;	
	p_hashfind(p_chans, h_first, &temp);
	setflag(&temp);
	p_hashfind(p_chans, h_second, &temp);
	return(getmatch(temp));
}

void hashcreate(struct hash**_t, char*value, char*info, int size)
{	(*_t)=malloc(sths);
	(*_t)->sflag=0;
	(*_t)->size=size;
	(*_t)->nodes=malloc(4*size);
	(*_t)->time=time(NULL);
	(*_t)->count=0;
	strncpy((*_t)->value, value, NAME_SZ);
	if(info)
		strncpy((*_t)->info, info, INFO_SZ);
	else
		*(*_t)->info=0;
	ZERO((*_t)->nodes, size*4);
	(*_t)->hval=HVAL(value);
	return;
}

void hashinit()
{	hashcreate(&p_names, "Names dbase", "Main username database", H_SMALL*H_SMALL);
	hashcreate(&p_chans, "Channel dbase", "Main channel database", H_SMALL*H_SMALL);
	h_global=1;
}

void rehash(struct hash**_tmp)
{	static short j;
	static struct hash*newhash;
	struct hash*tmp=(*_tmp);
	hashcreate(&newhash, tmp->value, tmp->info, tmp->size*2);
	for(j=0;j<tmp->size;j++)
	{	if(tmp->nodes[j])
			p_hashadd(&newhash, tmp->nodes[j]);
	}
	tmp->size=newhash->size;
	tmp->nodes=newhash->nodes;	//Here is where it gets pointed to.
	return;
}

void p_hashadd(struct hash**_tmp, struct hash*toadd)
{	struct hash	*tmp=(*_tmp);	

	static int 	index, 
			mask;

	if(tmp->count*2>tmp->size)
	{	rehash(_tmp);
		tmp=(*_tmp);
	}
	mask=tmp->size-1;
	index=toadd->hval*tmp->size/FACTOR;
	while(tmp->nodes[index])
		index=(index+1)&mask;
	tmp->nodes[index]=toadd;
	tmp->count++;
	return;	
}

void hashadd(char*first, char*second, char*info)
{	static struct hash	*h_first, 
				*h_second, 
				*h_tmp;
	switch(*first)
	{	case'#':	
			if(p_hashfind(p_chans, first, &h_first)!=1)
			{	hashcreate(&h_first, first, info, H_SMALL);
				p_hashadd(&p_chans, h_first);
			}
			if(p_hashfind(p_names, second, &h_second)!=1)
			{	hashcreate(&h_second, second, info, H_SMALL);
				p_hashadd(&p_names, h_second);
			}
			break;
		default:	
			if(p_hashfind(p_names, first, &h_first)!=1)  //If not already in the table
			{	hashcreate(&h_first, first, info, H_SMALL);
				p_hashadd(&p_names, h_first);		//Add it
			}
			if(p_hashfind(p_chans, second, &h_second)!=1)
			{	hashcreate(&h_second, second, info, H_SMALL);
				p_hashadd(&p_chans, h_second);
			}
			break;	
	}	
	if(p_hashfind(h_first, second, &h_tmp)!=1)
		p_hashadd(&h_first, h_second);
}

void p_addinfo(struct hash**_tmp, char*info)
{	
	struct hash*tmp=(*_tmp);
	if(tmp != NULL)
	{
		strncpy(tmp->info, info, INFO_SZ);
		tmp->time=time(NULL);
		(*_tmp)=tmp;
	}
	return;
}

void addinfo(char*first, char*info)
{	static struct hash	*h_first;
	switch(*first)
	{	case'#':
			p_hashfind(p_chans, first, &h_first);
			break;
		default:
			p_hashfind(p_names, first, &h_first);
			break;
	}
	p_addinfo(&h_first, info);
	return;
}

void singlehashadd(char*first, char*info)
{	static struct grouphash	*h_first;
	static struct hash*ptr;
	static int n=HGSIZE, gr=sizeof(struct grouphash);
	if(!first)
		return;
	if(n==HGSIZE)
	{ 	h_first=malloc(gr);	
		for(n=0;n<HGSIZE;n++)
		{	h_first->hashes[n].nodes=(struct hash**)(&h_first->nodes[n]); 
			h_first->hashes[n].sflag=0;
			h_first->hashes[n].size=H_SMALL;
		}
		n=0;
	}
	ptr=&(h_first->hashes[n]);
	strncpy(ptr->value, first, NAME_SZ);
	if(info)
		strncpy(ptr->info, info, INFO_SZ);
	else
		*ptr->info=0;
	ptr->hval=HVAL(first);
	ptr->time=time(NULL);
	switch(*first)
	{	case'#':
			if(p_hashfind(p_chans, first, &ptr)!=1)
		 		p_hashadd(&p_chans, ptr);
			break;
		default:
			if(p_hashfind(p_names, first, &ptr)!=1)
		 		p_hashadd(&p_names, ptr);
			break;	
	}	
	n++;
}

//Return values
//1 - a successful find was reached
//2 - it's free
unsigned short p_hashfind(struct hash*tmp, char*tofind, struct hash**_result)
{	unsigned static int 	index=0, mask;
	index=(HVAL(tofind)*tmp->size)/FACTOR;
	mask=tmp->size-1;
	while(tmp->nodes[index])
	{	if(!strcmp(tmp->nodes[index]->value, tofind))
		{	(*_result)=tmp->nodes[index];
			return(TRUE);
		}else
			index=(index+1)&mask;
	}
	return(2);
}

unsigned short hashfind(char*first)
{	static struct hash*temp;	
	static unsigned short res;
	switch(*first)
	{	case'#':
			res=p_hashfind(p_names, first, &temp);
			break;
		default:
			res=p_hashfind(p_chans, first, &temp);
			break;
	}
	if(res==2)
		return(FALSE);
	return(TRUE);
}

unsigned short p_hashdelete(struct hash*tmp, char*todelete)
{	static int index, mask;
	index=(HVAL(todelete)*tmp->size)/FACTOR;
	mask=tmp->size-1;
	while(tmp->nodes[index]) 	
	{	if(!strcmp(tmp->nodes[index]->value, todelete))
		{	tmp->nodes[index]=0;
			tmp->count--;
			return(TRUE);
		}
		else
			index=(index+1)&mask;
	}
	return(FALSE);
}


int hashdelete(char*first, char*second)
{ 	static struct hash	*h_first, 
				*h_second;
	switch(*first)
	{	case'#':
			if(p_hashfind(p_chans, first, &h_first)==1)
	 		{	if(p_hashfind(h_first, second, &h_second)==1)
					p_hashdelete(h_first, second);
				p_hashdelete(p_chans, first);
			}
		default:
			if(p_hashfind(p_names, first, &h_first)==1)
	 		{	if(p_hashfind(h_first, second, &h_second)==1)
					p_hashdelete(h_first, second);
				p_hashdelete(p_names, first);
			}
	}
	return(TRUE);
}

void p_walk(struct hash*tmp, int count, int max)
{	unsigned short t;
	if(tmp->size==0)
		return;
	wprintf("\033[%dC %s\n", count, tmp->value);
	count++;
	if(count>max)
		return;
	for(t=0;t<tmp->size;t++)
	{	if(tmp->nodes[t])	
		{	if(tmp->nodes[t]->sflag!=h_flagval)
			{	tmp->nodes[t]->sflag=h_flagval;
				p_walk(tmp->nodes[t], count, max);
			}
		}
	}
	return;
}

int walk(void*n)
{	
	h_flagval++;	
	p_walk(p_names, 0, 4);
	return(TRUE);
}

int chanlist(void*nothing)
{
	static int t;	

	for(t=p_chans->size-1;t;t--)
	{
		if(p_chans->nodes[t])
		{
			wprintf("%s\n", p_chans->nodes[t]->value);
		}
	}
	return(TRUE);
}

//The slowest function
int urlprint(char*in, char**_out)
{
	char*out=(*_out);
	static int t, n, low, high, sz;
	static struct hash*res, *touse;
	if(*in=='0')
	{	n=atoi(in+1);
		in+=2;
	}else if(*in=='?')//invalid first in IRC world - reserved for search
	{	char*rec, *ptr;
		in++;
		if(*in)
		{	while(*in++!='='&&*in);
			ptr=rec=malloc(SMALL);
			while(*in)
			{	switch(*in)
				{	case '%':
						*rec=((in[1]-'0')<<4)+in[2]-'0';
						in+=2;
						break;
					case '+':
						*rec=' ';
						break;
					default:
						*rec=*in;
						break;
				}
				in++;
				rec++;
			}
			rec[0]=0;
			wprintf("%s\n", ptr);
			in=ptr;
		}
		sprintf(out, "<style>body{margin:0;padding:0;background:#D4D4FF;}.s{float:left;display:compact;background:#99F;color:#002;width:100;height:50;margin-left:12.5;margin-top:12.5;border-width:2;border-style:solid;}.h{background:#AAF;color:#004;margin-left:0;margin-top:0;border-bottom-width:1;border-bottom-style:solid;border-top-style:solid;border-top-width:1;}</style><body><div class=s><center><br><a href=\"/\"><font color=black>dbase</font></a></div><div class=h><pre><font size=+4><center>Search</font></div><center><br><table bgcolor=#000080 border=0 cellpadding=1><tr><td><table border=0 bgcolor=#A0A0F0 cellpadding=5 cellspacing=3><tr><td><form name=query action=/?><input maxLength=64 size=32 name=q value=\"%s\"> <input type=submit value=\"Get Affinity\"></form></td></tr><tr><td>", in);
		if(*in)//oh goodie,  we are RETURNING results
		{	if(*in=='#')	//channel
			{	p_hashfind(p_chans, in, &res);
				if(!res)	//don't question this,  I got it from below
				{	sprintf(out, "%s%s</div><br><center><table bgcolor=#000080 border=0 cellpadding=1><tr><td><table border=0 bgcolor=#A0A0F0 cellpadding=5 cellspacing=3>Unable to find %s.  Either waiting to join or it is invite only/key required.</td></tr></table></td></tr></table>", out, in, in);
					return(TRUE);
				}else
				if(*res->info==0)
				    join(res->value);
			}else		//username
			{ 	*out=0;	//zero out the search
				urlprint(in, &out);	//1 level recursion
				return(TRUE);
			}
			sprintf(out, "%sUnable to find results for \"%s\".", out, in);
		}
		sprintf(out, "%s</td></tr></table></td></tr></table>", out);
		return(TRUE);
	}
	else
		n=0;
sprintf(out, "<style>body{margin:0;padding:0;background:#D4D4FF;}.s{float:left;display:compact;background:#99F;color:#002;width:100;height:50;margin-left:12.5;margin-top:12.5;border-width:2;border-style:solid;}.h{background:#AAF;color:#004;margin-left:0;margin-top:0;border-bottom-width:1;border-bottom-style:solid;border-top-style:solid;border-top-width:1;}</style><body><div class=s><center><br><a href=\"?\"><font color=black>Search</font></a></div><div class=h><pre><font size=+4><center>");
	if(*in=='.')	//taken as #
	{	*in='#';
		p_hashfind(p_chans, in, &res);	//don't question this,  I got it from above :)
		if(!res)
		{	sprintf(out, "%s%s</div><br><center><table bgcolor=#000080 border=0 cellpadding=1><tr><td><table border=0 bgcolor=#A0A0F0 cellpadding=5 cellspacing=3>Unable to find %s.  Either waiting to join or it is invite only/key required.</td></tr></table></td></tr></table>", out, in, in);
			return(TRUE);
		}else
		if(*res->info==0)
			join(res->value);
	}else if(!strlen(in))
		res=p_chans;
	else
	{	p_hashfind(p_names, in, &res);
		if(!res)
		{	sprintf(out, "%s%s</div><br><center><table bgcolor=#000080 border=0 cellpadding=1><tr><td><table border=0 bgcolor=#A0A0F0 cellpadding=5 cellspacing=3>Unable to find user %s.  A whois may be pending.</td></tr></table></td></tr></table>", out, in, in);
			return(TRUE);
		}else
		if(!*res->info)
			whois(res->value);
	}
	sprintf(out, "%s%s</font><font size=+2> %s</div><br><center><table bgcolor=#000080 border=0 cellpadding=1><tr><td><table border=0 bgcolor=#A0A0F0 cellpadding=5 cellspacing=3>", out, res->value, res->info);
	if(n)
	{	if(*in=='#')	
			*in='.';
		sprintf(out, "%s<tr><td><a href=0%d%s>prev</a></td>", out, n-1, in);
		low=n*URLT_SZ;
		high=low+URLT_SZ;
		if(high>res->count)
		{	high=res->count;
			sprintf(out, "%s<td></td><td></td></tr>", out);
		}else
			sprintf(out, "%s<td></td><td></td><td><a href=0%d%s>next</a></td></tr>", out, n+1, in);
		if(*in=='.')
			*in='#';
	}
	else if(res->count>URLT_SZ)
	{	
		if(*in=='#')	
		{
			*in='.';
		}
		sprintf(out, "%s<tr><td></td><td></td><td></td><td><a href=0%d%s>next</a></td></tr>", out, n+1, in);
		if(*in=='.')
		{
			*in='#';
		}
		low=0;
		high=URLT_SZ;
	}
	else
	{
		low=0;
		high=res->count;
	}
	sz=res->size;

	if(*in=='#'||res==p_names)
	{
		for(t=0;t<sz;t++)
		{	
			if(res->nodes[t])
			{
				if(low==0)	
				{
					if(high==0)
					{
						break;	
					}
					touse=res->nodes[t];
					sprintf(out, "%s<tr><td bgcolor=#D4D4FF><a href=%s>%s</a></td><td bgcolor=#D4D4FF>%s</td><td bgcolor=#D4D4FF>%d</td><td bgcolor=#D4D4FF>%s</td></tr>", out, touse->value, touse->value, touse->info, touse->count, numtotime(touse->time));
				}
				else
				{
					low--;
				}
				high--;
			}
		}
		strcat(out, "</td></tr></table></td></tr></table>");
	}
	else
	{
		for(t=0;t<sz;t++)
		{
			if(res->nodes[t])
			{
				if(low==0)	
				{
					if(high==0)
					{
						break;
					}
					touse=res->nodes[t];
					sprintf(out, "%s<tr><td bgcolor=#D4D4FF><a href=\"%c%s\">%s</a></d><td bgcolor=#D4D4FF>%s</td><td bgcolor=#D4D4FF>%d</td><td bgcolor=#D4D4FF>%s</td></tr>", out, '.', touse->value+1, touse->value, touse->info, touse->count, numtotime(touse->time));
				}
				else
				{
					low--;
				}
				high--;	
			}
		}
		strcat(out, "</td></tr></table></td></tr></table>");
	}
	if(*res->info==0)
	{
		p_addinfo(&res, "[No Topic]");
	}
	return(TRUE);
}
	
int nicklist(void*nothing)
{	
	static int t;	
	for(t=p_names->size-1;t;t--)
	{	if(p_names->nodes[t])
		{	wprintf("%s\n", p_names->nodes[t]->value);
		}
	}
	return(TRUE);
}

void p_hbuild(struct hash*tmp)
{
	unsigned short t;
	if(tmp->size==0)
		return;
	if(*(tmp->value)=='#')
	{	hchans[c_hchans]=tmp->value;
		c_hchans++;
	}else
	{	hnames[c_hnames]=tmp->value;
		c_hnames++;
	}
	for(t=0;t<tmp->size;t++)
	{	if(tmp->nodes[t]&&!tmp->nodes[t]->sflag)	
		{	tmp->nodes[t]->sflag=1;
			p_hbuild(tmp->nodes[t]);
		}
	}
	return;
}

void hbuild()
{	c_hchans=0;
	c_hnames=0;
	p_hbuild(p_chans);
	return;
}
#endif
