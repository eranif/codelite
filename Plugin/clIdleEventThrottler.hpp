#pragma once

#include <chrono>

class clIdleEventThrottler
{
public:
    clIdleEventThrottler(uint64_t interval_ms)
        : m_interval_ms(interval_ms)
    {
    }

    bool CanHandle()
    {
        uint64_t current_ts =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        if ((current_ts - m_last_idle_event) < m_interval_ms) {
            return false;
        }
        m_last_idle_event = current_ts;
        return true;
    }

private:
    uint64_t m_last_idle_event = 0;
    uint64_t m_interval_ms = 0;
};
