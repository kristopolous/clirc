//seed the 2nd character . . . index[1]
//We further make the ridiculous assumption that the second character
//is [a-z]

#ifndef _BUFFER_
#define _BUFFER_
#include "defines.h"

int showbuffers();
int printbuffer(char *);
int clearbuffer(char *);
int addtobuffer(char *, char *);
int copytobuffer(char *, char *);
int deletebuffer(char *);

#define BSEED	seed=word[1]-'a';
#define BFUNC	seed=(seed+BIN(n++))%TINY;

int find_buffer(char *word, struct buffers_**_ret)
{	
	static int n, seed;
	n=0;
	BSEED

	while(curc->buffers[n].name)
	{	
		if(!strncmp(curc->buffers[n].name, word, strlen(word)))
		{
			(*_ret)=&curc->buffers[n];
			return(TRUE);
		}
		BFUNC
	}
#ifdef _DEBUG_
	wprintf("DEBUG: in find_buffer: %s not found\n", word);
#endif
	return(FALSE);
}

struct buffers_*createbuffer(char *word, unsigned short nm)
{	
	int n=0, seed;
	static struct buffers_*toadd = 0;

	qtolower(&word);
	if(find_buffer(word, &toadd))
	{
		return(NULL);
	}
	BSEED
	while(curc->buffers[n].name)
	{
		BFUNC	
	}
	curc->buffers[n].name = CHR(strlen(word));
	strcpy(curc->buffers[n].name, word);
	curc->buffers[n].data = malloc(MEDIUM);
	memset(curc->buffers[n].data, 0, MEDIUM);
	curc->buffers[n].size = 0;
	curc->buffers[n].curpoint = 0;

	return(&curc->buffers[n]);
}

struct buffers_*createunregbuffer(char *word)
{	
	static struct buffers_*toadd;
	toadd=malloc(sizeof(struct buffers_));
	memset(toadd, 0, sizeof(struct buffers_));

	if(word)
	{
		qtolower(&word);
		toadd->name=CHR(strlen(word));
		strcpy(toadd->name, word);
	}
	toadd->data=malloc(MEDIUM);
	memset(toadd->data, 0, MEDIUM);
	toadd->size = 0;
	toadd->curpoint = 0;
	return(toadd);
}

int deletebuffer(char *buf)
{	static struct buffers_*toadd;
	if(find_buffer(buf, &toadd))
	{	free(toadd->data);
		toadd->data=0;
		free(toadd->name);
		toadd->name=0;
	}	
	return(TRUE);
}

int showbuffers()
{	
	int n = 0;

	if(!curc)
	{
		return(FALSE);
	}
	while(curc->buffers[n].name)
	{	printvalue(	curc->buffers[n].name, 
				(void*)curc->buffers[n].size, 
				QINT, QMAR);
		n++;
	}
	return(TRUE);
}

int printbuffer(char *buf)
{ 	
	struct buffers_ *toadd = 0;
	static int n = 0, 
		  m = 0;

	if(find_buffer(buf, &toadd))
	{	
		godark();	
		m=toadd->size;

		for(n=0;n<m;n++)
		{
			if(n==toadd->curpoint&&toadd->curpoint)
			{
				message("Data not sent yet", MAGENTA, 0, 0);
			}
			message(toadd->data[n], GREEN, 0, 0);
		}
		goback();
		return(TRUE);
	}
#ifdef _DEBUG_
	wprintf("Unable to printbuffer %s\n", buf);
#endif
	return(FALSE);
}

int clearbuffer(char *buf)
{ 	
	struct buffers_ *toadd = 0;
	static int n = 0;

	if(find_buffer(buf, &toadd))
	{	
		for(n = toadd->size; n; n--)
		{
			free(toadd->data[n]);	
		}

		toadd->size = 0;
		toadd->curpoint = 0;
		return(TRUE);
	}
#ifdef _DEBUG_
	wprintf("Unable to clearbuffer %s\n", buf);
#endif
	return(FALSE);
}

char *getbuffer(char *buf)
{	
	static struct buffers_ *toadd = 0;
	static char *ret = 0;

	find_buffer(buf, &toadd);

	if(toadd->curpoint == toadd->size)
	{
		return(0);
	}
	ret=toadd->data[toadd->curpoint];
	toadd->curpoint++;

	return(ret);
}

char *getcurunregbuffer(struct buffers_*toget)
{	
	return(toget->data[toget->curpoint]);
}

int addtobuffer(char *buf, char *data)
{
	struct buffers_ *toadd = 0;

	if(!(find_buffer(buf, &toadd)))
	{
		createbuffer(buf, NOBUFFER);
		find_buffer(buf, &toadd);
	}

	toadd->data[toadd->size] = malloc(strlen(data));
	ASSERT(toadd->data[toadd->size]);

	memcpy(toadd->data[toadd->size], data, strlen(data) + 1);
	toadd->size++;

	return(TRUE);
}

int addtounregbuffer(struct buffers_*toadd, char *data)
{
	toadd->data[toadd->size] = malloc(strlen(data) + 1);
	ASSERT(toadd->data[toadd->size]);

	memset(toadd->data[toadd->size], 0, strlen(data) + 1);
	strcpy(toadd->data[toadd->size], data);
	toadd->size++;

	return(TRUE);
}

int replacecurunregbuffer(struct buffers_*toadd, char *data)
{
	if(toadd->data[toadd->curpoint])
	{
		free(toadd->data[toadd->curpoint]);	
	}

	toadd->data[toadd->curpoint] = malloc(strlen(data) + 1);
	strcpy(toadd->data[toadd->curpoint], data);

	return(TRUE);
}

int incrunregbuffer(struct buffers_ *toinc)
{	
	if((toinc->size - toinc->curpoint) > 1)
	{
		toinc->curpoint++;
		return(TRUE);
	}
	else
	{
#ifdef _DEBUG_
		wprintf("Unable to incrunregbuffer\n");
#endif
		return(FALSE);
	}
}

int decunregbuffer(struct buffers_ *toinc)
{	
	if(toinc->curpoint > 0)
	{	
		toinc->curpoint--;
		return(TRUE);
	}
	else
	{
#ifdef _DEBUG_
		wprintf("Unable to decrunregbuffer\n");
#endif
		return(FALSE);
	}
}

int addtobuffernl(char *buf, char *data)
{
	struct buffers_*toadd;

	if(!(find_buffer(buf, &toadd)))
	{
		createbuffer(buf, NOBUFFER);
		find_buffer(buf, &toadd);
	}

	strcat(data, "\n");

	toadd->data[toadd->size] = data;
	toadd->size++;

	return(TRUE);
}

int copytobuffer(char *buf, char *data)
{	
	static struct buffers_ *toadd = 0;

	if(!(find_buffer(buf, &toadd)))
	{
		createbuffer(buf, NOBUFFER);
		find_buffer(buf, &toadd);
	}

	toadd->data[toadd->size] = malloc(strlen(data)+1);
	ASSERT(toadd->data[toadd->size]);

	strcpy(toadd->data[toadd->size], data);
	toadd->size++;

	return(TRUE);
}
#endif
