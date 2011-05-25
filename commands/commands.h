#ifndef _COMMANDS_
#define _COMMANDS_

int chat(void*channel);
int join(void*channel);
int part(void*channel);
int affinity(void*var);
int list(void*nothing);
int chanz(void*nothing);
int clear(void*nothing);
int print(void*nothing);
int ircdisconnect(void*null);
int ircconnect(void*null);
int set(void*var);
int show(void*var);
int raw(void*var);
int showmesg(void*nothing);
void whois(void*var);
int ircsleep(void*var);
int load(void*var);
int setnick(void*nothing);
int openlog(void*nothing);
int script(void*nothing);
int script_print(void*nothing) ;
int populate(void*nothing);
void ircreg(void);

#include "standard.c"
#include "channel.c"
#include "servercoms.c"
#include "script.c"
#endif
