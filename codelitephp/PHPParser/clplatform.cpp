#include "clplatform.h"

#ifdef __WXMSW__
const wxChar clPlatform::PathSeparator = ';';
#else
const wxChar clPlatform::PathSeparator = ':';
#endif
