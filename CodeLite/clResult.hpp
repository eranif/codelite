#pragma once

#include "StringUtils.h" // wxStringView

#include <wx/string.h>

enum class StatusCode {
    kSuccess,
    kNotFound,
    kInvalidArgument,
    kOther,
    kPermissionDenied,
    kIOError,
    kNetError,
    kNeedMoreData,
    kProtocolError,
    kAlreadyExists,
    kResourceBusy,
    kParseError,
    kExpandError,
    kTimeout,
};

class clStatus
{
public:
    clStatus(StatusCode code)
        : m_code(code)
    {
    }
    clStatus() = default;
    ~clStatus() = default;

    bool ok() const { return m_code == StatusCode::kSuccess; }

    wxString message() const
    {
        wxString msg;
        switch (m_code) {
        case StatusCode::kExpandError:
            msg = "Expand error";
            break;
        case StatusCode::kParseError:
            msg = "Parsing error.";
            break;
        case StatusCode::kResourceBusy:
            msg = "Resource is busy";
            break;
        case StatusCode::kTimeout:
            msg = "Timeout occured";
            break;
        case StatusCode::kInvalidArgument:
            msg = "Invalid argument";
            break;
        case StatusCode::kSuccess:
            msg = "Success";
            break;
        case StatusCode::kNotFound:
            msg = "Not found";
            break;
        case StatusCode::kOther:
            msg = "Other error";
            break;
        case StatusCode::kPermissionDenied:
            msg = "Permission denied";
            break;
        case StatusCode::kIOError:
            msg = "I/O error";
            break;
        case StatusCode::kNetError:
            msg = "Network error";
            break;
        case StatusCode::kNeedMoreData:
            msg = "Need more data";
            break;
        case StatusCode::kProtocolError:
            msg = "Protocol error";
            break;
        case StatusCode::kAlreadyExists:
            msg = "Already exists";
            break;
        }

        if (!m_message.empty()) {
            msg << ". " << m_message;
        }
        return msg;
    }

    static clStatus MakeStatus(StatusCode code, const wxString& message = {})
    {
        clStatus st(code);
        st.m_message = message;
        return st;
    }

    StatusCode code() const { return m_code; }

private:
    StatusCode m_code{StatusCode::kSuccess};
    wxString m_message;
};

inline bool StatusIsInvalidArgument(const clStatus& st) { return st.code() == StatusCode::kInvalidArgument; }
inline bool StatusIsSuccess(const clStatus& st) { return st.code() == StatusCode::kSuccess; }
inline bool StatusIsNotFound(const clStatus& st) { return st.code() == StatusCode::kNotFound; }
inline bool StatusIsOther(const clStatus& st) { return st.code() == StatusCode::kOther; }
inline bool StatusIsPermissionDenied(const clStatus& st) { return st.code() == StatusCode::kPermissionDenied; }
inline bool StatusIsIOError(const clStatus& st) { return st.code() == StatusCode::kIOError; }
inline bool StatusIsNetError(const clStatus& st) { return st.code() == StatusCode::kNetError; }
inline bool StatusIsNeedMoreData(const clStatus& st) { return st.code() == StatusCode::kNeedMoreData; }
inline bool StatusIsProtocolError(const clStatus& st) { return st.code() == StatusCode::kProtocolError; }
inline bool StatusIsAlreadyExists(const clStatus& st) { return st.code() == StatusCode::kAlreadyExists; }
inline bool StatusIsResourceBusy(const clStatus& st) { return st.code() == StatusCode::kResourceBusy; }
inline bool StatusIsParseError(const clStatus& st) { return st.code() == StatusCode::kParseError; }
inline bool StatusIsExpandError(const clStatus& st) { return st.code() == StatusCode::kExpandError; }
inline bool StatusIsTimeout(const clStatus& st) { return st.code() == StatusCode::kTimeout; }

///-------------------
/// Helper methods
///-------------------

inline clStatus StatusOk() { return clStatus::MakeStatus(StatusCode::kSuccess); }
inline clStatus StatusInvalidArgument(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kInvalidArgument, msg);
}
inline clStatus StatusTimeout(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kTimeout, msg);
}
inline clStatus StatusResourceBusy(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kResourceBusy, msg);
}
inline clStatus StatusNotFound(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kNotFound, msg);
}
inline clStatus StatusOther(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kOther, msg);
}
inline clStatus StatusPermissionDenited(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kPermissionDenied, msg);
}
inline clStatus StatusIOError(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kIOError, msg);
}
inline clStatus StatusNetworkError(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kNetError, msg);
}
inline clStatus StatusProtocolError(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kProtocolError, msg);
}
inline clStatus StatusNeedMoreData(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kNeedMoreData, msg);
}
inline clStatus StatusAlreadyExists(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kAlreadyExists, msg);
}

inline clStatus StatusParseError(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kParseError, msg);
}

inline clStatus StatusExpandError(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kExpandError, msg);
}

///--------------------------------------
/// clStatusOr: a value or clStatus
///--------------------------------------
template <typename Value>
class clStatusOr
{
public:
    wxString error_message() const { return m_status.message(); }

    bool ok() const { return m_status.ok(); }
    bool operator!() const { return !ok(); }
    explicit operator bool() const { return ok(); }

    /// return the success result
    const Value& value() const { return m_value; }
    Value& value() { return m_value; }

    clStatusOr() {}
    clStatusOr(const clStatus& status) { *this = status; }
    clStatusOr& operator=(const clStatus& status)
    {
        m_status = status;
        return *this;
    }
    clStatusOr& operator=(const Value& v)
    {
        m_value = v;
        return *this;
    }
    clStatusOr(Value&& v) { m_value = std::move(v); }

    StatusCode code() const { return m_status.code(); }
    clStatus status() const { return m_status; }

private:
    Value m_value;
    clStatus m_status;
};

using clStatusBool = clStatusOr<bool>;
using clStatusString = clStatusOr<wxString>;
using clStatusStringView = clStatusOr<wxStringView>;
