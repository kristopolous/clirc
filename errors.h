#ifndef _ERRORS_
#define _ERRORS_
#include"defines.h"

struct errdata
{	char*message[SMALL];
	int times[SMALL];
	short n;
};

struct errdata g_msg[502];

#define RWELCOME	001
#define RYOURHOST	002
#define RCREATED	003
#define RMYINFO	004
#define RBOUNCE	005
#define RUSERHOST	302
#define RISON	303
#define RAWAY	301
#define RUNAWAY	305
#define RNOWAWAY	306
#define RWHOISUSER	311
#define RWHOISSERVER	312
#define RWHOISOPERATOR	313
#define RWHOISIDLE	317
#define RENDOFWHOIS	318
#define RWHOISCHANNELS	319
#define RWHOWASUSER	314
#define RENDOFWHOWAS	369
#define RLISTSTART	321
#define RLIST	322
#define RLISTEND	323
#define RUNIQOPIS	325
#define RCHANNELMODEIS	324
#define RNOTOPIC	331
#define RTOPIC	332
#define RINVITING	341
#define RSUMMONING	342
#define RINVITELIST	346
#define RENDOFINVITELIST	347
#define REXCEPTLIST	348
#define RENDOFEXCEPTLIST	349
#define RVERSION	351
#define RWHOREPLY	352
#define RENDOFWHO	315
#define RNAMREPLY	353
#define RENDOFNAMES	366
#define RLINKS	364
#define RENDOFLINKS	365
#define RBANLIST	367
#define RENDOFBANLIST	368
#define RINFO	371
#define RENDOFINFO	374
#define RMOTDSTART	375
#define RMOTD	372
#define RENDOFMOTD	376
#define RYOUREOPER	381
#define RREHASHING	382
#define RYOURESERVICE	383
#define RTIME	391
#define RUSERSSTART	392
#define RUSERS	393
#define RENDOFUSERS	394
#define RNOUSERS	395
#define RTRACELINK	200
#define RTRACECONNECTING	201
#define RTRACEHANDSHAKE	202
#define RTRACEUNKNOWN	203
#define RTRACEOPERATOR	204
#define RTRACEUSER	205
#define RTRACESERVER	206
#define RTRACESERVICE	207
#define RTRACENEWTYPE	208
#define RTRACECLASS	209
#define RTRACERECONNECT	210
#define RTRACELOG	261
#define RTRACEEND	262
#define RSTATSLINKINFO	211
#define RSTATSCOMMANDS	212
#define RENDOFSTATS	219
#define RSTATSUPTIME	242
#define RSTATSOLINE	243
#define RUMODEIS	221
#define RSERVLIST	234
#define RSERVLISTEND	235
#define RLUSERCLIENT	251
#define RLUSEROP	252
#define RLUSERUNKNOWN	253
#define RLUSERCHANNELS	254
#define RLUSERME	255
#define RADMINME	256
#define RADMINLOC1	257
#define RADMINLOC2	258
#define RADMINEMAIL	259
#define RTRYAGAIN	263
#define ENOSUCHNICK	401
#define ENOSUCHSERVER	402
#define ENOSUCHCHANNEL	403
#define ECANNOTSENDTOCHAN	404
#define ETOOMANYCHANNELS	405
#define EWASNOSUCHNICK	406
#define ETOOMANYTARGETS	407
#define ENOSUCHSERVICE	408
#define ENOORIGIN	409
#define ENORECIPIENT	411
#define ENOTEXTTOSEND	412
#define ENOTOPLEVEL	413
#define EWILDTOPLEVEL	414
#define EBADMASK	415
#define EUNKNOWNCOMMAND	421
#define ENOMOTD	422
#define ENOADMININFO	423
#define EFILEERROR	424
#define ENONICKNAMEGIVEN	431
#define EERRONEUSNICKNAME	432
#define ENICKNAMEINUSE	433
#define ENICKCOLLISION	436
#define EUNAVAILRESOURCE	437
#define EUSERNOTINCHANNEL	441
#define ENOTONCHANNEL	442
#define EUSERONCHANNEL	443
#define ENOLOGIN	444
#define ESUMMONDISABLED	445
#define EUSERSDISABLED	446
#define ENOTREGISTERED	451
#define ENEEDMOREPARAMS	461
#define EALREADYREGISTRED	462
#define ENOPERMFORHOST	463
#define EPASSWDMISMATCH	464
#define EYOUREBANNEDCREEP	465
#define EYOUWILLBEBANNED	466
#define EKEYSET	467
#define ECHANNELISFULL	471
#define EUNKNOWNMODE	472
#define EINVITEONLYCHAN	473
#define EBANNEDFROMCHAN	474
#define EBADCHANNELKEY	475
#define EBADCHANMASK	476
#define ENOCHANMODES	477
#define EBANLISTFULL	478
#define ENOPRIVILEGES	481
#define ECHANOPRIVSNEEDED	482
#define ECANTKILLSERVER	483
#define ERESTRICTED	484
#define EUNIQOPPRIVSNEEDED	485
#define ENOOPERHOST	491
#define EUMODEUNKNOWNFLAG	501
#define EUSERSDONTMATCH	502
#endif
