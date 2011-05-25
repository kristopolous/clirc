#ifndef _SETTINGS_
#define _SETTINGS_
#include"defines.h"
//This will write the qvals to a file.
//Should be really straight forward with its output
#define LBRACKET	'['
#define RBRACKET	']'

int dumpvals()
{
	static struct qval*in;
	static int sz, n, m;
	struct con*pt;
	static struct buffers_*toproc;
	FILE*ifile;
	char*sfile=qnget(g_ircvar, 14);

	if(!sfile)
	{
		return(FALSE);
	}
	if(!(ifile=fopen(sfile, "w")))
	{
		message("Failed to open file to write", RED, 0, 0);
	}
	for(sz=0;sz<qmain.size;sz++)
	{
		in=qmain.qvals[sz];	

		for(n=0;n<32;n++)
	   	{
			if((in->toget<<n)<0)
			{	fprintf(ifile, "[%s][%s]", qmain.names[sz], in->name[n]);
				if(in->value[n])
				{	switch(in->type[n])	
					{	case QSTR:
							toproc=(struct buffers_*)in->value[n];
							{	for(m=0;m<toproc->size;m++)
								{	wprintf("[%s]", toproc->data[m]);
								}
							}
							wprintf("Hello");
							fprintf(ifile, "[%s]", (char*)in->value[n]);
							break;
						default:
							fprintf(ifile, "[%d]", (int)in->value[n]);
							break;
					}
				}else
					fprintf(ifile, "[]");
				fprintf(ifile, "\012");
			}
		}
	}

	//contexts
	for(sz=0;sz<g_con.size;sz++)
	{	pt=g_con.cons[sz];
		for(n=0;n<3;n++)
		{	fprintf(ifile, "[Context][%d][%s]", sz, pt->vals->name[n]);
			in=pt->vals;
			if(in->value[n])
			{	switch(in->type[n])	
				{	case QSTR:
						fprintf(ifile, "[%s]", (char*)in->value[n]);
						break;
					case QBUF:
						fprintf(ifile, "ASHDFJKLASDFHASLKDFALSDKFJASDLKFJASDF");
					default:
						fprintf(ifile, "[%d]", (int)in->value[n]);
						break;
				}
			}else
				fprintf(ifile, "NULL");
			fprintf(ifile, "\012");
		}
	}
	fclose(ifile);
	return(TRUE);
}
#define NEXTCHAR(a, b)	a++; \
			if(*a[0]!=b) \
			{	wprintf("WARNING: Expecting %c,  got %c\n", b, *a[0]);\
			}

int setvals(void*nothing)
{	
	static int m, n, curcont, ncon;
	FILE*ifile;
	char*sfile, 
		*in=(char*)nothing, 
		*parsetable;
	static char
		*bufin, 
		*pbuf, 
		*tok[TINY], 
		**pcur;
	struct qval*tomod;
	bufin=malloc(LARGE);
	pbuf=bufin;
	m=0;

	sfile=qnget(g_ircvar, 14);
	while(!sfile||!(ifile=fopen(sfile, "r")))
	{	message("Cannot setup values from this file!", RED, 0, 0);
	        qclear(&g_toget);
		qassign(&g_toget, "Filename", QSTR, NULL, 0);
		if(!query(in, &g_toget))
        	        return(FALSE);
		qnset(&g_ircvar, 14, qnget(g_toget, 0), QSTR);
		sfile=qnget(g_ircvar, 14);
	}
	while(!feof(ifile))
	{	*pbuf=fgetc(ifile);
		pbuf++;
	}
	fclose(ifile);
	newparse(&parsetable);	
	CHARSTATE(parsetable, '[', P_REPL);
	CHARSTATE(parsetable, ']', P_REPL);
//For assigning buffers
	CHARSTATE(parsetable, ',', P_REPL);
	tableparse(parsetable, &bufin, &g_out);
	pcur=g_out;
	n=0;
	setcolor(CYAN);
	godark();
	while(*pcur)
	{	switch(*pcur[0])
		{	case LBRACKET:
				pcur++;
				tok[m]=*pcur;
	//Could be blank
				if(*tok[m]!=']')
				{	NEXTCHAR(pcur, ']');
				}else
					tok[m]=NULL;
				m++;
				break;
			case '\n':
				tomod=findqval(tok[0]);
				if(tomod)
				{ 	if(tok[2]!=NULL)
					{	if(qset(&tomod, tok[1], tok[2], QSTR))
						{	wprintf("%s: %s = %s\n", tok[0], tok[1], tok[2]);
						}else
						{	wprintf("You Failed!\n");
						}
					}
				}else
				{//Probably a context
					if(!strcmp(tok[0], "Context"))
					{	ncon=atoi(tok[1]);	//context	
						if(curcont!=ncon)
						{	curcont=ncon;	//set the context
							curc=newcon();
							setcon(NULL, NULL, 0, curc);
						}
						if(tok[3]!=NULL)
						{	if(qset(&curc->vals, tok[2], tok[3], QSTR))
							{	wprintf("[%d] %s = %s\n", atoi(tok[1]), tok[2], tok[3]);
							}else
							{	wprintf("You Failed!\n");
							}
						}
	
					}else
					{	wprintf("We lose");
					}
				}
				m=0;
				break;
		}
		pcur++;
	}
	return(TRUE);
}
#endif
