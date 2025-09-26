#pragma once

#include "codelite_exports.h"

#include <any>
#include <atomic>
#include <limits>

namespace llm
{
class WXDLLIMPEXP_SDK CancellationToken
{
public:
    CancellationToken(size_t max_tokens = std::numeric_limits<size_t>::max());
    virtual ~CancellationToken();

    CancellationToken(const CancellationToken&) = delete;
    CancellationToken& operator=(const CancellationToken&) = delete;

    /// Increment the number of tokens by 1. If the increment process crosses
    /// the max allowed, return false.
    inline bool Incr()
    {
        size_t old_value = m_tokens_count.fetch_add(1);
        return old_value < m_max_tokens;
    }

    inline size_t GetTokenCount() const { return m_tokens_count.load(); }
    inline void Cancel() { m_cancelled.store(true); }
    inline bool IsCancelled() { return m_cancelled.load(); }
    inline bool IsMaxTokenReached() const { return m_tokens_count.load() > m_max_tokens; }

    void Reset()
    {
        m_tokens_count.store(0);
        m_cancelled.store(false);
    }

private:
    std::atomic_size_t m_tokens_count{0};
    const size_t m_max_tokens{0};
    std::atomic_bool m_cancelled{false};
};

} // namespace llm