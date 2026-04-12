#ifndef UIHANGDETECTOR_HPP
#define UIHANGDETECTOR_HPP

// clang-format off
#include <wx/app.h>
// clang-format on

#include <atomic>
#include <string>
#include <thread>

#ifdef __WXMSW__
#include <windows.h>
#endif

#include "codelite_exports.h"

class WXDLLIMPEXP_CL UIHangDetector
{
public:
    UIHangDetector();
    ~UIHangDetector();

    /**
     * Start the watchdog thread
     * @param hangThresholdMs Time in milliseconds before considering the main thread hung
     * @param checkIntervalMs How often the watchdog wakes up to check (should be < hangThresholdMs)
     */
    void Start(long hangThresholdMs = 50, long checkIntervalMs = 10);

    /**
     * Stop the watchdog thread
     */
    void Stop();

    /**
     * Check if the detector is running
     */
    bool IsRunning() const { return m_running.load(); }

private:
    void WatchdogLoop();
    static void CaptureMainThreadStack(HANDLE threadHandle, DWORD threadId);
    static std::string GetThreadStackTrace(HANDLE threadHandle, DWORD threadId);
    static uint64_t GetCurrentTimeMs();

    std::atomic<bool> m_running{false};
    std::thread m_watchdogThread;

#ifdef __WXMSW__
    HANDLE m_mainThreadHandle{nullptr};
    DWORD m_mainThreadId{0};
#endif

    long m_hangThresholdMs{50};
    long m_checkIntervalMs{10};
    std::atomic<uint64_t> m_lastHeartbeat{0};
};

#endif // UIHANGDETECTOR_HPP
