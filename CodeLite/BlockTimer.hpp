#ifdef BLOCKTIMER_HPP_INCLUDED
#error "BlockTimer.hpp must be included only once per translation unit"
#else
#define BLOCKTIMER_HPP_INCLUDED
#endif

#ifdef clENABLE_PERF
#undef clENABLE_PERF
#endif

// Change to 0 to globally disable timing
#define clENABLE_PERF 0
#include "StringUtils.h"

#include <sstream>
#include <string>
#include <wx/arrstr.h>
#include <wx/stopwatch.h>

// Disable performance statement, unconditionally.
#define __PERF_DISABLE(Statment)

#if clENABLE_PERF
#define __PERF_IF_ENABLED(Statement) Statement;
#else
#define __PERF_IF_ENABLED(Statement)
#endif

class WXDLLIMPEXP_CL BlockTimer final
{
public:
    explicit BlockTimer(const char* label);
    ~BlockTimer();

    template <typename T>
    BlockTimer& operator<<(const T& o)
    {
        m_buffer << o;
        return *this;
    }

    BlockTimer& operator<<(const wxString& o)
    {
        m_buffer << o.ToStdString(wxConvUTF8);
        return *this;
    }

    BlockTimer& operator<<(const wxArrayString& arr)
    {
        m_buffer << StringUtils::Join(arr, ",");
        return *this;
    }

    void Finish();
    static void Enable();

    struct Content {
        wxString duration;
        wxString message;
        size_t level{0};

        inline wxString to_string() const
        {
            wxString as_str;
            as_str.reserve(256);
            as_str << duration << wxT("µs ") << wxT(" [") << level << wxT("] ");
            if (level == 0) {
                as_str << wxT("◢ ") << message;
            } else {
                wxString indent{wxT(' '), 2 * level};
                as_str << indent << wxT(" ∟") << message;
            }
            return as_str;
        }
    };

private:
    BlockTimer* GetParent() const;
    std::vector<BlockTimer::Content> BuildContent() const;

    /**
     * Reports the timing results for the current block.
     *
     * If timing is enabled and the block is active, it builds the content string and
     * either adds it to a parent timer's content list or logs the results and
     * any children's content directly to the system log.
     *
     * @return void
     */
    void DoReport();

    wxStopWatch m_sw;
    bool m_active{true};
    std::stringstream m_buffer;
    std::string m_label;
    std::vector<BlockTimer::Content> m_childrenContent;
};
