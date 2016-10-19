/*
   3APA3A simpliest proxy server
   (c) 2002-2008 by ZARAZA <3APA3A@security.nnov.ru>

   please read License Agreement

   $Id: tcppm.c,v 1.13 2008/01/08 21:46:39 vlad Exp $
*/

#include "proxy.h"

#ifndef PORTMAP
#define PORTMAP
#endif
#define RETURN(xxx) { param->res = xxx; goto CLEANRET; }

void * tcppmchild(struct clientparam* param) {
 int res;

 parsehostname((char *)param->srv->target, param, ntohs(param->srv->targetport));
 param->operation = CONNECT;
 res = (*param->srv->authfunc)(param);
 if(res) {RETURN(res);}
 RETURN (sockmap(param, conf.timeouts[CONNECTION_L]));
CLEANRET:
 
 (*param->srv->logfunc)(param, NULL);
 freeparam(param);
 return (NULL);
}

#ifdef WITHMAIN
struct proxydef childdef = {
	tcppmchild,
	0,
	0,
	S_TCPPM,
	""
};
#include "proxymain.c"
#endif
