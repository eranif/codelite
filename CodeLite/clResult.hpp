#ifndef CLRESULT_HPP
#define CLRESULT_HPP

#include <wx/string.h>

template <typename T> class clResult
{
    T m_result;
    bool m_ok = false;
    wxString m_errmsg;

public:
    const wxString& error_message() const { return m_errmsg; }

    bool operator!() const { return !m_ok; }
    operator bool() const { return m_ok; }
    const T& result() const { return m_result; }

    static clResult make_error(const wxString& message)
    {
        clResult o;
        o.m_errmsg = message;
        return o;
    }

    static clResult make_success(T res)
    {
        clResult o;
        o.m_ok = true;
        std::swap(o.m_result, res);
        return o;
    }
};

typedef clResult<bool> clResultBool;
typedef clResult<wxString> clResultString;

#endif // CLRESULT_HPP
