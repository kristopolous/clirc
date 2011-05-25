//TODO: Major revisions.
#ifndef _QVAL_
#define _QVAL_
#include "defines.h"

//Add a qval set to the central database
void qaddtobig(struct qval*toadd,char*name)
{	
	static int sz;

	sz=qmain.size;
	qmain.names[sz]=malloc(strlen(name)+1);
	strcpy(qmain.names[sz],name);
	qmain.qvals[sz]=toadd;
	qmain.size++;
	return;
}

struct qval*findqval(char*tosearch)
{	
	static int sz;
	for(sz=0;sz<qmain.size;sz++)
	{
		if(!strcmp(qmain.names[sz],tosearch))
		{	
			return(qmain.qvals[sz]);
		}
	}
	return(NULL);
}

//Remove a qval set from the central database
void qdelfrombig(struct qval*toadd,char*name)
{	
	static int sz;
	sz=qmain.size;	

	while(sz)
	{
		if(!strncmp(qmain.names[sz],name,strlen(name)))
		{
			qmain.names[sz]=0;
			qmain.qvals[sz]=0;
			return;
		}
		sz--;
	}
	return;
}

//Converts a string to lowercase letters
void qtolower(char**_str)
{	
	static int n;
	char*str=(*_str);
	for(n=strlen(str);n>-1;n--)
	{
		if(str[n]>='A')
		{
			if(str[n]<='Z')
			{
				str[n]|=0x20;
			}
		}
	}
}

//Associates a function with a qval
int qsetfunc(struct qval**_in,int index,int(*function)(void*))
{	
	struct qval*in=(*_in);
	in->func[index].function=function;
	return(TRUE);
}

//Runs the associated function
int qrunfunc(struct qval**_in,int index)
{
	struct qval*in=(*_in);
	if(in->func[index].function)
	{
		(in->func[index].function)(qnget(in,index));
	}
	return(TRUE);
}

int qassign(struct qval**_in,char*str,char type,void*value,int index)
{	
	struct qval*in=(*_in);

	in->name[index]=malloc(strlen(str)+1);
	memcpy(in->name[index],str,strlen(str)+1);
	qtolower(&in->name[index]);
	in->flags[index]=type&QFLAG;
	type		&=0x0F;
	in->type[index]	=type;
	in->toget	=in->toget|BIN(index);

	switch(type)
	{
		case QINT:
		case QBOL:
			in->value[index]=malloc(4);
			break;
		case QSTR:
			in->value[index]=createunregbuffer(NULL);
			break;
	}
	if(value)
	{
		switch(type)
		{	case QINT:
			case QBOL:
				memcpy(&in->value[index],value,4);
				break;
			case QSTR:
				addtounregbuffer(in->value[index],value);
				break;
		}
	}
	qrunfunc(_in,index);
	return(1);
}	
int qgetnum(char*ins)
{
	static int temp;

	temp=atoi(ins);
	if((!temp)&&*ins!='0') //tis a string
	{
		if(!strncmp("true",ins,strlen(ins)))
		{
			temp=1;
		}
		else if(!strncmp("false",ins,strlen(ins)))
		{
			temp=0;
		}
		else if(!strncmp("no",ins,strlen(ins)))
		{
			temp=0;
		}
		else if(!strncmp("yes",ins,strlen(ins)))
		{
			temp=1;
		}
	}
	return(temp);
}

int qnset(struct qval**_in,int index,void*value,int type)
{	
	struct qval*in=(*_in);
	static int toput;

	switch(in->type[index])
	{	case QBOL:	
		case QINT:
			if(type==QSTR)
				toput=qgetnum((char*)value);
			else
				toput=(int)value;
			memcpy(&in->value[index],&toput,4);
			break;
		case QSTR:
		//	in->value[index]=createunregbuffer(NULL);
			addtounregbuffer(in->value[index],value);
			break;
	}
	qrunfunc(_in,index);
	if(in->watch&BIN(index))
	{
		printvalue(in->name[index],in->value[index],in->type[index],QMAR);
	}
	return(TRUE);
}	

int QINTqnset(struct qval**_in,int n,int value)
{	
	struct qval*in=(*_in);
	static int toput;
		toput=(int)value;

	memcpy(&in->value[n],&toput,4);
	qrunfunc(_in,n);
	if(in->watch&BIN(n))
	{
		printvalue(in->name[n],in->value[n],in->type[n],QMAR);
	}
	return(TRUE);
}	

int qset(struct qval**_in,char*str,void*value,int type)
{	
	struct 	qval*in=(*_in);
	static int 	index=0,
			toput;
	qtolower(&str);
	for(index=0;index<32;index++)
	{
		if(in->toget&BIN(index))
		{
			if(!strncmp(str,in->name[index],strlen(str)))
			{
				break;
			}
		}
	}
	if(index==32)
	{
		return(FALSE);
	}	
	switch(in->type[index])
	{	case QBOL:	
		case QINT:
			if(type==QVAL)	
			{	in->value[index]=malloc(sizeof(int*));
				*(int*)in->value[index]=(int)value;
				break;
			}
			else if(type==QSTR)
				toput=qgetnum((char*)value);
			else if (type==QINT)
				toput=(int)value;
			memcpy(&in->value[index],&toput,4);
			break;
		case QSTR:
			replacecurunregbuffer(in->value[index],value);
			break;
	}
	qrunfunc(_in,index);
	if(in->watch&BIN(index))
		printvalue(in->name[index],in->value[index],in->type[index],QMAR);
	return(TRUE);
}	

