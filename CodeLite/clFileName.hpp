#ifndef CLFILENAME_HPP
#define CLFILENAME_HPP

#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_CL clFileName : public wxFileName
{
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
     * @brief covnert to cygwin path
     */
    static wxString ToCygwin(const wxString& fullpath);
    static wxString ToCygwin(const wxFileName& fullpath);
};

#endif // CLFILENAME_HPP
