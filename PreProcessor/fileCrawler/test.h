#include <vector>
using  namespace   boost::filesystem;
#include <list>
using     namespace std;
#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>

namespace bf = boost::filesystem ;

#define A

#ifdef  A
	#define B(x, y) {    \
    if(x < y){\
			printf(x);   \
    }else{\
			printf(y);   \
    }
#endif

#ifdef C
	// hop
	#ifndef D
		// rem
	#endif
#endif
    
#if E
	// 
#endif

#if defined(F)
	//
#endif

#if defined(G) && defined(H)
	//
#endif

#if I=1 && J=7
	//
#endif

#if defined(K) || !defined(L)
	//
#endif

#if defined(M) && N=7
	//
#elif !defined(O)
	//
#endif
