#ifndef PRECOMPILED_HEADER_H
#define PRECOMPILED_HEADER_H

// A proxy to the right PCH file
// we need to so we will get a different flags compiled per file
#ifdef NDEBUG
#    include "precompiled_header_release.h"
#else
#    include "precompiled_header_dbg.h"
#endif

#endif

