#include "BlockTimer.hpp"

#include "clModuleLogger.hpp"
#include "file_logger.h"

#include <vector>
#include <wx/wxcrt.h>

INITIALISE_MODULE_LOG(LOG, "Perf", "perf.log");

static thread_local std::vector<BlockTimer*> g_timers_stack;
static bool g_enabled{false};

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
