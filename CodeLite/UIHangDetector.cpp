#include "UIHangDetector.hpp"

#include "BlockTimer.hpp"
#include "clModuleLogger.hpp"
#include "event_notifier.h"
#include "file_logger.h"

#include <chrono>
#include <sstream>

#ifdef __WXMSW__
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

INITIALISE_MODULE_LOG(HANG_LOG, "HangDetector", "hang-detector.log");
#define HLOG() HANG_LOG().SetCurrentLogLevel(FileLogger::System) << HANG_LOG().Prefix()
#define CHECK_ENABLED()             \
    if (!BlockTimer::IsEnabled()) { \
        return;                     \
    }

UIHangDetector::UIHangDetector()
{
    CHECK_ENABLED()
#ifdef __WXMSW__
    // Get the main thread handle (current thread when constructed)
    m_mainThreadId = GetCurrentThreadId();

    // DuplicateHandle to get a real handle we can use from another thread
    HANDLE pseudoHandle = GetCurrentThread();
    if (!DuplicateHandle(GetCurrentProcess(),
                         pseudoHandle,
                         GetCurrentProcess(),
                         &m_mainThreadHandle,
                         0,
                         FALSE,
                         DUPLICATE_SAME_ACCESS)) {
        HLOG() << "Failed to duplicate main thread handle: " << GetLastError() << endl;
        m_mainThreadHandle = nullptr;
    }
#endif
}

UIHangDetector::~UIHangDetector()
{
    CHECK_ENABLED()
    Stop();

#ifdef __WXMSW__
    if (m_mainThreadHandle) {
        CloseHandle(m_mainThreadHandle);
        m_mainThreadHandle = nullptr;
    }
#endif
}

void UIHangDetector::Start(long hangThresholdMs, long checkIntervalMs)
{
    CHECK_ENABLED()

    if (checkIntervalMs <= 0) {
        HLOG() << "Invalid check interval provided:" << checkIntervalMs << ". Setting it default of 50ms" << endl;
        checkIntervalMs = 50;
    }

    if (hangThresholdMs < 250) {
        HLOG() << "Invalid check interval provided:" << hangThresholdMs << ". Setting it default of 250ms" << endl;
        hangThresholdMs = 250;
    }

    if (m_running.load()) {
        return; // Already running
    }

    m_hangThresholdMs = hangThresholdMs;
    m_checkIntervalMs = checkIntervalMs;

    m_running.store(true);
    m_watchdogThread = std::thread(&UIHangDetector::WatchdogLoop, this);

    HLOG() << "UIHangDetector started (threshold=" << hangThresholdMs << "ms, check_interval=" << checkIntervalMs
           << "ms)" << endl;
}

void UIHangDetector::Stop()
{
    CHECK_ENABLED()
    if (!BlockTimer::IsEnabled()) {
        return;
    }

    if (!m_running.load()) {
        return;
    }

    m_running.store(false);

    if (m_watchdogThread.joinable()) {
        m_watchdogThread.join();
    }

    HLOG() << "UIHangDetector stopped" << endl;
}

uint64_t UIHangDetector::GetCurrentTimeMs()
{
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return static_cast<uint64_t>(duration.count());
}

void UIHangDetector::WatchdogLoop()
{
#ifdef __WXMSW__
    CHECK_ENABLED()
    size_t idle_loops = 10000 / m_checkIntervalMs;

    // Wait for 10 seconds before starting measure things.
    while (idle_loops > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_checkIntervalMs));
        idle_loops--;
        if (!m_running.load()) {
            // Going down
            break;
        }
    }

    HLOG() << "Monitoring starts now" << endl;

    // 2. Start a SINGLE long-lived Monitor Thread
    std::thread monitor([this]() {
        while (m_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(m_checkIntervalMs));

            uint64_t now = GetCurrentTimeMs();
            uint64_t last = m_lastHeartbeat.load();

            uint64_t timeSinceLastActivity = now - last;
            if (last != 0 && (timeSinceLastActivity > static_cast<uint64_t>(m_hangThresholdMs))) {
                HLOG() << "=== UI HANG DETECTED ===" << endl;
                HLOG() << "Main Thread was not responsive for over" << timeSinceLastActivity << "ms" << endl;
                HLOG() << "=== (Live Capture) ===" << endl;
                MSWCaptureMainThreadStack(m_mainThreadHandle, m_mainThreadId);

                // To avoid flooding logs during a permanent hang,
                // you might want to sleep longer here or set a flag.
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }
    });

    // 3. Main Watchdog loop: just sends the heartbeat
    while (m_running.load()) {
        // Send heartbeat to main thread
        EventNotifier::Get()->RunOnMain<int>([this]() {
            m_lastHeartbeat.store(GetCurrentTimeMs());
            return 0;
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(m_checkIntervalMs));
    }

    if (monitor.joinable())
        monitor.join();
#endif
}

#ifdef __WXMSW__
void UIHangDetector::MSWCaptureMainThreadStack(HANDLE threadHandle, DWORD threadId)
{
    if (!threadHandle) {
        HLOG() << "No main thread handle available" << endl;
        return;
    }

    std::string stackTrace = MSWGetThreadStackTrace(threadHandle, threadId);

    HLOG() << "=== MAIN THREAD STACK TRACE ===" << endl << stackTrace << "================================" << endl;
}

std::string UIHangDetector::MSWGetThreadStackTrace(HANDLE threadHandle, DWORD threadId)
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
