#include "clplatform.h"

#ifdef __WXMSW__
wxChar clPlatform::PathSeparator = ';';
wxChar clPlatform::DirSeparator = '\\';
#else
wxChar clPlatform::PathSeparator = ':';
wxChar clPlatform::DirSeparator = '/';
#endif

clPlatform::eOS clPlatform::GetOS()
{
#ifdef __WXMSW__
    return clPlatform::kWindows;

#elif defined(__WXGTK__)
    return clPlatform::kGtk;

#elif defined(__WXMAC__)
    return clPlatform::kMac;

#else
    return clPlatform::kUnknown;
#endif
}
