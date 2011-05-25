#ifndef _SERVERCOMS_
#define _SERVERCOMS_
#include"../defines.h"

int bouncer_start(void*nothing)
{ 	if(!bounceserver)	
		bounceserver=create_server(	(int)qnget(g_bouncevar,0),
						bouncewelcome,
						bounce,
						bouncebuf);
	bounceserver->timeout=10000000;
	if(!bounceserver->isactive)
		activate_server(&bounceserver);
	qset(&g_bouncevar,"active",(void*)TRUE,QINT);
	return(TRUE);
}

int bouncer_stop(void*nothing)
{	server_stop(bounceserver);
	qset(&g_bouncevar,"active",(void*)FALSE,QINT);
	return(TRUE);
}

int webserver_start(void*nothing)
{	if(!webserver)	
		webserver=create_server((int)qnget(g_servar,0),NULL,get,NULL);
	if(!webserver->isactive)
		activate_server(&webserver);
	qset(&g_servar,"active",(void*)TRUE,QINT);
	return(TRUE);
}

int webserver_stop(void*nothing)
{	server_stop(webserver);
	qset(&g_servar,"active",(void*)FALSE,QINT);
	return(TRUE);
}

int remote_start(void*nothingn)
{	if(!remoteserver)	
		remoteserver=create_server((int)qnget(g_remote,0),password,plaincline,NULL);
	if(!remoteserver->isactive)
		activate_server(&remoteserver);
	qset(&g_remote,"active",(void*)TRUE,QINT);
	return(TRUE);
}

int remote_stop(void*nothing)
{	server_stop(remoteserver);
	qset(&g_remote,"active",(void*)FALSE,QINT);
	return(TRUE);
}

#endif
