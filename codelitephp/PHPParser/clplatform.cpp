#include "clplatform.h"

#ifdef __WXMSW__
wxChar clPlatform::PathSeparator = wxT(';');
#else
wxChar clPlatform::PathSeparator = wxT(':');
#endif

bool clPlatform::OS()
{
#ifdef __WXMSW__
    return clPlatform::Windows;

#elif defined(__WXGTK__)
    return clPlatform::Gtk;

#elif defined(__WXMAC__)
    return clPlatform::Mac;

#else
    return clPlatform::Unknown;
#endif

}
