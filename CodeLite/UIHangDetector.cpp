#include "UIHangDetector.hpp"

#include "BlockTimer.hpp"
#include "StackWalker/StackWalker.hpp"
#include "event_notifier.h"
#include "file_logger.h"

#include <chrono>

#ifdef __WXMSW__
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

#define CHECK_ENABLED()             \
    if (!BlockTimer::IsEnabled()) { \
        return;                     \
    }

UIHangDetector::UIHangDetector() {}

UIHangDetector::~UIHangDetector()
{
    CHECK_ENABLED()
    Stop();
}

void UIHangDetector::Start(long hangThresholdMs, long checkIntervalMs)
{
    CHECK_ENABLED()

    if (checkIntervalMs <= 0) {
        clSYSTEM() << "Invalid check interval provided:" << checkIntervalMs << ". Setting it default of 50ms" << endl;
        checkIntervalMs = 50;
    }

    if (hangThresholdMs < 250) {
        clSYSTEM() << "Invalid check interval provided:" << hangThresholdMs << ". Setting it default of 250ms" << endl;
        hangThresholdMs = 250;
    }

    if (m_running.load()) {
        return; // Already running
    }

    m_hangThresholdMs = hangThresholdMs;
    m_checkIntervalMs = checkIntervalMs;

    m_running.store(true);
    m_watchdogThread = std::thread(&UIHangDetector::WatchdogLoop, this);

    clSYSTEM() << "UIHangDetector started (threshold=" << hangThresholdMs << "ms, check_interval=" << checkIntervalMs
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

    clSYSTEM() << "UIHangDetector stopped" << endl;
}

uint64_t UIHangDetector::GetCurrentTimeMs()
{
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return static_cast<uint64_t>(duration.count());
}

void UIHangDetector::WatchdogLoop()
{
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

    clSYSTEM() << "Monitoring starts now" << endl;

    // 2. Start a SINGLE long-lived Monitor Thread
    std::thread monitor([this]() {
        while (m_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(m_checkIntervalMs));

            uint64_t now = GetCurrentTimeMs();
            uint64_t last = m_lastHeartbeat.load();

            uint64_t timeSinceLastActivity = now - last;
            if (last != 0 && (timeSinceLastActivity > static_cast<uint64_t>(m_hangThresholdMs))) {
                wxString header;
                header << "Main Thread was not responsive for over" << timeSinceLastActivity << "ms";
                StackWalker::Dump({"=== UI HANG DETECTED ===", header, "=== (Live Capture) ==="}, false);

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

    if (monitor.joinable()) {
        monitor.join();
    }
}
