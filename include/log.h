#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdbool.h>

#define PROTECT

#ifndef LOG_CPP
extern FILE *LOG_FILE;
#endif

#define LOG(...) fprintf(LOG_FILE, __VA_ARGS__)
#define LOGS(string) fputs(string, LOG_FILE)

#ifdef PROTECT
#define ASSERT(condition, action) if(!(condition))\
                                  {\
                                      LOG ("%s:%s:%d: Assertion cathced at ASSERT(", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                                      LOGS(#condition);\
                                      LOG (", ...);\n");\
                                      \
                                      action;\
                                  }
#else
#define ASSERT(condition, action)
#endif

#define EXEC_ASSERT(condition, action) {bool __temp__ = (condition); ASSERT(__temp__, action);}

FILE *OpenLog(void);

void CloseLog(void);

#endif //LOG_H