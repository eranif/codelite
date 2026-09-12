#pragma once

#include <atomic>
#include <chrono>
#include <functional>

class CallThrottler
{
public:
    explicit CallThrottler(std::chrono::milliseconds minInterval = std::chrono::milliseconds(50))
        : m_minInterval(minInterval)
    {
        m_lastExecution = std::chrono::steady_clock::now();
    }

    /**
     * @brief Executes the provided function if the minimum interval has passed since the last execution.
     *
     * This function checks whether the time elapsed since the last execution is greater than or equal
     * to the minimum required interval. If so, it executes the given function and updates the last
     * execution timestamp.
     *
     * @param func The function to execute if the interval condition is met
     */
    void ExecuteIfAllowed(std::function<void()> func)
    {
        auto now = std::chrono::steady_clock::now();
        auto last = m_lastExecution;

        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count() >= m_minInterval.count()) {
            func();
            m_lastExecution = now;
        }
    }

private:
    std::chrono::steady_clock::time_point m_lastExecution;
    std::chrono::milliseconds m_minInterval;
};
