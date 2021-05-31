#ifdef __WXMSW__
#include "wx/msw/wrapwin.h" // includes windows.h
#include <stdio.h>
BOOL APIENTRY MyCreatePipe(OUT LPHANDLE lpReadPipe, OUT LPHANDLE lpWritePipe, IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
                           IN DWORD nSize);
#endif
