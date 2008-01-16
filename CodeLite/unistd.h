/*
 * This file is part of the Mingw32 package.
 *
 * unistd.h maps (roughly) to io.h
 */

#ifndef __STRICT_ANSI__
#ifdef __WXGTK__
#include "/usr/include/unistd.h"
#elif defined(__WXMAC__)
#include "/usr/include/unistd.h"
#else
#include <io.h>
#include <process.h>
#endif
#endif 
