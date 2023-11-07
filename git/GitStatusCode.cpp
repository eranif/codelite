#include "GitStatusCode.hpp"

GitStatusCode::GitStatusCode(const wxString& message)
{
    wxString msg = message.Lower();
    if(msg.Contains("username for")) {
        m_code = Code::ERROR_USER_REQUIRED;
    } else if(msg.Contains("commit-msg hook failure") || msg.Contains("pre-commit hook failure")) {
        m_code = Code::ERROR_HOOK;
    } else if(msg.Contains("*** please tell me who you are")) {
        m_code = Code::ERROR_WHO_ARE_YOU;
    } else if(msg.EndsWith("password:") || msg.Contains("password for") || msg.Contains("authentication failed")) {
        m_code = Code::ERROR_PASSWORD_REQUIRED;
    } else if((msg.Contains("the authenticity of host") && msg.Contains("can't be established")) ||
              msg.Contains("key fingerprint")) {
        m_code = Code::ERROR_AUTHENTICITIY;
    } else if(msg.Contains("fatal: unable to update url base from redirection")) {
        m_code = Code::ERROR_REDIRECT;
        int where = message.Find("redirect:");
        if(where != wxNOT_FOUND) {
            m_subText = message.Mid(where + wxStrlen("redirect:"));
            m_subText.Trim().Trim(false);
        }
    }
}

GitStatusCode::~GitStatusCode() {}
