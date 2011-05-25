#ifndef _SCRIPT_
#define _SCRIPT_
#include"../defines.h"

int sc_label(void *in)
{
	return(TRUE);
}

int sc_goto(void *in)
{
	return(TRUE);
}

int sc_if(void *nothing)
{ 	
	char*in=(char*)nothing;
	static int toret;

	qclear(&g_toget);
	qassign(&g_toget, "Variable to test", QSTR, NULL, 0);
	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	toret=(int)qget(gp_curcont, (char*)qnget(g_toget, 0));
	return(toret);
}

int sc_end(void *in)
{
	return(TRUE);
}

int sc_var(void *in)
{
	return(TRUE);
}

int sc_watch(void *nothing)
{
	char*in=(char*)nothing;
	qclear(&g_toget);
	qassign(&g_toget, "Variable", QSTR, NULL, 0);
	if(!query(in, &g_toget))
	{
		return(FALSE);
	}
	if(qsetwatch(&gp_curcont, (char*)qnget(g_toget, 0)))
	{
		message("Watching . . .", BLUE, 0, 0);
	}
	else
	{
		message("Failure! Can't Watch that!", RED, 0, 0);
	}
	
	return(TRUE);
}

#endif
