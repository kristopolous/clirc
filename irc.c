#ifndef _IRC_
#define _IRC
#include "defines.h"

void PipeHandler(int Signal)
{
	printf("%d\n",Signal);
	return;
}

int ircsend(char*msg)
{	
	static int t=0;	
#ifdef _DEBUG_
	wprintf("%s",msg);
#endif
	if(msg)
	{
		addtobuffer("out",msg);
	}
	if(qnget(gp_curcont,3)==0)
	{
		return(FALSE);
	}
        if(poll(g_ufds,MAX_FDS,10))
        {
	 	if(g_ufds[0].revents&badstuff)
                {
		 	message("Connection dropped!",RED,0,0);
			ircdisconnect(NULL);
			g_ufds[0].revents=0;
			return(FALSE);
                }
	}
/*	if(time(NULL)==t)
		return(FALSE);
//Want to avoid being kicked for flooding, but also want a reasonable time
*/	msg=getbuffer("out");
	if(msg)
		send(g_sd,msg,strlen(msg),0);
	t=time(NULL);
	return(TRUE);
}

void server_mode()
{	struct server*clirc;

	clirc=create_server(6650,NULL,plaincline,NULL);
	activate_server(&clirc);
	printf("Server port: %d\n",clirc->port);
	loop:
		getstuff(SAVE);
		ircsend(NULL);
		poll_servers();
		goto loop;
	do_exit(0);
}

