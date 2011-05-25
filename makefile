CC=cc -g3 -gdwarf-2 -Wall -D_DEBUG
clirc: defines.h buffer.o cline/cline.o cline/command.o commands/channel.o commands/script.o commands/servercoms.o commands/standard.o common.o connection.o getthings.o hash.o irc.o parse.o qval.o settings.o server/bouncer.o server/server.o
	$(CC) $(LDFLAGS) -o clirc irc.c
clean:
	rm -f *.o */*.o core clirc
