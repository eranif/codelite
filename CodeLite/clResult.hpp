#ifndef CLRESULT_HPP
#define CLRESULT_HPP

#include <string>
#include <wx/string.h>

typedef std::basic_string_view<wxChar> wxStringView;

template <typename T, typename E> class clResult
{
    T m_result;
    bool m_ok = false;
    wxString m_errmsg;
    E m_error;

public:
    const wxString& error_message() const { return m_errmsg; }

    bool operator!() const { return !m_ok; }
    operator bool() const { return m_ok; }

    /// return the success result
    T success() const { return m_result; }

    /// return the error object
    E error() const { return m_error; }

    clResult() {}
    clResult(T&& t)
    {
        m_result = std::move(t);
        m_ok = true;
    }

    static clResult make_error(E&& error)
    {
        clResult o;
        o.m_ok = false;
        o.m_error = std::move(error);
        return o;
    }
};

typedef clResult<bool, wxString> clResultBool;
typedef clResult<wxString, wxString> clResultString;
typedef clResult<wxStringView, wxString> clResultStringView;

#endif // CLRESULT_HPP
