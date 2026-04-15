#include "StackWalker.hpp"

#include "file_logger.h"

#include <sstream>

// clang-format off
#include <wx/app.h>
#include <wx/stackwalk.h>
// clang-format on

#ifdef __WXMSW__
#include <dbghelp.h>
#include <windows.h>
#pragma comment(lib, "dbghelp.lib")
static HANDLE g_mainThreadHandle{nullptr};
static DWORD g_mainThreadId{0};
#endif

class MyStackWalker : public wxStackWalker
{
public:
    MyStackWalker(wxString& output)
        : m_output{output}
    {
    }
    void OnStackFrame(const wxStackFrame& frame) override
    {
        m_output << frame.GetModule() << wxT(" ") << frame.GetName() << wxT(" ") << frame.GetFileName() << wxT(":")
                 << frame.GetLine() << wxT("\n");
    }

private:
    wxString& m_output;
};

void StackWalker::Shutdown()
{
#ifdef __WXMSW__
    if (g_mainThreadHandle) {
        CloseHandle(g_mainThreadHandle);
        g_mainThreadHandle = nullptr;
        g_mainThreadId = 0;
    }
#endif
}

void StackWalker::Initialise()
{
#ifdef __WXMSW__
    // Get the main thread handle (current thread when constructed)
    g_mainThreadId = GetCurrentThreadId();

    // DuplicateHandle to get a real handle we can use from another thread
    HANDLE pseudoHandle = GetCurrentThread();
    if (!DuplicateHandle(GetCurrentProcess(),
                         pseudoHandle,
                         GetCurrentProcess(),
                         &g_mainThreadHandle,
                         0,
                         FALSE,
                         DUPLICATE_SAME_ACCESS)) {
        clWARNING() << "Failed to duplicate main thread handle: " << GetLastError() << endl;
        g_mainThreadHandle = nullptr;
    }
#endif
}

#ifdef __WXMSW__
std::string MSWGetStackTrace(HANDLE threadHandle, DWORD threadId)
{
    std::stringstream ss;

#ifndef _M_X64
    ss << "Stack trace is only supported on x86_64 Windows" << std::endl;
    return ss.str();
#else
    // Suspend the target thread
    DWORD suspendCount = SuspendThread(threadHandle);
    if (suspendCount == (DWORD)-1) {
        ss << "Failed to suspend thread: " << GetLastError() << std::endl;
        return ss.str();
    }

    // Initialize symbol handler for the current process
    HANDLE process = GetCurrentProcess();

    // Set symbol options
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    static bool symInitialized = false;
    if (!symInitialized) {
        if (!SymInitialize(process, NULL, TRUE)) {
            ss << "Failed to initialize symbols (error: " << GetLastError() << ")" << std::endl;
            ResumeThread(threadHandle);
            return ss.str();
        }
        symInitialized = true;
    }

    // Get the thread context
    CONTEXT context;
    memset(&context, 0, sizeof(CONTEXT));
    context.ContextFlags = CONTEXT_FULL;

    if (!GetThreadContext(threadHandle, &context)) {
        ss << "Failed to get thread context: " << GetLastError() << std::endl;
        ResumeThread(threadHandle);
        return ss.str();
    }

    // Setup stack frame for walking
    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));

    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Mode = AddrModeFlat;

    // Walk the stack
    int frameNum = 0;
    while (StackWalk64(machineType,
                       process,
                       threadHandle,
                       &stackFrame,
                       &context,
                       NULL,
                       SymFunctionTableAccess64,
                       SymGetModuleBase64,
                       NULL)) {

        if (stackFrame.AddrPC.Offset == 0) {
            break; // End of stack
        }

        DWORD64 address = stackFrame.AddrPC.Offset;

        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        ss << "#" << frameNum++ << " 0x" << std::hex << address << std::dec << " ";

        DWORD64 displacement = 0;
        if (SymFromAddr(process, address, &displacement, symbol)) {
            ss << symbol->Name;
            if (displacement != 0) {
                ss << " + 0x" << std::hex << displacement << std::dec;
            }

            // Try to get line information
            IMAGEHLP_LINE64 line;
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD lineDisplacement = 0;
            if (SymGetLineFromAddr64(process, address, &lineDisplacement, &line)) {
                ss << " at " << line.FileName << ":" << line.LineNumber;
            }
        } else {
            ss << "<unknown>";
        }

        ss << std::endl;

        // Limit depth to avoid very long traces
        if (frameNum > 50) {
            ss << "... (truncated)" << std::endl;
            break;
        }
    }

    // Resume the thread
    ResumeThread(threadHandle);

#endif // _M_X64
    return ss.str();
}
#endif

void StackWalker::Dump(const std::vector<wxString>& prefix, bool dumpCurrentThread)
{
    wxString text = "\n";
    for (const wxString& header : prefix) {
        text << header << "\n";
    }
#ifdef __WXMSW__
    text << "=== MAIN THREAD BACKTRACE ===" << "\n";
    text << "\n";
    wxString trace = wxString::FromUTF8(MSWGetStackTrace(g_mainThreadHandle, g_mainThreadId));
    text << trace << "\n";
    text << "\n";

    if (dumpCurrentThread) {
        // Get the main thread handle (current thread when constructed)
        auto currThreadId = GetCurrentThreadId();
        HANDLE currThreadHandle{INVALID_HANDLE_VALUE};

        // DuplicateHandle to get a real handle we can use from another thread
        HANDLE pseudoHandle = GetCurrentThread();
        if (!DuplicateHandle(GetCurrentProcess(),
                             pseudoHandle,
                             GetCurrentProcess(),
                             &currThreadHandle,
                             0,
                             FALSE,
                             DUPLICATE_SAME_ACCESS)) {
            clWARNING() << "Failed to duplicate main thread handle: " << GetLastError() << endl;
            currThreadHandle = nullptr;
            return;
        }
        trace = wxString::FromUTF8(MSWGetStackTrace(currThreadHandle, currThreadId));
        text << "\n=== CURRENT THREAD BACKTRACE ===\n\n";
        text << trace << "\n\n";

        if (currThreadHandle) {
            CloseHandle(currThreadHandle);
            currThreadHandle = nullptr;
            currThreadId = 0;
        }
    }
    clERROR() << text << "\n";
#else
    MyStackWalker walker{text};
    walker.Walk();
    clERROR() << text << "\n";
#endif
}
