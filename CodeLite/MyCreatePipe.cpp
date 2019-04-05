/*++
    CreatePipe-like function that lets one or both handles be overlapped
Author:
    Dave Hart  Summer 1997
--*/
#ifdef __WXMSW__
#include "MyCreatePipe.h"

ULONG PipeSerialNumber = 0;

/*++
Routine Description:
    The MyCreatePipe API is used to create an anonymous pipe I/O device.
    Unlike CreatePipe FILE_FLAG_OVERLAPPED may be specified for one or
    both handles.
    Two handles to the device are created.  One handle is opened for
    reading and the other is opened for writing.  These handles may be
    used in subsequent calls to ReadFile and WriteFile to transmit data
    through the pipe.
Arguments:
    lpReadPipe - Returns a handle to the read side of the pipe.  Data
        may be read from the pipe by specifying this handle value in a
        subsequent call to ReadFile.
    lpWritePipe - Returns a handle to the write side of the pipe.  Data
        may be written to the pipe by specifying this handle value in a
        subsequent call to WriteFile.
    lpPipeAttributes - An optional parameter that may be used to specify
        the attributes of the new pipe.  If the parameter is not
        specified, then the pipe is created without a security
        descriptor, and the resulting handles are not inherited on
        process creation.  Otherwise, the optional security attributes
        are used on the pipe, and the inherit handles flag effects both
        pipe handles.
    nSize - Supplies the requested buffer size for the pipe.  This is
        only a suggestion and is used by the operating system to
        calculate an appropriate buffering mechanism.  A value of zero
        indicates that the system is to choose the default buffering
        scheme.
Return Value:
    TRUE - The operation was successful.
    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.
--*/
BOOL APIENTRY MyCreatePipe(OUT LPHANDLE lpReadPipe, OUT LPHANDLE lpWritePipe, IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
                           IN DWORD nSize)

{
    HANDLE ReadPipeHandle, WritePipeHandle;
    DWORD dwError;
    CHAR PipeNameBuffer[MAX_PATH];

    //
    // Only one valid OpenMode flag - FILE_FLAG_OVERLAPPED
    //
    DWORD dwReadMode = FILE_FLAG_OVERLAPPED;
    DWORD dwWriteMode = FILE_FLAG_OVERLAPPED;

    //
    //  Set the default timeout to 120 seconds
    //
    if(nSize == 0) { nSize = 4096; }

    // Increment the pipe number, in a MT safe way
    InterlockedIncrement(&PipeSerialNumber);
    sprintf(PipeNameBuffer, "\\\\.\\Pipe\\LOCAL\\RemoteExeAnon.%08x.%08x", (unsigned int)GetCurrentProcessId(),
            (unsigned int)PipeSerialNumber);

    ReadPipeHandle = CreateNamedPipeA(PipeNameBuffer, PIPE_ACCESS_INBOUND | dwReadMode, PIPE_TYPE_BYTE | PIPE_WAIT,
                                      1,          // Number of pipes
                                      nSize,      // Out buffer size
                                      nSize,      // In buffer size
                                      120 * 1000, // Timeout in ms
                                      lpPipeAttributes);

    if(!ReadPipeHandle) { return FALSE; }

    WritePipeHandle = CreateFileA(PipeNameBuffer, GENERIC_WRITE,
                                  0, // No sharing
                                  lpPipeAttributes, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | dwWriteMode,
                                  NULL // Template file
    );

    if(INVALID_HANDLE_VALUE == WritePipeHandle) {
        dwError = GetLastError();
        CloseHandle(ReadPipeHandle);
        SetLastError(dwError);
        return FALSE;
    }

    *lpReadPipe = ReadPipeHandle;
    *lpWritePipe = WritePipeHandle;
    return (TRUE);
}
#endif // __WXMSW__
