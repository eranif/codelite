#ifdef __WXMSW__
#include <stdio.h>
#include <windows.h>
BOOL APIENTRY MyCreatePipe(OUT LPHANDLE lpReadPipe, OUT LPHANDLE lpWritePipe, IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
                           IN DWORD nSize);
#endif
