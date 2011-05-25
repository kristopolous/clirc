#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

#define BIN(x)  	(1<<(31-x))
#define SBAIT(t,v)	(t|=BIN(v))
#define GBAIT(t,v)	(t&BIN(v))

#define TRUE	1
#define FALSE	0

#define BUF_SIZ 262144

int g_size = 15;
char*g_nl="\n";

#define TK_NULL		0x00
#define TK_RAND		0x01
#define TK_VAR		0x02	
#define TK_PRINT	0x03
#define TK_ID		0x04

struct var
{	char*name;
	char**vals;
};

unsigned char token[BUF_SIZ],**data;
int n;

struct sym
{	struct var*symtab;
};

char isnum(char*totest)
{	if(*totest>='0')
		if(*totest<='9')
			return(TRUE);
	return(FALSE);
}

int tokenize(unsigned char**out)
{	n=0;
	while(out[n])
	{	if	(!strcmp(out[n],"randomize"))
			token[n]=TK_RAND;
		else if	(!strcmp(out[n],"print"))
			token[n]=TK_PRINT;
		else if (!strcmp(out[n],"var"))
			token[n]=TK_VAR;
		else
			token[n]=TK_ID;
		n++;
	}
	token[n]=TK_NULL;
	return(TRUE);
}

void execerror(char*in)
{	printf("ERROR:%s! :)\n",in);
	exit(0);
}

int f_print()
{	return(TRUE);
}

int f_rand()
{	return(TRUE);
}

int f_var()
{	return(TRUE);
}

int execute()
{	char*p=token;
	loop:
		switch(*p)
		{	case	TK_NULL:
					return(TRUE);
					break;
			case	TK_PRINT:
					f_print();
					break;
			case	TK_RAND:
					f_rand();
					break;
			case	TK_VAR:
					f_var();
					break;
			default:
					execerror("Unknown Token");
					break;
		}
		p++;
		goto loop;
	return(TRUE);
}


void parse(unsigned int len, unsigned char **_in, unsigned char ***_out)
{ 	unsigned char 	*in = (*_in),
		**out = (*_out),
		*end;

	end=in+len;

	if (!len) 
	{ 	*out = 0;
		return;
	}
	while (*in == 32)
		++in;	
	*out = in;

	while (in!=end)
	{       if(*in>32)
		{	in++;
			continue;
		}
		if(*in!=' ')
		{       if(*out)
				if(*out[0]>32)
					out++;
			*out=g_nl;
		}
		*in=0;
		in++;
		if(*in>32)
		{       out++;
			*out=in;
		}
	}
	out[1] = 0;
	out[2] = 0;
}

void*getval(int n)
{	return(NULL);
}

struct var*f_randomize()
{	if(token[n+1]==TK_ID)
	{	if(1)
		{
		}else
		{	execerror("Tried to randomize an undefined variable");
		}
	}
	else
	{	execerror("Tried to randomize a nonid");
	}
	return(NULL);
}

struct var*var()
{	if(token[n+1]==TK_ID)
	{	if(1)
		{
		}else
		{	execerror("Was not able to create variable");
		}
	}
	else
	{	execerror("Tried to initialize a nonid");
	}
	return(NULL);
}

struct var*print()
{	n++;	
	if(token[n]==TK_ID)
	{	if(getval(n))
		{	
		}else
		{	execerror("Tried to print an undefined variable");
		}
	}
	else
	{	execerror("Asked to print something that wasn't an ID");
	}
	return(NULL);
}

int main(int argc,char*argv[])
{	//Token creation
 	int 	file, 
		ret;

	unsigned char 	*buffer,
		*pbuf;
	struct tok*totok;

	n=BUF_SIZ;
	data = malloc(BUF_SIZ);

	if (argc > 1) 
	{ 	file = open(argv[1], O_RDONLY);
		if (file == -1)
			return (printf("No such file\n"));
		printf("Found file\n");
	} else
		return (printf("Include filename as param\n"));

	buffer = malloc(BUF_SIZ);
	pbuf = buffer;

	for (;;) 
	{	ret = read(file,pbuf,BUF_SIZ);	
		if (BUF_SIZ == ret)
		{	n+=BUF_SIZ;
			buffer=realloc(buffer,n);
			pbuf=n+buffer-BUF_SIZ;
		} else
			break;
	}
	pbuf+=ret;

	close(file);
	parse(pbuf-buffer, &buffer, &data);
	tokenize(data);
	execute();

	return (TRUE);
}
