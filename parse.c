//Add parsechars
#ifndef _PARSE_
#define _PARSE_
#include"defines.h"

//Parse char
//a 256 entry array.  value of 
//	0 = pass through		P_ADD
//	1 = replace with char symbol	P_REPL
//	2 = replace but only once	P_REPLONCE	
//	P_REPLONCE is useful for whitespace when you want "          " to register only as,  say " "

int newparse(char**_in)
{
	static char*in;	
	(*_in)=malloc(256);
	in=(*_in);
	memset(in, P_ADD, 256);
	memset(in, P_REPL, 32);
	return(TRUE);
}

int tableparse(char*table, char**_in, char***_out)
{
	static char	*in, 
			**out, 
			*end, 
			c;
	in=(*_in);
	out=(*_out);
	end=in+strlen(in);

	if(end==in)
	{
		*out=0;
		return(TRUE);
	}	
	while(table[(int)*in]==P_REPLONCE)//if the first character is a space
	{
		++in;//dump it.
	}
	*out=in;

	while(in!=end)
	{
		switch(table[(int)*in])
		{
			case P_ADD:
				break;
			case P_REPLONCE:
				out++;
				*out=g_chrs[(int)*in];

				if(*(in+1)==*in)
				{
					c=*in;
					while(*in==c)
					{	*in=0;
						in++;
					}
				}
				if(table[(int)(*(in+1))]!=P_REPL)
				{
					out++;
					*out=(in+1);
				}

				*in=0;
				break;

			case P_REPL:
				out++;
				*out=g_chrs[(int)*in];

				if(table[(int)*(in+1)]!=P_REPL)
				{	
					out++;
					*out=(in+1);
				}
				*in=0;
				break;
		}
		in++;
	}
	out[1]=0;
	out[2]=0;
	out[3]=0;
	return(TRUE);
}	


void unparse(char***_in)
{	char**in=(*_in);
	static int n;
	n=1;
	loop1:
		if(!in[n])
			return;
		else if(*in[n]<32)
			in[n-1][strlen(in[n-1])]=*in[n];
		else if(in[n][-1]);
		else
			in[n][-1]=' ';
		n++;
		goto loop1;
}

void parse(char**_in, char***_out)
{	static char	*in, 
				**out, 
				*end;
	in=(*_in);
	out=(*_out);
	end=in+strlen(in);

	if(end==in)
	{	*out=0;
		return;
	}	
	while(*in==32)//if the first character is a space
		++in;//dump it.
	*out=in;

	while(in!=end)
	{	if(*in>32)
		{	in++;
			continue;	
		}
		if(*in!=' ')
		{	if(*out)	
				if(*out[0]>32)
					out++;
			*out=g_chrs[(int)*in];
		}
		*in=0;
		in++;
		if(*in>32)
		{	out++;
			*out=in;
		}	
	}
	out[1]=0;
	out[2]=0;
	out[3]=0;
/*	i=0;
	while(out[i])
		printf("<%s>", out[i++]);
	fflush(stdout);
*/	return;
}	

int endofline(char**out, int start)
{	
	loop:
		if(!out[start])
		{
			return(start);
		}
		if(out[start]==g_chrs[10])
		{
			return(start);
		}
		start++;
		goto loop;
}

int nextline(char**out, int start)
{ 	
	loop:
		if(!out[start])
		{
			return(0);
		}
		if(out[start]==g_chrs[10])
		{
			if(out[start+1])
			{
				return(start+1);
			}
		}
		start++;
		goto loop;
}

int combine(char**out, int start, char**_res)
{	
	int end=endofline(out, start);
	char*res=(*_res);
	if(start==end)
	{
		return(FALSE);
	}
	res=malloc(256);	
	ZERO(res, 256);
	sprintf(res, "%s", out[start]);
	start++;
	while(start!=end)
	{
		sprintf(res, "%s %s", res, out[start]);
		start++;
	}
	(*_res)=res;
	return(TRUE);
}
#endif
