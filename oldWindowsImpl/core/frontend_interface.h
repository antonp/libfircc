#ifndef _FRONTEND_INTERFACE_H_
#define _FRONTEND_INTERFACE_H_

#define FIRC_COREAPI __stdcall
#define FIRC_CALLBACK __stdcall

//typedef void (FIRC_COREAPI *OUTPUTHANDLER)(const char *, int);
typedef void (FIRC_COREAPI *EVENTHANDLER)(int, int, int, const char *, const char *, const char *);

extern "C" {
//void FIRC_COREAPI firc_setOutputHandler(OUTPUTHANDLER o);
void FIRC_COREAPI firc_setEventHandler(EVENTHANDLER e);
void FIRC_COREAPI firc_processInput(char *s);

void FIRC_COREAPI firc_init();
void FIRC_COREAPI firc_exit();

int FIRC_COREAPI firc_getAgentCount();
void FIRC_COREAPI firc_getAgentLabel(char *dest, int maxsize, int id);
}

#endif // _FRONTEND_INTERFACE_H_