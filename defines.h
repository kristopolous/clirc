#ifndef _DEFINES_
#define _DEFINES_

#define VERSION	"0.0.1.9.3c"

#include<arpa/inet.h>
#include<fcntl.h>
#include<langinfo.h>
#include<netdb.h>
#include<netinet/in.h>
#include<signal.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/poll.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<termios.h>
#include<time.h>
#include<unistd.h>

#ifdef bzero
#define ZERO(a,b)	bzero(a,b)
#else
#define ZERO(a,b)	memset(a,0,b)
#endif

#define ABSOLUTE	2
#define ACCE		3

#define ASSERT(n)\
if ( !(n) )\
{	printf("<ASSERT FAILURE@%s:%d>",__FILE__,__LINE__);\
	fflush(0);\
	__asm("int $0x3");\
}

#define BIN(x)  	(1<<(31-x))
#define CHR(sz)		malloc(sz)
#define COLOR(x)	"\033[1;3xm"
#define CONL		4
#define CONT		5
#define C_ACHR		5
#define C_ACPT		2
#define C_AENC		4
#define C_ALNG		3
#define C_HOST		0
#define C_IFMD		6
#define C_USER		1
#define CHARSTATE(table,index,state)    table[index]=state;
#define CHARRESET(table,index)          table[index]=P_ADD;
#define CRASH		*(int*)0=0;
#define DATE		0
enum
{	FALSE,
	TRUE,
	DONE,
	FAIL,
	WAITING,
	ERROR
};
#define GETFLAG(n,f)	(n&BIN(f))
#define H_CHAN		1
#define H_MAX		8
#define H_NAME		0
#define H_SMALL		8 
#define LARGE		65536   //2^16
#define LEVEL		(int)qnget(g_ircvar,3)
#define LMOD		2
#define MAX_ARGS	20
#define MAX_CONS	20
#define MAX_FDS		30
#define MAX_SVRS	32	//Maxmimum number of IRC servers
#define MEDIUM  	4096    //2^11
#define MEDLAR		16384	//2^14
#define NOBUFFER	0xffff
#define NOFOUND		-1
#define OUTGOING	0xffff
#define P_ADD		0
#define P_REPL		1
#define P_REPLONCE	2
enum
{	QINT=1,
	QBOL,
	QSTR,
	QNL,
	QVAL,
	QLIN
};
enum
{	RED=1,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE
};
enum
{	PRINT=1,
	RAW,
	SAVE,
	DUMP
};
#define QBUF		80
#define QENCVER		1	//Version of the encoder
#define QFLAG		0xF0	//0b11110000 flags
#define QMAR		15
#define QOPTIONAL	0x20
#define	QREAD		0x80	//Read only
#define REDIRECT	302
#define RELATIVE	1
#define SERV		1
#define SETFLAG(n,f)	n|=BIN(f)
#define SLEEP		usleep((int)qnget(g_ircvar,7));
#define SMALL		256	//2^8
#define TIMEOUT		30
#define TINY		63	//2^5-1, used for hashes
#define VAR		1

struct _head
{       
	char    set;
        char*   text;

} 
head[]=
{
	{1,"Date:"},
	{1,"Server:"},
	{1,"Last-Modified:"},
	{1,"Accept-Ranges:"},
	{1,"Content-Length:"},
	{1,"Content-Type:"},
	{0,0}
};

struct _chead
{      
       	char    set;
        char*   name;
        char    value[MEDIUM];
} 
chead[]=
{
	{0,"Host:",{0}},
	{0,"User-Agent:",{0}},
	{0,"Accept:",{0}},
	{0,"Accept-Language:",{0}},
	{0,"Accept-Encoding:",{0}},
	{0,"Accept-Charset:",{0}},
	{0,"If-Modified-Since:",{0}},
	{0,0,{0}}
};

struct bigqval
{	
	char	*names[32];
	struct 	qval*qvals[32];
	short 	size;
};

struct bserver_
{	
	struct server*servers[32];
	char count;
};

struct buffers_
{	
	char	*name,
		**data;
	int 
		size,
		curpoint;
};

