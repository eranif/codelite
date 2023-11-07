#ifndef GITSTATUSCODE_HPP
#define GITSTATUSCODE_HPP

#include <wx/string.h>

class GitStatusCode final
{
public:
    enum Code {
        ERROR_PASSWORD_REQUIRED,
        ERROR_USER_REQUIRED,
        ERROR_HOOK,
        ERROR_REDIRECT,
        ERROR_WHO_ARE_YOU,
        ERROR_AUTHENTICITIY,
        ERROR_OTHER,
    };

private:
    Code m_code = Code::ERROR_OTHER;
    wxString m_subText;

public:
    GitStatusCode(const wxString& message);
    ~GitStatusCode();

    GitStatusCode::Code GetCode() const { return m_code; }
    const wxString& GetSubText() const { return m_subText; }
};

#endif // GITSTATUSCODE_HPP
