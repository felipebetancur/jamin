#ifndef DEBUG_H
#define DEBUG_H

extern int debug_level;			/* current debugging level... */
#define DBG_OFF		0
#define DBG_TERSE	1
#define DBG_NORMAL	2
#define DBG_VERBOSE	3

#define IF_DEBUG(lvl,statements) if (debug_level>=(lvl)) statements;

#endif
