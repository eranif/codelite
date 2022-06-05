#ifndef CLMODULELOGGER_HPP
#define CLMODULELOGGER_HPP

#include "codelite_exports.h"
#include "file_logger.h"

#include <wx/colour.h>
#include <wx/gdicmn.h>

/// A small logger designed to be used by small subsystem of CodeLite
class clModuleLogger;
typedef clModuleLogger& (*clModuleLoggerFunction)(clModuleLogger&);

class WXDLLIMPEXP_CL clModuleLogger
{
    int m_current_log_level = FileLogger::Error;
    wxString m_buffer;
    wxFileName m_logfile;

public:
    clModuleLogger();
    ~clModuleLogger();

    /// used to check if an entry added using the current log level
    /// will eventually be logged (we check this against the global
    /// logger debug level)
    bool CanLog() const;

    clModuleLogger& SetCurrentLogLevel(int level);

    /**
     * @brief return prefix string suitable for the current time and log level
     */
    wxString Prefix();

    /**
     * @brief open log file
     */
    void Open(const wxFileName& filepath);

    inline clModuleLogger& operator<<(clModuleLoggerFunction f)
    {
        Flush();
        return *this;
    }

    // special types printing
    inline clModuleLogger& operator<<(const std::vector<wxString>& arr)
    {
        if(!CanLog()) {
            return *this;
        }

        if(!m_buffer.empty()) {
            m_buffer << " ";
        }

        m_buffer << "[";
        if(!arr.empty()) {
            for(size_t i = 0; i < arr.size(); ++i) {
                m_buffer << arr[i] << ", ";
            }
            m_buffer.RemoveLast(2);
        }
        m_buffer << "]";
        return *this;
    }

    inline clModuleLogger& operator<<(const wxStringSet_t& S)
    {
        if(!CanLog()) {
            return *this;
        }
        if(!m_buffer.empty()) {
            m_buffer << " ";
        }
        m_buffer << "{";
        if(!S.empty()) {
            for(const wxString& s : S) {
                m_buffer << s << ", ";
            }
            m_buffer.RemoveLast(2);
        }
        m_buffer << "}";
        return *this;
    }

    inline clModuleLogger& operator<<(const wxStringMap_t& M)
    {
        if(!CanLog()) {
            return *this;
        }
        if(!m_buffer.empty()) {
            m_buffer << " ";
        }
        m_buffer << "{";
        if(!M.empty()) {
            for(const auto& vt : M) {
                m_buffer << "{" << vt.first << ", " << vt.second << "}, ";
            }
            m_buffer.RemoveLast(2);
        }
        m_buffer << "}";
        return *this;
    }

    inline clModuleLogger& operator<<(const wxArrayString& arr)
    {
        if(!CanLog()) {
            return *this;
        }
        std::vector<wxString> v{ arr.begin(), arr.end() };
        *this << v;
        return *this;
    }

    inline clModuleLogger& operator<<(const wxColour& colour)
    {
        if(!CanLog()) {
            return *this;
        }

        *this << colour.GetAsString(wxC2S_HTML_SYNTAX);
        return *this;
    }

    inline clModuleLogger& operator<<(const wxPoint& point)
    {
        if(!CanLog()) {
            return *this;
        }

        wxString str;
        str << "{x:" << point.x << ", y:" << point.y << "}";
        *this << str;
        return *this;
    }

    inline clModuleLogger& operator<<(const wxSize& size)
    {
        if(!CanLog()) {
            return *this;
        }

        wxString str;
        str << "{w:" << size.GetWidth() << ", h:" << size.GetHeight() << "}";
        *this << str;
        return *this;
    }

    inline clModuleLogger& operator<<(const wxRect& rect)
    {
        if(!CanLog()) {
            return *this;
        }

        *this << "{" << rect.GetTopLeft() << "," << rect.GetSize() << "}";
        return *this;
    }

    /**
     * @brief special wxString printing
     * Without this overload operator, on some compilers, the "clDEBUG()<< wxString" might be "going" to the one
     * that handles wxFileName...
     */
    inline clModuleLogger& operator<<(const wxString& str)
    {
        if(!CanLog()) {
            return *this;
        }
        if(!m_buffer.empty()) {
            m_buffer << " ";
        }
        m_buffer << str;
        return *this;
    }
    /**
     * @brief handle char*
     */
    inline clModuleLogger& operator<<(const char* str)
    {
        if(!CanLog()) {
            return *this;
        }
        wxString s(str);
        return *this << s;
    }

    /**
     * @brief special wxFileName printing
     */
    inline clModuleLogger& operator<<(const wxFileName& fn)
    {
        if(!CanLog()) {
            return *this;
        }
        if(!m_buffer.empty()) {
            m_buffer << " ";
        }
        m_buffer << fn.GetFullPath();
        return *this;
    }

    /**
     * @brief append any type to the buffer, take log level into consideration
     */
    template <typename T> clModuleLogger& Append(const T& elem)
    {
        if(!m_buffer.empty()) {
            m_buffer << " ";
        }
        m_buffer << elem;
        return *this;
    }

    /**
     * @brief flush the logger content
     */
    void Flush();
};

inline clModuleLogger& endl(clModuleLogger& d)
{
    d.Flush();
    return d;
}

template <typename T> clModuleLogger& operator<<(clModuleLogger& logger, const T& obj)
{
    if(!logger.CanLog()) {
        return logger;
    }

    logger.Append(obj);
    return logger;
}

#define LOG_SYSTEM(LOG) LOG.SetCurrentLogLevel(FileLogger::System) << LOG.Prefix()
#define LOG_ERROR(LOG) LOG.SetCurrentLogLevel(FileLogger::Error) << LOG.Prefix()
#define LOG_WARNING(LOG) LOG.SetCurrentLogLevel(FileLogger::Warning) << LOG.Prefix()
#define LOG_DEBUG(LOG) LOG.SetCurrentLogLevel(FileLogger::Dbg) << LOG.Prefix()
#define LOG_TRACE(LOG) LOG.SetCurrentLogLevel(FileLogger::Developer) << LOG.Prefix()

#endif // CLMODULELOGGER_HPP