int main(int argc,char*argv[])
{	char 	*config=0,
			v_server=0;

	int 	n	=0,
		one	=1,
		ten	=10,
		thou	=1000,
		remote	=6650,
		sixthou	=6640,
		bounce	=6630;

	//
	// Handle broken pipes
	//
	signal(SIGPIPE, PipeHandler);

	addvarcom(&g_globalcoms,NULL,TRUE);
	addvarcom(&g_irccoms,&g_ircvar,TRUE);
	addvarcom(&g_chancoms,NULL,TRUE);
	addvarcom(&g_scriptcoms,NULL,TRUE);
	addvarcom(&g_servercoms,&g_servar,TRUE);
	addvarcom(&g_bouncecoms,&g_bouncevar,TRUE);
	addvarcom(&g_remotecoms,&g_remote,TRUE);
	addvarcom(&g_concoms,&g_convar,FALSE);
	addvarcom(&g_bufcoms,NULL,TRUE);

	g_canjoin	=TRUE;
	bigserver.count	=0;
	towrite		=malloc(MEDIUM);

	qmain.size=0;
	qaddtobig(g_ircvar,"Main Variables");
	qaddtobig(g_servar,"Server Variables");
	qaddtobig(g_bouncevar,"Bouncer Variables");
	qaddtobig(g_convar,"Connection Variables");

	g_out=(char**)malloc(LARGE);
	g_toget=malloc(sizeof(struct qval));

	ZERO(g_out,LARGE);
	ZERO(g_con.cons,SMALL*4);

	srand(time(NULL));
	qassign(&g_servar,"Port",QINT,&sixthou,0);
	qassign(&g_servar,"Logfile",QSTR,NULL,1);
	qassign(&g_servar,"Active",QBOL,&n,2);

	qassign(&g_bouncevar,"Port",QINT,&bounce,0);
	qassign(&g_bouncevar,"Logfile",QSTR,NULL,1);
	qassign(&g_bouncevar,"Active",QBOL,&n,2);

	qassign(&g_remote,"Port",QINT,&remote,0);
	qassign(&g_remote,"Logfile",QSTR,NULL,1);
	qassign(&g_remote,"Active",QBOL,&one,2);
	qassign(&g_remote,"CryptPass",QSTR,NULL,3);
	qsetfunc(&g_remote,3,setpass);

	qassign(&g_ircvar,"Verbose",QINT,&n,3);
	qassign(&g_ircvar,"Auto-reconnect",QBOL,&n,4);
	qassign(&g_ircvar,"Save raw",QBOL,&n,5);
	qassign(&g_ircvar,"Logfile",QSTR,NULL,6);
	qassign(&g_ircvar,"Backoff",QINT,&thou,7);
	qassign(&g_ircvar,"Message Log",QBOL,&n,8);
	qassign(&g_ircvar,"Tries",QINT,&ten,9);
	qassign(&g_ircvar,"Reconnect?",QBOL,&one,11);
	qassign(&g_ircvar,"Auto-join",QBOL,&one,12);
	qassign(&g_ircvar,"Auto-affinity",QBOL,&one,13);
	qassign(&g_ircvar,"Settings",QSTR,NULL,14);
	qassign(&g_ircvar,"Auto-Client",QBOL,&n,15);
	qsetfunc(&g_ircvar,6,openlog);

	hashinit();
 
	setcolor(GREEN);
	dospace(0);
	
	addsub(&g_irccoms,"context",g_concoms,g_convar);
	addsub(&g_irccoms,"dbase",g_chancoms,g_ircvar);
	addsub(&g_irccoms,"bouncer",g_bouncecoms,g_bouncevar);
	addsub(&g_irccoms,"server",g_servercoms,g_servar);
	addsub(&g_irccoms,"script",g_scriptcoms,g_ircvar);
	addsub(&g_irccoms,"buffer",g_bufcoms,g_ircvar);
	addsub(&g_irccoms,"remote",g_remotecoms,g_remote);

	addcom(&g_globalcoms,"context",context,NULL,"Set a context");
	addcom(&g_globalcoms,"add",cadd,NULL,"Add a fallback");
	addcom(&g_globalcoms,"connect",ircconnect,NULL,"Connect to the server context");
	addcom(&g_globalcoms,"quit",clexit,NULL,"Exit program or subshell");
	addcom(&g_globalcoms,"exit",clexit,NULL,"Exit program or subshell");
	addcom(&g_globalcoms,"delete",condel,NULL,"Delete a connection");
//	addcom(&g_globalcoms,"qreset",reset,NULL,"Reset terminal");
//	addcom(&g_globalcoms,"help",clshowcom,(void*)(*coms),"Show help");

	addcom(&g_irccoms,"join",join,NULL,"Join a channel");
	addcom(&g_irccoms,"part",part,NULL,"Part a channel");
	addcom(&g_irccoms,"disconnect",ircdisconnect,NULL,"Disconnect from server");
	addcom(&g_irccoms,"exthelp",exthelp,NULL,"Extended help");
	addcom(&g_irccoms,"raw",raw,NULL,"Drop to raw irc mode");
	addcom(&g_irccoms,"load",load,NULL,"Load configuration from a file");
	addcom(&g_irccoms,"chat",chat,NULL,"Chat in a channel or with an individual");
	addcom(&g_irccoms,"ctcp",ctcp,NULL,"Send a CTCP type command");
	addcom(&g_irccoms,"svals",setvals,NULL,"Setup values");

	addcom(&g_bufcoms,"show",showbuffers,NULL,"List buffers");
	addcom(&g_bufcoms,"print",print,NULL,"Print a buffer");
	addcom(&g_bufcoms,"messages",showmesg,NULL,"System Messages");
	addcom(&g_bufcoms,"clear",clear,NULL,"Clear a buffer");
	addcom(&g_bufcoms,"goto",shift,NULL,"Shift the current pointer of a buffer");
//	addcom(&g_bufcoms,"add",add,NULL,"Add a fallback");

	addcom(&g_chancoms,"chanlist",chanlist,NULL,"List all channels in dbase");
	addcom(&g_chancoms,"nicklist",nicklist,NULL,"List all nicknames in dbase");
	addcom(&g_chancoms,"print",print,NULL,"Print recent server messages");
	addcom(&g_chancoms,"chat",chat,NULL,"Chat in a channel or with an individual");
	addcom(&g_chancoms,"join",join,NULL,"Join a channel");
	addcom(&g_chancoms,"part",part,NULL,"Part a channel");
	addcom(&g_chancoms,"affinity",affinity,NULL,"Show the affinity of a channel");
	addcom(&g_chancoms,"walk",walk,NULL,"Walk hash");
	addcom(&g_chancoms,"list",list,NULL,"Do a /list and a database build");

	addcom(&g_scriptcoms,"sleep",ircsleep,NULL,"Sleep for n usecs");
	addcom(&g_scriptcoms,"print",script_print,NULL,"Print a string");
	addcom(&g_scriptcoms,"populate",populate,NULL,"Populate channels");
	addcom(&g_scriptcoms,"label",sc_label,NULL,"Set a label");
	addcom(&g_scriptcoms,"goto",sc_goto,NULL,"Goto a label");
	addcom(&g_scriptcoms,"if",sc_if,NULL,"Begin conditional block");
	addcom(&g_scriptcoms,"end",sc_end,NULL,"End conditional block");
	addcom(&g_scriptcoms,"watch",sc_watch,NULL,"Watch a variable");

	addcom(&g_servercoms,"start",webserver_start,NULL,"Startup the webserver");
	addcom(&g_servercoms,"kill",webserver_stop,NULL,"Kill the webserver");

	addcom(&g_bouncecoms,"start",bouncer_start,NULL,"Startup the server");
	addcom(&g_bouncecoms,"kill",bouncer_stop,NULL,"Kill the server");

//	addcom(&g_concoms,"add",cadd,NULL,"Add a fallback");
	addcom(&g_concoms,"new",conadd,NULL,"Add a connection");
	addcom(&g_concoms,"context",context,NULL,"Set a connection context");

	addcom(&g_concoms,"set",cset,NULL,"Set a variable");
	addcom(&g_concoms,"show",showcon,NULL,"Show the value of a variable");

	addcom(&g_remotecoms,"password",setpass,NULL,"Set the remote access password");
	addcom(&g_remotecoms,"kill",remote_stop,NULL,"Disallow remote access");
	addcom(&g_remotecoms,"start",remote_start,NULL,"(Re)allow remote access");

	g_curprompt=&g_headprompt;
	g_curprompt->vars=g_ircvar;
	g_curprompt->coms=g_irccoms;
	g_curprompt->prompt=0;
	g_curprompt->color=BLUE;

//	curc=newcon();
//	setcon(NULL,NULL,0,curc);

	g_curprompt->vars=g_ircvar;

	golight();

	stdoutin=create_stream("/dev/stdout",NULL,NULL,NULL);
	stdoutin->hascolor=TRUE;
	stdoutin->pri=0;
	g_curs=stdoutin;

	while(argc>0)
	{ 	
		if(!strcmp(*argv,"-s"))
		{
			v_server=1;	
		}
	 	else if(!strcmp(*argv,"-h"))
		{
			exthelp("usage");	
			do_exit(0);
		}
	
	 	else if(!strcmp(*argv,"--help"))
		{	
			exthelp("usage");	
			do_exit(0);
		}

	 	else if(!strcmp(*argv,"-help"))
		{
			exthelp("usage");	
			do_exit(0);
		}

		else if(!strcmp(*argv,"-r"))
		{
			reattach(*(argv+1));
			do_exit(0);
		}
		else 
		{
			config=*argv;
		}

		argv++;
		if(argc)
		{
			argc--;
		}
	}
	
       	if(config) 
	{
         	load(config);	
	}
	if(v_server)
	{
		g_polltime=100;
		server_mode();
	}
	g_polltime=20;

	remoteserver=create_server(6650,password,plaincline,NULL);
	activate_server(&remoteserver);

        g_con.curpoint=0;

	printf("[0m - - - - -/\\ | ^ - - - - - - - -\n");
	printf("|        |   | | |/\\ /\\         | %s",VERSION);
#ifdef _DEBUG_
	printf("-DEBUG");
#endif
	printf("\n - - - - -\\/ | | | - \\_- - - - -\n");
	
#ifdef _DEBUG_
	{
		int ix;

		for(ix=0;ix<SMALL;ix++)
		{
			if(g_varcom.ccoms[ix])
			{	
				wprintf("%d is defined\n",ix);
				showcom(g_varcom.ccoms[ix]);
			}
		}
	}
#endif
	loop:
		mainloop();
	goto loop;
}
#endif
