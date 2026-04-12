#include "BlockTimer.hpp"

#include "clModuleLogger.hpp"
#include "file_logger.h"

#include <vector>
#ifdef __WXMSW__
// clang-format off
#include <windows.h>
#include <dbghelp.h>
#include <iostream>
// clang-format on
#endif
#include <wx/wxcrt.h>

INITIALISE_MODULE_LOG(LOG, "Perf", "perf.log");
INITIALISE_MODULE_LOG(SLOW_LOG, "Dump", "slow-dump.log");

static thread_local std::vector<BlockTimer*> g_timers_stack;
static bool g_enabled{false};

namespace
{
#ifdef __WXMSW__
std::string DumpCurrentThreadStack()
{
    std::stringstream ss;
#ifndef _M_X64
    // Only supported on x86_64 Windows
    ss << "Stack trace is only supported on x86_64 Windows (not ARM64 or x86)" << std::endl;
#else
    // x86_64 implementation

    ss << "=== Stack Trace ===" << std::endl;

    // Initialize symbol handler for the current process
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    // Set symbol options
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    if (!SymInitialize(process, NULL, TRUE)) {
        ss << "Failed to initialize symbols (error: " << GetLastError() << ")" << std::endl;
        return ss.str();
    }

    // Setup stack frame for walking
    CONTEXT context;
    RtlCaptureContext(&context);

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));

    DWORD machineType;
    machineType = IMAGE_FILE_MACHINE_AMD64;
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
                       thread,
                       &stackFrame,
                       &context,
                       NULL,
                       SymFunctionTableAccess64,
                       SymGetModuleBase64,
                       NULL)) {

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
    }

    ss << "===================" << std::endl;

    // Cleanup
    SymCleanup(process);
#endif // _M_X64
    return ss.str();
}
#else
std::string DumpCurrentThreadStack() { return {}; }
#endif
} // namespace

SlowdownTracker::SlowdownTracker(const char* label, long dump_backtrace_threshold_ms)
    : m_label{label}
    , m_dump_backtrace_threshold_ms{dump_backtrace_threshold_ms}
{
    m_sw.Start();
}

SlowdownTracker::~SlowdownTracker()
{
    m_sw.Pause();
    if (m_sw.Time() >= m_dump_backtrace_threshold_ms) {
        // Dump backtrace
        wxString label = m_label == nullptr ? wxString{} : wxString{m_label};
        wxString bt = wxString::FromUTF8(DumpCurrentThreadStack());
        SLOW_LOG().SetCurrentLogLevel(FileLogger::System)
            << SLOW_LOG().Prefix() << label << "Slowdown detected! Dumping backtrace:\n"
            << bt << "\n"
            << endl;
    }
}

// ==----------------------------
// ==----------------------------
BlockTimer::BlockTimer(const char* label)
    : m_label{label}
{
    if (g_enabled) [[unlikely]] {
        m_sw.Start();
        g_timers_stack.push_back(this);
    }
}

BlockTimer::~BlockTimer() { DoReport(); }

std::vector<BlockTimer::Content> BlockTimer::BuildContent() const
{
    std::vector<BlockTimer::Content> result;
    result.reserve(1 + m_childrenContent.size());

    size_t depth = g_timers_stack.size() - 1;
    result.push_back(Content{
        .duration = wxString::Format(wxT("%6u"), m_sw.TimeInMicro()),
        .message = m_label,
        .level = depth,
    });
    result.insert(result.end(), m_childrenContent.begin(), m_childrenContent.end());
    return result;
}

void BlockTimer::Enable() { g_enabled = true; }
bool BlockTimer::IsEnabled() { return g_enabled; }

void BlockTimer::Finish() { DoReport(); }

BlockTimer* BlockTimer::GetParent() const
{
    if (g_timers_stack.size() >= 2) {
        return g_timers_stack[g_timers_stack.size() - 2];
    }
    return nullptr;
}

void BlockTimer::DoReport()
{
    if (g_enabled && m_active) [[unlikely]] {
        auto parent = GetParent();
        m_sw.Pause();
        auto content = BuildContent();
        if (parent) [[unlikely]] {
            parent->m_childrenContent.reserve(parent->m_childrenContent.size() + content.size());
            parent->m_childrenContent.insert(parent->m_childrenContent.end(), content.begin(), content.end());
        } else {
            for (const auto& child_content : content) {
                LOG().SetCurrentLogLevel(FileLogger::System) << LOG().Prefix() << child_content.to_string() << endl;
            }
        }
        g_timers_stack.pop_back();
    }
    m_active = false;
}
