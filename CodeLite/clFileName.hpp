#ifndef CLFILENAME_HPP
#define CLFILENAME_HPP

#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_CL clFileName : public wxFileName
{
    bool m_isRemote = false;
    wxString m_account; // for remote file, this will hold the owner account
    wxString m_remotePath;

public:
    clFileName(const wxString& fullpath);
    clFileName(const wxString& dir, const wxString& name);
    clFileName();
    virtual ~clFileName();

    /**
     * @brief attempt to convert cygwin path to Windows native path
     */
    static wxString FromCygwin(const wxString& fullpath);

    /**
     * @brief convert to cygwin path
     */
    static wxString ToCygwin(const wxString& fullpath);
    static wxString ToCygwin(const wxFileName& fullpath);
    /**
     * @brief conversion to and from MSYS2
     */
    static wxString ToMSYS2(const wxString& fullpath);
    static wxString ToMSYS2(const wxFileName& fullpath);
    static wxString FromMSYS2(const wxString& fullpath);

    inline const wxString& GetAccount() const { return m_account; }
    inline void SetRemote(const wxString& account)
    {
        m_isRemote = true;
        m_account = account;
    }

    inline bool IsRemote() const { return m_isRemote; }
    const wxString& GetRemoteFullPath() const;
};

#endif // CLFILENAME_HPP