int qmove(struct qval**_in,char*str,int value,short type)
{	struct 	qval*in=(*_in);
	static int 	index=0;

	qtolower(&str);
	for(index=0;index<32;index++)
		if(in->toget&BIN(index))
			if(!strncmp(str,in->name[index],strlen(str)))
				break;
	if(index==32)
		return(FALSE);
	switch(type)
	{	case ABSOLUTE:
			((struct buffers_*)(in->value[index]))->curpoint=value;
			break;
		case RELATIVE:
			((struct buffers_*)(in->value[index]))->curpoint+=value;
			break;
	}
	qrunfunc(_in,index);
	if(in->watch&BIN(index))
		printvalue(in->name[index],in->value[index],in->type[index],QMAR);
	return(TRUE);
}	

int qadd(struct qval**_in,char*str,void*value,int type)
{	struct 	qval*in=(*_in);
	static int 	index=0,
			toput;
	qtolower(&str);
wprintf("%s %s\n",str,value);
	for(index=0;index<32;index++)
		if(in->toget&BIN(index))
			if(!strncmp(str,in->name[index],strlen(str)))
			{	wprintf("%s\n",in->name[index]);	
				break;
			}

	if(index==32)
		return(FALSE);
	switch(in->type[index])
	{	case QBOL:	
		case QINT:
			if(type==QSTR)
				toput=qgetnum((char*)value);
			else
				toput=(int)value;
			memcpy(&in->value[index],&toput,4);
			break;
		case QSTR:
			addtounregbuffer(in->value[index],value);
			break;
	}
	qrunfunc(_in,index);
	if(in->watch&BIN(index))
		printvalue(in->name[index],in->value[index],in->type[index],QMAR);
	return(TRUE);
}	

//Not used
int qexist(struct qval**_in,char*name)
{	int n=0;
	struct qval*in=*_in;

	for(;n<32;n++)
	{	if(!strcmp(name,in->name[n]))
			return(TRUE);
	}
	return(FALSE);
}

//Not used
int qdump(struct qval**_in,int index)
{	struct qval*in=*_in;

	if(in->toget&BIN(index))	
	{	in->toget=in->toget^BIN(index);
		free(in->name[index]);
	}
	return(TRUE);
}

//Retrieves a value from a qval specified by a numerical index
void*qnget(struct qval*in,int n)
{	if(in->type[n]==QSTR)
		return(getcurunregbuffer(in->value[n]));
	return(in->value[n]);
}

//Retrieves a value from a qval
void*qget(struct qval*in,char*nam)
{	static int n=0;
	static char*name=0;

	if(!name)
		name=malloc(TINY);
	strcpy(name,nam);
	qtolower(&name);
	for(n=0;n<32;n++)	
	{	if(!(in->toget&BIN(n)))
			continue;	
		if(in->name[n])
		{	if(!strcmp(name,in->name[n]))
			{	switch(in->type[n])	
				{	case QBOL:	
					case QINT: 
						return((int*)in->value[n]);
						break;
					case QSTR:
						return(getcurunregbuffer(in->value[n]));
						break;
				}
			}
		}
	}	
	return(NULL);
}

void qclear(struct qval**_in)
{	static short n;
	static struct qval*in;
	in=(*_in);
	for(n=0;n<32;n++)
	{	if(in->value[n])	
		{	
#ifndef __FreeBSD__
			if(in->type[n]!=QINT)	
			{	free(in->value[n]);
			}
#endif
			in->value[n]=0;
		}
	}	
	in->toget=0;
	in->watch=0;
}

int query(char*_cin,struct qval**_in)
{	static struct qval*in;

	static int 	n=0,
			m=0,
			temp;

	static char	**out=0,
				*ins=0;
	n=0;
	m=0;
	in=(*_in);
	if(!ins)
		ins=malloc(QBUF);
	if(!out)
		out=malloc(4*SMALL);
	if(_cin)
		parse(&_cin,&out);
	for(;m<32;m++)
	{	if((in->toget<<m)<0)
		{	if(!out[n])
			{	if((in->flags[m]&QOPTIONAL)||(in->flags[m]&QREAD))
				{//	in->value[n]=blank;
					continue;
				}	
				printvalue(in->name[n],NULL,(int)NULL,QMAR);
				wfgets(ins,QBUF);
				ins[strlen(ins)-1]=0;//cut the newline
				if(!strlen(ins))
				{	message("Cancelled",RED,0,0);
					return(FALSE);
				}else
				switch(in->type[m])
				{	case QBOL:
					case QINT:
						in->value[m]=malloc(4);	
						temp=qgetnum(ins);
						memcpy(&in->value[m],&temp,4);
						break;
					case QSTR:
						qtolower(&ins);
						addtounregbuffer(in->value[m],ins);
						break;
				}
			}
			else	
			{	switch(in->type[m])
				{	case QBOL:	
					case QINT:	
						temp=atoi(out[n]);
						in->value[m]=(int*)temp;
						break;
					case QSTR:
						qtolower(&out[n]);
						addtounregbuffer(in->value[m],out[n]);
						break;
				}
			}
		}
		n++;
	}
	return(TRUE);
}

//Outputs qval names and values to the screen
int qprint(struct qval**_in)
{	
	struct qval*in=(*_in);
	static int n=0;

	for(n=0;n<32;n++)
	{
		if((in->toget<<n)<0)
		{
			printvalue(in->name[n],in->value[n],in->type[n],QMAR);
		}
	}
	return(TRUE);
}

//Sets a watch to report when a variable changes
int qsetwatch(struct qval**_in,char*str)
{
	static unsigned short index;
	struct qval*in=(*_in);

	qtolower(&str);
	for(index=0;index<32;index++)
	{
		if(in->toget&BIN(index))
		{
			if(!strncmp(str,in->name[index],strlen(str)))
			{
				in->watch|=BIN(index);
				return(TRUE);
			}
		}
	}
	return(FALSE);
}

#endif
