#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <stdio.h>

#ifndef ERR_OFF
#define ERR(...) fprintf(stderr, ##__VA_ARGS__)
#else
#define ERR(...) ;
#endif

#ifndef TRACE_OFF
#define TRACE(...) {ERR("(%s: %s, Line %d)",__FILE__,__FUNCTION__,__LINE__); fprintf(stderr, ##__VA_ARGS__) ; ERR("\n"); }
#else
#define TRACE(...) ;
#endif
#endif // DEBUG_HPP

