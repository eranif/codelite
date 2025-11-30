#pragma once

#include "StringUtils.h" // wxStringView

#include <wx/string.h>

enum class StatusCode {
    kSucceess,
    kNotFound,
    kInvalidArgument,
    kOther,
    kPermissionDenied,
    kIOError,
    kNetError,
    kNeedMoreData,
    kProtocolError,
    kAlreadyExists,
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

    inline bool ok() const { return m_code == StatusCode::kSucceess; }

    inline wxString message() const
    {
        wxString msg;
        switch (m_code) {
        case StatusCode::kInvalidArgument:
            msg = "Invalid argument";
            break;
        case StatusCode::kSucceess:
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
        case StatusCode::kIOError:
            break;
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

    static inline clStatus MakeStatus(StatusCode code, const wxString& message = {})
    {
        clStatus st(code);
        st.m_message = message;
        return st;
    }

    inline StatusCode code() const { return m_code; }

private:
    StatusCode m_code{StatusCode::kSucceess};
    wxString m_message;
};

///-------------------
/// Helper methods
///-------------------

inline clStatus StatusOk() { return clStatus::MakeStatus(StatusCode::kSucceess); }
inline clStatus StatusInvalidArgument(const wxString& msg = wxEmptyString)
{
    return clStatus::MakeStatus(StatusCode::kInvalidArgument, msg);
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
    operator bool() const { return ok(); }

    /// return the success result
    Value value() const { return m_value; }

    clStatusOr() {}
    clStatusOr(const clStatus& status) { *this = status; }
    clStatusOr& operator=(const clStatus& status)
    {
        m_status = status;
        return *this;
    }
    clStatusOr(Value&& v) { m_value = std::move(v); }

    inline StatusCode code() const { return m_status.code(); }

private:
    Value m_value;
    clStatus m_status;
};

using clStatusBool = clStatusOr<bool>;
using clStatusString = clStatusOr<wxString>;
using clStatusStringView = clStatusOr<wxStringView>;