//Used to maintain multiple curpoints
struct bufferpointer
<<<<<<< defines.h
{
	unsigned char 
=======
{	char 
>>>>>>> 1.34
		**name,
		***data;
	int 
		*size,
		curpoint;
};

struct ccom
<<<<<<< defines.h
{      
       	unsigned char	*name,
=======
{       char	*name,
>>>>>>> 1.34
        	*description,
        	type,
        	value;

        int     (*function)(void*);

        void    *arg;

	struct	ccom	*sub;
	struct  qval	*subvars;

        struct  ccom	*left,
        		*right,
        		*head;
};

struct con
{
	struct buffers_	*hosts,	//point to the entry in vals
			*nicks,
			buffers[TINY];

	struct qval*	vals;

	int	*curhost,
		*curnick,
		ufd,
		fd,
		mynum;
};

struct cons_
{	struct con*	cons[SMALL];
	int 	curpoint;
	unsigned short 	size;
};

struct curprompt_
{	struct qval	*vars;
	struct ccom	*coms;
	char	*prompt;
	int 		color;
	struct curprompt_
			*next,
			*prev;
};

struct irccom_
{       int 		number;
        char*	message;
};

struct pqfunc 
{	int (*function)(void*); 
};

struct qval
{       char	
		*name[32],
         	type[32],	//QINT, QSTR etc
		flags[32];	//QOPTIONAL, QREAD, QWRITE
        void*	value[32];
	struct pqfunc 	func[32];	
	signed   int 	toget;	//*******MUST BE SIGNED.  The BIN test is a negative test********
       	unsigned int 	watch;
};

struct random
{	char	**values;
	double		*chance;	// 0<x<100
	//Internal representation
	int		*bps;
	int		indices;
};

struct section
{	struct qval*	qvals;
	struct ccom*	coms;
	char*		name;
};

struct server
{
	int	port,
	 	pri,
		prifd,
		curcon,
		concount,

		cur[MAX_CONS],
		curfd[MAX_CONS],	//current connection
		timeout,

		logfd;

	socklen_t	addrlen;

	unsigned int	last[MAX_CONS];	//UNIX time of last activity

	int	(*inifunction)(char*,struct server*,int);	//initial function
	int 	(*function)(char*,struct server*);	//Function to execute on input
	int 	(*buffunction)(struct server*);	//What to do when the buffer pointers are not updated

	struct sockaddr		addr;
	
	struct bufferpointer	buffers[TINY];

	char	isactive,
			isconnected[MAX_CONS],
			hascolor,
			inistatus[MAX_CONS];	//ini function status

	struct qval*	qvals;
	struct curprompt_ svp;		//not the best option - this should only be a temp solution
	void*		misc[MAX_CONS][8];	//miscellaneous general purpose pointers
};

struct user
{       char	
		*name,
        	*host,
        	*usr;
};

struct varcom
{	struct ccom*ccoms[SMALL];
	struct qval*qvals[SMALL];
	unsigned short count;
};

void 	activate_server(struct server**);
int 	add(void*);
struct ccom*addcom(struct ccom**com,char*name,int(*function)(void*),void*,char*);
void	addinfo(char*,char*);
int 	addtobuffer(char*,char*);
int 	addtobuffernl(char*,char*);
int 	addtounregbuffer(struct buffers_*,char*);
void	addvarcom(struct ccom**,struct qval**,char);
char*	
	itoa(int);
int 	bounce(char*,struct server*);
int	bouncebuf(struct server*);
int	bouncewelcome(char*,struct server*,int);
int 	bouncer_start(void*);
int 	bouncer_stop(void*);
struct tm*
	chrtotm(char*);
int 	clearbuffer(char*);
int 	clexit(void*);
int 	clshowcom(void*);
int 	cline(struct ccom**,char,char*);
int 	plaincline(char*,struct server*);
int 	clineproc(struct ccom**,char*);
void	clinit(struct ccom**);
int 	combine(char**,int,char**);
int 	copytobuffer(char*,char*);
struct server*
	create_server(int,
			int(*inifunction)(char*,struct server*,int),
			int(*function)(char*,struct server*),
			int(*buffunction)(struct server*));
struct buffers_*
	createbuffer(char*,unsigned short);
struct buffers_*
	createunregbuffer(char*);
int 	deletebuffer(char*);
int 	delcon(char*,char*,int);
void 	do_exit(int);
void 	dospace(int);
int	dumpvals();
int	exthelp(void*);
int 	findcom(struct ccom*,char*,struct ccom**);
int 	findcon(char*,char*,int);
struct qval*
	findqval(char*);
int 	get(char*,struct server*);
char* 	getcurunregbuffer(struct buffers_*);
char*	gethelp(char*);
char* 	getlongtime();
void	getstuff(int);
char* 	getshorttime();
void 	godark();
void 	goback();
void 	golight();
void 	hashadd(char*,char*,char*);
int 	hashdelete(char*,char*);
unsigned short hashfind(char*);
void	hbuild();
int 	ircconnect(void*);
int 	ircdisconnect(void*);
void 	ircreg();
int 	incrunregbuffer(struct buffers_*);
int 	ircsend(char*);
int 	join(void*);
void	mainloop();
void 	message(char*,int,int,int);
int 	nextline(char**,int);
struct con*	newcon();
int	newparse(char**);
char* 	numtotime(time_t);
void 	parse(char**,char***);
void 	parsenl(char**,char***);
int 	part(void*);
int	password(char*,struct server*,int);
void 	patience();
void	PipeHandler(int);
void 	poll_servers();
int 	popprompt();
int 	printbuffer(char*);
void 	printstring(char*,int);
void 	printvalue(char*,void*,int,int);
int 	process(char*in,int);
void 	protected_write(struct server*,char*);
void 	pushprompt(struct qval*,struct ccom*,char*,int);
int 	put(char**,int);
int 	qadd(struct qval**,char*,void*,int);
void 	qaddtobig(struct qval*,char*);
int 	qassign(struct qval**,char*,char,void*,int);
void	qclear(struct qval**);
void 	qdelfrombig(struct qval*,char*);
void*	qget(struct qval*,char*);
int 	QINTqnset(struct qval**,int,int);
int 	qmove(struct qval**,char*,int,short);
int 	qnset(struct qval**,int,void*,int);
int 	qnreset(struct qval**,int);
int 	qnext(struct qval**,char*);
int 	qnnext(struct qval**,int);
int 	qnprev(struct qval**,int);
void*	qnget(struct qval*,int);
int 	qprev(struct qval**,char*);
int 	qprint(struct qval**);
int 	qreset(struct qval**,char*);
int 	qrunfunc(struct qval**,int);
int 	qset(struct qval**,char*,void*,int);
int	qsetfunc(struct qval**,int,int(*function)(void*));
int 	qsetwatch(struct qval**,char*);
void	qtolower(char**);
int 	query(char*,struct qval**);
char*	quenc(char*);
int 	replacecurunregbuffer(struct buffers_*,char*);
void 	revertcolor();
char*	
	rprintf(char*,...);
int 	sc_end(void*);
int 	sc_goto(void*);
int 	sc_if(void*);
int 	sc_label(void*);
void 	setcolor(int);
int 	setcon(char*,char*,int,struct con*);
int	setpass(void*);
int	setvals(void*);
int	server_stop(struct server*);
int	serverassociate(struct server*,struct con*);
int 	showcom(struct ccom*);
int 	shift(void*);
int 	showbuffers();
void	singlehashadd(char*,char*);
int 	subshell(void*);
int	tableparse(char*,char**,char***);
int	twovarset(char*,char*);
void 	unparse(char***);
int 	urlprint(char*,char**);
int 	walk(void*);
char*	wfgets(char*,int);
void 	whois(void*);
int 	wprintf(char*,...);
void	wupdate();

struct server	*g_curs;
struct bigqval 	qmain;
struct bserver_	bigserver;
struct buffers_	/*g_buffers[TINY],*/
		g_varbufs[TINY];

struct ccom	*g_globalcoms,
		*g_bufcoms,
		*g_irccoms,
		*g_chancoms,
		*g_concoms,
		*g_scriptcoms,
		*g_bouncecoms,
		*g_servercoms,
		*g_remotecoms;

struct con	*curc;	//Current context
struct cons_	g_con;

struct curprompt_	g_headprompt,
			*g_curprompt;


struct irccom_ 	g_irccom[SMALL];
struct pollfd 	g_ufds[MAX_FDS];

struct qval	*g_bouncevar,
		*g_convar,
		*g_ircvar,
		*g_servar,
		*g_toget,
		*g_remote,
		*g_context[MAX_SVRS],
		*gp_curcont;	//pointer to current context

struct server	*stdoutin=0,
		*webserver=0,
		*bounceserver=0,
		*remoteserver=0;

struct varcom	g_varcom;
int 	c_hchans,	//hash table channel database
	c_hnames,	//hash table name database
	g_fd=0,
	g_fdstdin,
	g_polltime,
	g_sd,
	g_serv;

FILE*	g_log;

unsigned short	badstuff=POLLERR|POLLHUP|POLLNVAL,
		g_backoff=100,
		g_canjoin,
		g_sflag=0,
		g_temp;
/*const*/ char	*g_chrs[]=
	{	"\000", "\001", "\002",	"\003",
		"\004",	"\005", "\006",	"\007",
		"\010",	"\011", "\012",	"\013",
		"\014",	"\015", "\016",	"\017",
		"\020",	"\021", "\022",	"\023",
		"\024",	"\025", "\026",	"\027",
		"\030",	"\031", "\032",	"\033",
		"\034",	"\035", "\036",	"\037",
		"\040", "\041", "\042", "\043",
		"\044", "\045", "\046", "\047",
		"\050", "\051", "\052", "\053",
		"\054", "\055", "\056", "\057",
		"\060", "\061", "\062", "\063",
		"\064", "\065", "\066", "\067",
		"\070", "\071", "\072", "\073",
		"\074", "\075", "\076", "\077",
		"\100", "\101", "\102", "\103",
		"\104", "\105", "\106", "\107",
		"\110", "\111", "\112", "\113",
		"\114", "\115", "\116", "\117",
		"\120", "\121", "\122", "\123",
		"\124", "\125", "\126", "\127",
		"\130", "\131", "\132", "\133",
		"\134", "\135", "\136", "\137",
		"\140", "\141", "\142", "\143",
		"\144", "\145", "\146", "\147",
		"\150", "\151", "\152", "\153",
		"\154", "\155", "\156", "\157",
		"\160", "\161", "\162", "\163",
		"\164", "\165", "\166", "\167",
		"\170", "\171", "\172", "\173",
		"\174", "\175", "\176", "\177",
	};

char	
	*blank,
	*hchans[MEDIUM],
	*hnames[MEDIUM],
//The escape codes really aren't necessary
//TODO: This needs to change.  That is too identifiable.  Has to match the username somehow, BUT NOT in a predictable pattern.
	**g_out=0,
	*g_version=VERSION,
	*towrite,
//maintain the lie
//TODO, there should be a weight on such things as v5.91 to make it come up more than say, Pocket IRC (which I had never heard of until I started ctcping random people)
	*versions[]={	"PIRCH98:WIN 95/98/WIN NT:1.0 (build 1.0.1.1190)",
			"mIRC32 v5.91 K.Mardam-Bey",
			"mIRC v6.02 Khaled Mardam-Bey",
			"mIRC v6.03 Khaled Mardam-Bey",
			"mIRC v6.12 Khaled Mardam-Bey",
			"mIRC v6.14 Khaled Mardam-Bey",
			"mIRC v6.16 Khaled Mardam-Bey",
			"Pocket IRC 1.1 http://pocketirc.com/"},
	*g_spoof;

void	*g_void;

#include"errors.h"
#include"buffer.c"
#include"cline/cline.c"
#include"cline/command.c"
#include"commands/commands.h"
#include"common.c"
#include"connection.c"
#include"getthings.c"
#include"hash.c"
#include"parse.c"
#include"qval.c"
#include"server/bouncer.c"
#include"server/server.c"
#include"settings.c"

#endif
