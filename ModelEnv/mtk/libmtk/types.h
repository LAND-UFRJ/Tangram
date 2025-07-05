#ifndef TYPES_H
#define TYPES_H

#include <list>
#include <string>

#define MTK_VERSION_STRING "MTK - Modelling ToolKit - version 0.5a\n" \
                           "Compiled on " __DATE__ " at " __TIME__ "\n" \
                           "Compiler Version: " __VERSION__

typedef std::string     Name;
typedef std::list<Name> NameList;

#ifdef DEBUG_LEVEL
#define LEVEL_ERROR   0
#define LEVEL_WARNING 1
#define LEVEL_INFO    2
#define DEBUG(LEVEL,...) if(LEVEL < DEBUG_LEVEL) \
                         { \
                             fprintf(stderr, "%s - %s: ", #LEVEL, __func__);\
                             fprintf(stderr,__VA_ARGS__); \
                         }
#else

#ifdef DEBUG
#undef DEBUG
#endif

#define DEBUG(LEVEL,...)

#endif

#endif /* TYPES_H */
